#include "main.h"

unsigned int  PWM = 0;
unsigned int  Strom = 0; //ca. in 0,1A
unsigned char Strom_max = 0;
unsigned char Mittelstrom = 0;
unsigned int  Drehzahl = 0;  // in 100UPM  60 = 6000
unsigned int  KommutierDelay = 10;
unsigned int  I2C_Timeout = 0;
unsigned char SIO_Timeout = 0;
unsigned int  SollDrehzahl = 0;
unsigned int  IstDrehzahl = 0;
unsigned int  DrehZahlTabelle[256];//vorberechnete Werte zur Drehzahlerfassung
unsigned char ZeitFuerBerechnungen = 1;
unsigned char MotorAnwerfen = 0;
unsigned char MotorGestoppt = 1;
unsigned char MaxPWM = MAX_PWM;
unsigned int  CntKommutierungen = 0;
unsigned int  SIO_Drehzahl = 0;
unsigned char ZeitZumAdWandeln = 1;


//############################################################################
//
void SetPWM(void)
//############################################################################
{
    unsigned char tmp_pwm; 
    tmp_pwm = PWM;
    if(tmp_pwm > MaxPWM)    // Strombegrenzung
        {
        tmp_pwm = MaxPWM;
        PORTC |= ROT; 
        } 
    if(Strom > MAX_STROM)   // Strombegrenzung
        {
        OCR1A = 0; OCR1B = 0; OCR2  = 0;
        PORTC |= ROT; 
        Strom--;
        }
    else 
        {
        #ifdef  _32KHZ 
        OCR1A =  tmp_pwm; OCR1B =  tmp_pwm; OCR2  = tmp_pwm;
        #endif 

        #ifdef  _16KHZ 
        OCR1A = 2 * (int)tmp_pwm; OCR1B = 2 * (int)tmp_pwm; OCR2  = tmp_pwm;
        #endif 
        }
}

//############################################################################
//
void PWM_Init(void)
//############################################################################
{
    PWM_OFF;
    TCCR1B = (1 << CS10) | (0 << CS11) | (0 << CS12) | (1 << WGM12) | 
             (0 << WGM13) | (0<< ICES1) | (0 << ICNC1);
}

//############################################################################
//
void Wait(unsigned char dauer)
//############################################################################
{
    dauer = (unsigned char)TCNT0 + dauer;
    while((TCNT0 - dauer) & 0x80);
}

//############################################################################
//
void Anwerfen(unsigned char pwm)
//############################################################################
{
    unsigned long timer = 300,i;
    DISABLE_SENSE_INT;
    PWM = 5;
    SetPWM();
    Manuell();
    Delay_ms(200);
    PWM = pwm;
    while(1)
        {
        for(i=0;i<timer; i++) 
            {
            if(!UebertragungAbgeschlossen)  SendUart();
            else DatenUebertragung();
            Wait(100);  // warten
            } 
        timer-= timer/15+1;
        if(timer < 25) { if(TEST_MANUELL) timer = 25; else return; }

        Manuell();
        Phase++;
        Phase %= 6;
        AdConvert();
        PWM = pwm;
        SetPWM();
        if(SENSE) 
            {
            PORTD ^= GRUEN;
            } 
        }
}

//############################################################################
//
unsigned char SollwertErmittlung(void)
//############################################################################
{
    static unsigned int sollwert = 0;
    unsigned int ppm;
    if(!I2C_Timeout)   // bei Erreichen von 0 ist der Wert ung¨¹ltig
        {
        if(SIO_Timeout)  // es gibt g¨¹ltige SIO-Daten
            {
            sollwert =  (MAX_PWM * (unsigned int) SIO_Sollwert) / 200;  // skalieren auf 0-200 = 0-255
            }
        else
            if(PPM_Timeout)  // es gibt g¨¹ltige PPM-Daten
                {
                ppm = PPM_Signal;
                if(ppm > 300) ppm =   0;  // ung¨¹ltiges Signal
                if(ppm > 200) ppm = 200;
                if(ppm <= MIN_PPM) sollwert = 0;
                else 
                    {
                    sollwert = (int) MIN_PWM + ((MAX_PWM - MIN_PWM) * (ppm - MIN_PPM)) / (190 - MIN_PPM);
                    }
                PORTC &= ~ROT;
                }
            else   // Kein g¨¹ltiger Sollwert
                {
                if(!TEST_SCHUB) { if(sollwert) sollwert--; }   
                PORTC |= ROT; 
                }
        }
    else // I2C-Daten sind g¨¹ltig
        {
        sollwert = I2C_RXBuffer; 
        PORTC &= ~ROT;
        }
    if(sollwert > MAX_PWM) sollwert = MAX_PWM;
    return(sollwert); 
}

void DebugAusgaben(void)
{
    DebugOut.Analog[0] = Strom;
    DebugOut.Analog[1] = Mittelstrom;
    DebugOut.Analog[2] = SIO_Drehzahl;
    DebugOut.Analog[3] = PPM_Signal;
}


