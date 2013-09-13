/*############################################################################
 + 无刷电机控制器
 + ATMEGA8 At 8MHz
 + (c) 01.2007 Holger Buss
 + 只作私人用途
 + 准确性没有保证
 + 未经我同意不得商业使用
 + 该代码使用BL_Ctrl 1.0版已经开发的硬件
 + www.mikrocontroller.com
/############################################################################*/

#include "main.h"

unsigned int  PWM = 0;
unsigned int  Strom = 0; //ca. in 0,1A    //电流
unsigned char Strom_max = 0;              //最大电流  
unsigned char Mittelstrom = 0;            //平均电流
unsigned int  Drehzahl = 0;  // in 100UPM  60 = 6000  //转速
unsigned int  KommutierDelay = 10; //换向延时
unsigned int  I2C_Timeout = 0;
unsigned char SIO_Timeout = 0;
unsigned int  SollDrehzahl = 0;  //额定转速，计算转速
unsigned int  IstDrehzahl = 0;   //实际转速
unsigned int  DrehZahlTabelle[256];//vorberechnete Werte zur Drehzahlerfassung //转速表
unsigned char ZeitFuerBerechnungen = 1; //计算所用时间
unsigned char MotorAnwerfen = 0; //电机启动，（加速
unsigned char MotorGestoppt = 1; //电机停止
unsigned char MaxPWM = MAX_PWM;
unsigned int  CntKommutierungen = 0; //换向计数
unsigned int  SIO_Drehzahl = 0;      //串口速度
unsigned char ZeitZumAdWandeln = 1;   //到AD转换的时间


//############################################################################
//
void SetPWM(void)
//############################################################################
{
    unsigned char tmp_pwm; 
    tmp_pwm = PWM;
    if(tmp_pwm > MaxPWM)    // 电流限制
        {
        tmp_pwm = MaxPWM;
        PORTC |= ROT; //红色LED亮
        } 
    if(Strom > MAX_STROM)   // 电流限制
        {
        OCR1A = 0; OCR1B = 0; OCR2  = 0;
        PORTC |= ROT; //红色LED亮
        Strom--;
        }
    else 
        {
        #ifdef  _32KHZ 
        OCR1A =  tmp_pwm; 
        OCR1B =  tmp_pwm; 
        OCR2  = tmp_pwm;
        #endif 

        #ifdef  _16KHZ //16kHZ时T1用的是9位快速的pwm,所以赋值要加倍，T2用的是相位修正，不用加倍
        OCR1A = 2 * (int)tmp_pwm; 
        OCR1B = 2 * (int)tmp_pwm; 
        OCR2  = tmp_pwm;
        #endif 
        }
}

//############################################################################
//
void PWM_Init(void)  //PWM初始化
//############################################################################
{
    PWM_OFF;
    TCCR1B = (1 << CS10) | (0 << CS11) | (0 << CS12) | (1 << WGM12) | 
             (0 << WGM13) | (0<< ICES1) | (0 << ICNC1);
             // T/C1 无预分频,下降沿触发输入捕捉,不使用输入捕捉噪声抑制功能,
             // 9位快速PWM,比较匹配时清零OC1A/OC1B(输出低电平)
}

//############################################################################
//
void Wait(unsigned char dauer)   //dauer 持续时间
//############################################################################
{
    dauer = (unsigned char)TCNT0 + dauer;
    while((TCNT0 - dauer) & 0x80);
}

//############################################################################
//
void Anwerfen(unsigned char pwm) //启动马达
//############################################################################
/*补充注释：
开环启动算法
*/
{
    unsigned long timer = 300,i;
    DISABLE_SENSE_INT;   //关闭模拟比较器中断
    PWM = 5;
    SetPWM();//T1和T2定时计数器赋值，设置pwm占空比
    Manuell();
    /*补充注释： 
开环顺序换向算法，注意换向时必须同步修改比较器端口及触发沿 
以便在反相感生电动势到达切换条件时，自动切换到闭环运转状态 
*/////////////////////////////////////////////////// 
    Delay_ms(200);
    PWM = pwm;
    while(1)
        {
        for(i=0;i<timer; i++) 
            {
            //if(!UebertragungAbgeschlossen)  SendUart(); //UebertragungAbgeschlossen完成转换
            //else DatenUebertragung();  //数据传输
            Wait(100);  // warten 等待
            } 
        timer-= timer/15+1;
        if(timer < 25) { if(TEST_MANUELL) timer = 25; else return; }

        Manuell();
        Phase++;
        Phase %= 6;
        AdConvert();  //测量电流
        PWM = pwm;
        SetPWM();
        if(SENSE) //如果模拟比较器中断标志置位,绿灯亮
            {
            PORTD ^= GRUEN;
            } 
        }
}

