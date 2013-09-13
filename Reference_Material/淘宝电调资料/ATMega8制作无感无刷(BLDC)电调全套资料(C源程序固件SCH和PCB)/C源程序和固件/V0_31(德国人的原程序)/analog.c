#include "main.h"

//############################################################################
//Init ADC
void ADC_Init(void)
//############################################################################
{
  ADCSRA = 0xA6;  // Free Run & 1MHZ
  ADMUX = 7;      // Kanal 7
  ADCSRA |= 0x40; // Start
}

//############################################################################
//Strom Analogwerte lesen
void AdConvert(void)
//############################################################################
{
 unsigned int i=0;  
 unsigned char sense;
  sense = ADMUX;   // Sense-Kanal merken
  ADMUX  =  0x06;  // Kanal 6
  SFIOR  =  0x00;  // Analog Comperator aus
  ADCSRA =  0xD3;  // Converter ein, single
  ADCSRA |= 0x10;  // Ready l?schen
  ADMUX  =  0x06;  // Kanal 6
  ADCSRA |= 0x40;  // Start
  while (((ADCSRA & 0x10) == 0));
  ADMUX = sense;   // zur¨¹ck auf den Sense-Kanal
  i = ADCW * 4;
  if(i > 200) i = 200;
  Strom = (i + Strom * 7) / 8; 
  if (Strom_max < Strom) Strom_max = Strom;
  ADCSRA = 0x00;  
  SFIOR = 0x08;  // Analog Comperator ein
}
