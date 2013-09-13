/*############################################################################
############################################################################*/

#ifndef BLMC_H_
#define BLMC_H_

extern volatile unsigned char Phase;  //相
extern volatile unsigned char ShadowTCCR1A; //TCCR1A的镜像
extern volatile unsigned char CompInterruptFreigabe; //比较中断释放

void Blc(void);
void Manuell(void);

#ifdef  _32KHZ 
 #define PWM_C_ON  {TCCR1A = 0xAD; TCCR2  = 0x49;DDRB = 0x0A;} 
 #define PWM_B_ON  {TCCR1A = 0xAD; TCCR2  = 0x49;DDRB = 0x0C;}
 #define PWM_A_ON  {TCCR1A = 0xAD; TCCR2  = 0x69;DDRB = 0x08;}
 #define PWM_OFF   {TCCR1A = 0x0D; TCCR2  = 0x49;PORTC &= ~0x0E;}
#endif 

#ifdef  _16KHZ 
 #define PWM_C_ON  {TCCR1A = 0xA2; TCCR2  = 0x41; DDRB = 0x0A;} 
 #define PWM_B_ON  {TCCR1A = 0xA2; TCCR2  = 0x41; DDRB = 0x0C;}
 #define PWM_A_ON  {TCCR1A = 0xA2; TCCR2  = 0x61; DDRB = 0x08;}
 #define PWM_OFF   {TCCR1A = 0x02; TCCR2  = 0x41;PORTC &= ~0x0E;}
#endif 


#define  STEUER_A_H {PWM_A_ON}
#define  STEUER_B_H {PWM_B_ON}
#define  STEUER_C_H {PWM_C_ON}

#define  STEUER_A_L {PORTD &= ~0x30; PORTD |= 0x08;}
#define  STEUER_B_L {PORTD &= ~0x28; PORTD |= 0x10;}
#define  STEUER_C_L {PORTD &= ~0x18; PORTD |= 0x20;}
#define  STEUER_OFF {PORTD &= ~0x38; PWM_OFF;}

#define SENSE_A ADMUX = 0;      //ADC0 PC0 接NULL_A
#define SENSE_B ADMUX = 1;      //ADC1 PC1 接NULL_B
#define SENSE_C ADMUX = 2;      //ADC2 PC2 接NULL_C

#define ClrSENSE            ACSR |= 0x10    //清除模拟比较器中断标志
#define SENSE               ((ACSR & 0x10)) //模拟比较器中断标志置位时,SENSE为1
#define SENSE_L             (!(ACSR & 0x20))//模拟比较器输出为0时,SENSE_L为1
#define SENSE_H             ((ACSR & 0x20)) //模拟比较器输出为1时,SENSE_H为1
#define ENABLE_SENSE_INT    {CompInterruptFreigabe = 1;ACSR |= 0x0A; } //模拟比较器中断使能,下降沿触发
#define DISABLE_SENSE_INT   {CompInterruptFreigabe = 0; ACSR &= ~0x08; } //模拟比较器中断禁止


#define SENSE_FALLING_INT    ACSR &= ~0x01 //模拟比较器输出的下降沿产生中断
#define SENSE_RISING_INT    ACSR |= 0x03   //模拟比较器输出的上升沿产生中断
#define SENSE_TOGGLE_INT    ACSR &= ~0x03  //模拟比较器输出的发生变化产生中断

#endif //BLMC_H_