//############################################################################
//
unsigned char SollwertErmittlung(void) //设定值的测量值
//############################################################################
{
    static unsigned int sollwert = 0;
    //unsigned int ppm;
    if(!I2C_Timeout)   // bei Erreichen von 0 ist der Wert ung黮tig
        {
        if(SIO_Timeout)  // es gibt g黮tige SIO-Daten
            {
            //sollwert =  (MAX_PWM * (unsigned int) SIO_Sollwert) / 200;  // skalieren auf 0-200 = 0-255
            }
        else
		/*
            if(PPM_Timeout)  // es gibt g黮tige PPM-Daten
                {
                ppm = PPM_Signal;
                if(ppm > 300) ppm =   0;  // ung黮tiges Signal
                if(ppm > 200) ppm = 200;
                if(ppm <= MIN_PPM) sollwert = 0;
                else 
                    {
                    sollwert = (int) MIN_PWM + ((MAX_PWM - MIN_PWM) * (ppm - MIN_PPM)) / (190 - MIN_PPM);
                    }
                PORTC &= ~ROT;
                }
            else   // Kein g黮tiger Sollwert
                {*/
                if(!TEST_SCHUB) { if(sollwert) sollwert--; }   
                PORTC |= ROT; 
                /*}*/
			
        }
    else // I2C-Daten sind g黮tig
        {
        sollwert = I2C_RXBuffer; 
        PORTC &= ~ROT;
        }
    if(sollwert > MAX_PWM) sollwert = MAX_PWM;
    return(sollwert); 
}

