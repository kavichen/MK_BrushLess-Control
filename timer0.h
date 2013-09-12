#if defined (__AVR_ATmega8__)
#define TIMER_TEILER          CK8
//#define TIMER_RELOAD_VALUE    125
#endif


extern volatile unsigned int CountMilliseconds;
extern volatile unsigned char Timer0Overflow;


void Timer1_Init(void);
void Delay_ms(unsigned int);
unsigned int SetDelay (unsigned int t);
char CheckDelay (unsigned int t);

