/*############################################################################
############################################################################*/

#ifndef BLMC_H_
#define BLMC_H_

extern volatile unsigned char Phase;
extern volatile unsigned char ShadowTCCR1A;
extern volatile unsigned char CompInterruptFreigabe;

void Blc(void);
void Manuell(void);

// anselm
/*
#define COM1A		((0 << COM1A0) | (1 << COM1A1))		// COM1A-> OC1A non inverting mode
#define COM1B		((0 << COM1B0) | (1 << COM1B1))		// COM1B-> OC1B non inverting mode
#define COM2		((0 << COM20) | (1 << COM21))		// COM2-> OC2 non inverting mode

#ifdef  _32KHZ
 #define WGM1		((1 << WGM10) | (0 << WGM11))				// WGM10:13-> fast PWM 8bit
 #define WGMCS2		((1 << WGM20) | (1 << WGM21) | (1 << CS20))	// WGM20:21-> fast PWM, no prescale
#endif

#ifdef  _16KHZ
 #define WGM1		((1 << WGM10) | (0 << WGM11))				// WGM10:13-> phase corr, PWM 8bit
 #define WGMCS2		((1 << WGM20) | (0 << WGM21) | (1 << CS20))	// WGM20:21-> phase corr. PWM, no prescale
#endif

#define PWM_C_ON  {TCCR1A = COM1A | WGM1 | COM1B; TCCR2  = WGMCS2; 
				   DDRB = 0x02;}						// Steuer_C+ output
#define PWM_B_ON  {TCCR1A = COM1B | WGM1 | COM1A; TCCR2  = WGMCS2; \
 				   DDRB = 0x04;}						// Steuer_B+ output
#define PWM_A_ON  {TCCR1A = WGM1; TCCR2  = COM2 | WGMCS2; \
 				   DDRB = 0x08;}						// Steuer_A+ output
#define PWM_OFF   {TCCR1A = WGM1; \
 				   TCCR2  = WGMCS2; \
 				   PORTB &= ~0x0E; DDRB = 0x0E;}		// OC1x & OC2 disconnected, Steuer_X+ output low
// anselm
*/

#ifdef  _32KHZ 
 #define PWM_C_ON  {TCCR1A = 0xAD; TCCR2  = 0x49;DDRB = 0x0A;} 
 #define PWM_B_ON  {TCCR1A = 0xAD; TCCR2  = 0x49;DDRB = 0x0C;}
 #define PWM_A_ON  {TCCR1A = 0xAD; TCCR2  = 0x69;DDRB = 0x08;}
 #define PWM_OFF   {TCCR1A = 0x0D; TCCR2  = 0x49;PORTC &= ~0x0E;}
#endif 

#ifdef  _16KHZ 
// #define PWM_C_ON  {TCCR1A = 0xA2; TCCR2  = 0x41; DDRB = 0x0A;} 
// #define PWM_B_ON  {TCCR1A = 0xA2; TCCR2  = 0x41; DDRB = 0x0C;}
// #define PWM_A_ON  {TCCR1A = 0xA2; TCCR2  = 0x61; DDRB = 0x08;}

// #define PWM_C_ON  {TCCR2  = 0x41; if(PPM_Betrieb) { TCCR1A = 0xA1;DDRB = 0x0A;} else { TCCR1A = 0x81; DDRB = 0x0E;}} 
// #define PWM_B_ON  {TCCR2  = 0x41; if(PPM_Betrieb) { TCCR1A = 0xA1;DDRB = 0x0C;} else { TCCR1A = 0x21; DDRB = 0x0E;}} 
// #define PWM_A_ON  {TCCR2  = 0x61; if(PPM_Betrieb) { TCCR1A = 0xA1;DDRB = 0x08;} else { TCCR1A = 0x01; DDRB = 0x0E;}} 

#define PWM_C_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x02;} 
#define PWM_B_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x04;}
#define PWM_A_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x08;}


// #define PWM_C_ON  {TCCR1A = 0x82; TCCR2  = 0x41; PORTB &= ~0x04; DDRB = 0x0E;} 
// #define PWM_B_ON  {TCCR1A = 0x22; TCCR2  = 0x41; PORTB &= ~0x02; DDRB = 0x0E;}
// #define PWM_A_ON  {TCCR1A = 0x02; TCCR2  = 0x61; PORTB &= ~0x06; DDRB = 0x0E;}

#define PWM_OFF   {OCR1A = 0; OCR1B = 0; OCR2  = 0; TCCR1A = 0x01; TCCR2  = 0x41; DDRB = 0x0E; PORTB &= ~0x0E;}
#endif 

#define  STEUER_A_H {PWM_A_ON}
#define  STEUER_B_H {PWM_B_ON}
#define  STEUER_C_H {PWM_C_ON}

#define  STEUER_A_L {PORTD &= ~0x30; PORTD |= 0x08;}
#define  STEUER_B_L {PORTD &= ~0x28; PORTD |= 0x10;}
#define  STEUER_C_L {PORTD &= ~0x18; PORTD |= 0x20;}
#define  STEUER_OFF {PORTD &= ~0x38; PWM_OFF; }
#define  FETS_OFF {PORTD &= ~0x38; PORTB &= ~0x0E; }

#define SENSE_A ADMUX = 0 | IntRef;
#define SENSE_B ADMUX = 1 | IntRef;
#define SENSE_C ADMUX = 2 | IntRef;

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