//############################################################################
//Hauptprogramm
int main (void)
//############################################################################
{
    char altPhase = 0;
    int test = 0;
    unsigned int MinUpmPulse,Blink,TestschubTimer;
    unsigned int Blink2,MittelstromTimer,DrehzahlMessTimer,MotorGestopptTimer;

    DDRC  = 0x08;
    PORTC = 0x08;
    DDRD  = 0xBA;
    PORTD = 0x80;
    DDRB  = 0x0E;
    PORTB = 0x31;
    
    UART_Init();
    Timer0_Init();
    sei ();//Globale Interrupts Einschalten
    
    // Am Blinken erkennt man die richtige Motoradresse
    for(test=0;test<5;test++)
        {
        if(test == MOTORADRESSE) PORTD |= GRUEN;
        Delay_ms(150);
        PORTD &= ~GRUEN;
        Delay_ms(250);
        }       

    Delay_ms(500);
   
    UART_Init();
    PWM_Init(); 

    InitIC2_Slave(0x50);                            
    InitPPM();

    Blink             = SetDelay(101);    
    Blink2            = SetDelay(102);
    MinUpmPulse       = SetDelay(103);
    MittelstromTimer  = SetDelay(254);
    DrehzahlMessTimer = SetDelay(1005);
    TestschubTimer    = SetDelay(1006);
    while(!CheckDelay(MinUpmPulse));
    PORTD |= GRUEN;
    PWM = 0;

    SetPWM();

    SFIOR = 0x08;  // Analog Comperator ein
    ADMUX = 1; 

    MinUpmPulse = SetDelay(10);
    DebugOut.Analog[1] = 1;
    PPM_Signal = 0;

    // zum Test der Hardware; Motor dreht mit konstanter Drehzahl ohne Regelung
    if(TEST_MANUELL)    Anwerfen(TEST_MANUELL);  // kommt von dort nicht wieder

    while (1)
        {
        if(!TEST_SCHUB)   PWM = SollwertErmittlung();
        //I2C_TXBuffer = PWM; // Antwort ¨¹ber I2C-Bus
        if(MANUELL_PWM)   PWM = MANUELL_PWM;

        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(Phase != altPhase)   // es gab eine Kommutierung im Interrupt
            {
            MotorGestoppt = 0;
            ZeitFuerBerechnungen = 0;    // direkt nach einer Kommutierung ist Zeit 
            MinUpmPulse = SetDelay(50);  // Timeout, falls ein Motor stehen bleibt
            altPhase = Phase;
            }
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!PWM)    // Sollwert == 0
            {
            MotorAnwerfen = 0;      // kein Startversuch
            ZeitFuerBerechnungen = 0;
            // nach 1,5 Sekunden den Motor als gestoppt betrachten 
            if(CheckDelay(DrehzahlMessTimer)) 
                {
                DISABLE_SENSE_INT;
                MotorGestoppt = 1;  
                STEUER_OFF;
                } 
            }
        else 
            {
            if(MotorGestoppt) MotorAnwerfen = 1;        // Startversuch
            MotorGestopptTimer = SetDelay(1500);
            }

        if(MotorGestoppt && !TEST_SCHUB) PWM = 0;
        SetPWM();
        // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!ZeitFuerBerechnungen++)
            {
            if(MotorGestoppt) PORTD |= GRUEN; //else PORTD &= ~GRUEN;
            if(SIO_DEBUG)
                {
                DebugAusgaben();  // welche Werte sollen angezeigt werden?
                if(!UebertragungAbgeschlossen)  SendUart(); 
                else DatenUebertragung();
                }
            // Berechnen des Mittleren Stroms zur (langsamen) Strombegrenzung
            if(CheckDelay(MittelstromTimer))   
                {
                MittelstromTimer = SetDelay(50); // alle 50ms
                if(Mittelstrom <  Strom) Mittelstrom++;// Mittelwert des Stroms bilden
                else if(Mittelstrom >  Strom) Mittelstrom--;
       
                if(Mittelstrom > LIMIT_STROM)// Strom am Limit?
                    {
                    MaxPWM--;// dann die Maximale PWM herunterfahren
                    PORTC |= ROT; 
                    }
                else 
                    {
                    if(MaxPWM < MAX_PWM) MaxPWM++;
                    }
                }

            if(CheckDelay(DrehzahlMessTimer))   // Ist-Drehzahl bestimmen
                {
                DrehzahlMessTimer = SetDelay(10);
                SIO_Drehzahl = (6 * CntKommutierungen) / (POLANZAHL / 2);
                CntKommutierungen = 0;
                if(PPM_Timeout == 0) // keine PPM-Signale
                ZeitZumAdWandeln = 1;
                }


            if(CheckDelay(TestschubTimer))  
                {
                TestschubTimer = SetDelay(1500);
                if(TEST_SCHUB)
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
            // Motor Stehen geblieben
            if((CheckDelay(MinUpmPulse) && SIO_Drehzahl == 0) || MotorAnwerfen) 
                {
                MotorGestoppt = 1;    
                DISABLE_SENSE_INT;
                MinUpmPulse = SetDelay(100);         
                if(MotorAnwerfen)
                    {
                    PORTC &= ~ROT;
                    MotorAnwerfen = 0;
                    Anwerfen(10);
                    PORTD |= GRUEN;
                    MotorGestoppt = 0;    
                    Phase--;
                    PWM = 1;
                    SetPWM();
                    SENSE_TOGGLE_INT;
                    ENABLE_SENSE_INT;
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
