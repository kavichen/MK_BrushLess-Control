#ifndef BLMC_H_
#define BLMC_H_

extern volatile unsigned char Phase;
extern volatile unsigned char ShadowTCCR1A;
extern volatile unsigned char CompInterruptFreigabe;

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

#define SENSE_A ADMUX = 0;
#define SENSE_B ADMUX = 1;
#define SENSE_C ADMUX = 2;

#define ClrSENSE            ACSR |= 0x10
#define SENSE               ((ACSR & 0x10))
#define SENSE_L             (!(ACSR & 0x20))
#define SENSE_H             ((ACSR & 0x20))
#define ENABLE_SENSE_INT    {CompInterruptFreigabe = 1;ACSR |= 0x0A; }
#define DISABLE_SENSE_INT   {CompInterruptFreigabe = 0; ACSR &= ~0x08; }


#define SENSE_FALLING_INT    ACSR &= ~0x01
#define SENSE_RISING_INT    ACSR |= 0x03
#define SENSE_TOGGLE_INT    ACSR &= ~0x03

#endif //BLMC_H_