//############################################################################
//主程序
int main (void)
//############################################################################
{
    char altPhase = 0;
    int test = 0;
    unsigned int MinUpmPulse,Blink,TestschubTimer;
    unsigned int Blink2,MittelstromTimer,DrehzahlMessTimer,MotorGestopptTimer;

    DDRC  = 0x08;//PC3输出，控制红色led DDRx中的位DDxn等于0时，I/O端口引脚工作在输入方式，1为输出
    PORTC = 0x08;//PC3输出高电平，红色led亮
    DDRD  = 0xBA;//10111010A-B-C-配置为输出，TXD,LED_GRN(PD7)配置为输出,MITTEL/RXD/INT0为输入
    PORTD = 0x80;//LED_GRN高电平，绿色灯亮 
    DDRB  = 0x0E;////A+B+C+配置为输出,其他为输入，ppm的ICP为输入
    PORTB = 0x31;//I2C总线(SDA/SCL)为高电平，ICP上拉电阻使能,其他低电平，因为有三极管截止，所以p沟道不导通
    
    //UART_Init();
    Timer0_Init();
    sei ();//打开全局中断
    
    //绿灯闪烁次数标识的电机地址，用于4轴的4个电调判断
    for(test=0;test<5;test++)
        {
        if(test == MOTORADRESSE) PORTD |= GRUEN;//绿灯亮
        Delay_ms(150);
        PORTD &= ~GRUEN;//绿灯灭 
        Delay_ms(250);
        }	

    Delay_ms(500);
   
    //UART_Init();
    PWM_Init();	

    //InitIC2_Slave(0x50);
    InitPPM();

    Blink             = SetDelay(101);    
    Blink2            = SetDelay(102);
    MinUpmPulse       = SetDelay(103); //最小转速脉冲
    MittelstromTimer  = SetDelay(254); //中点电流计时器
    DrehzahlMessTimer = SetDelay(1005);//测速计时器
    TestschubTimer    = SetDelay(1006);//推力测试计时器
    while(!CheckDelay(MinUpmPulse));
    PORTD |= GRUEN;//LED_GRN高电平，绿灯亮
    PWM = 0;

    SetPWM();

    SFIOR = 0x08;  // 模拟比较器多路复用器使能
    ADMUX = 1;     //ADC1作为模拟比较器的输入负极

    MinUpmPulse = SetDelay(10);
    //DebugOut.Analog[1] = 1;
    //PPM_Signal = 0;

    // zum Test der Hardware; Motor dreht mit konstanter Drehzahl ohne Regelung///测试硬件,马达转速不受控制
    if(TEST_MANUELL)    Anwerfen(TEST_MANUELL);  // kommt von dort nicht wieder//到再次没有重建时
                                                 //Anwerfen：启动，加速    
    while (1)
        {
        if(!TEST_SCHUB)   PWM = SollwertErmittlung();//额定值计算 
        //I2C_TXBuffer = PWM; // Antwort 黚er I2C-Bus
        if(MANUELL_PWM)   PWM = MANUELL_PWM;

        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(Phase != altPhase)   // es gab eine Kommutierung im Interrupt如果相位不等于旧的相位，换向
            {
            MotorGestoppt = 0;
            ZeitFuerBerechnungen = 0;    // direkt nach einer Kommutierung ist Zeit直接在某个之后计算实际时间 
            MinUpmPulse = SetDelay(50);  // Timeout, falls ein Motor stehen bleibt暂停，电机停止
            altPhase = Phase;//旧相位=新相位 
            }
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!PWM)    // Sollwert == 0
            {
            MotorAnwerfen = 0;      // kein Startversuch无启动试验
            ZeitFuerBerechnungen = 0;//计算所用的时间 
            // nach 1,5 Sekunden den Motor als gestoppt betrachten 在1，5瞬间之后电机停止检测
            if(CheckDelay(DrehzahlMessTimer)) 
                {
                DISABLE_SENSE_INT;
                MotorGestoppt = 1;  //电机停止标志位？
                STEUER_OFF;
                } 
            }
        else 
            {
            if(MotorGestoppt) MotorAnwerfen = 1;	// Startversuch启动试验
            MotorGestopptTimer = SetDelay(1500);
            }

        if(MotorGestoppt && !TEST_SCHUB) PWM = 0;
        SetPWM();
        // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!ZeitFuerBerechnungen++)//计算所用的时间
            {
            if(MotorGestoppt) PORTD |= GRUEN; //else PORTD &= ~GRUEN; //else PORTD &= ~GRUEN;//LED_GRN高电平，灯亮，否则灭
            if(SIO_DEBUG)
                {
                //DebugAusgaben();  // welche Werte sollen angezeigt werden?
                //if(!UebertragungAbgeschlossen)  SendUart(); 
                //else DatenUebertragung();
                }
            // Berechnen des Mittleren Stroms zur (langsamen) Strombegrenzung计算现在的平均电流（缓慢的）电流限制
            if(CheckDelay(MittelstromTimer))   
                {
                MittelstromTimer = SetDelay(50);  // alle 50ms每个50ms
                if(Mittelstrom <  Strom) Mittelstrom++;// Mittelwert des Stroms bilden平均电流形成
                else if(Mittelstrom >  Strom) Mittelstrom--;
       
                if(Mittelstrom > LIMIT_STROM)// Strom am Limit?
                    {
                    MaxPWM--;// dann die Maximale PWM herunterfahren最大限度pwm关闭
                    PORTC |= ROT; //#define ROT    0x08， LED_ROT亮
                    }
                else 
                    {
                    if(MaxPWM < MAX_PWM) MaxPWM++;
                    }
                }

            if(CheckDelay(DrehzahlMessTimer))   // Ist-Drehzahl bestimmen实际转速测定
                {
                DrehzahlMessTimer = SetDelay(10);
                SIO_Drehzahl = (6 * CntKommutierungen) / (POLANZAHL / 2);//管子换向
                CntKommutierungen = 0;
                //if(PPM_Timeout == 0) // keine PPM-Signale
                //ZeitZumAdWandeln = 1;
                }


            if(CheckDelay(TestschubTimer))  
                {
                TestschubTimer = SetDelay(1500);
                if(TEST_SCHUB)//   #define TEST_SCHUB    0  不执行
                    {
                    switch(test) 
                        {
                        case 0: PWM = 50; test++; break;
                        case 1: PWM = 130; test++; break;
                        case 2: PWM = 60;  test++; break;
                        case 3: PWM = 140; test++; break;
                        case 4: PWM = 150; test = 0; break;
                        default: test = 0;
                        } 
                    }
                }
            // Motor Stehen geblieben电机停止遗留
            if((CheckDelay(MinUpmPulse) && SIO_Drehzahl == 0) || MotorAnwerfen) 
                {
                MotorGestoppt = 1;    //电机停止标志1
                DISABLE_SENSE_INT;
                MinUpmPulse = SetDelay(100);         
                if(MotorAnwerfen)
                    {
                    PORTC &= ~ROT;//LED_ROT灭
                    MotorAnwerfen = 0;//电机启动标志0
                    Anwerfen(10);//启动
                    PORTD |= GRUEN;//LED_GRN亮
                    MotorGestoppt = 0;    //电机停止标志0  
                    Phase--;
                    PWM = 1;
                    SetPWM();
                    SENSE_TOGGLE_INT;//输出电平变化就中断
                    ENABLE_SENSE_INT;//开比较器中断
                    MinUpmPulse = SetDelay(100);
                    while(!CheckDelay(MinUpmPulse)); // kurz Synchronisieren
                    PWM = 10;
                    SetPWM();
                    MinUpmPulse = SetDelay(200);
                    while(!CheckDelay(MinUpmPulse)); // kurz Durchstarten
                    MinUpmPulse = SetDelay(1000);
                    altPhase = 7;
                    }
                }
            } // ZeitFuerBerechnungen
        } // while(1) - Hauptschleife
}

