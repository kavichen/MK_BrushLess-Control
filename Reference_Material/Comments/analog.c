/*############################################################################

############################################################################*/

#include "main.h"

//############################################################################
//Init ADC
void ADC_Init(void)   //ADC为模拟数据转换器
//############################################################################
{
  ADCSRA = 0xA6;  // Free Run & 1MHZ 10100110
  ADMUX = 7 | IntRef;      // Kanal IntRef为全局变量为0
  ADCSRA |= 0x40; // Start
}

//############################################################################
//Strom Analogwerte lesen 测量shunt上的电流大小
void AdConvert(void)
//############################################################################
{
 unsigned int i=0;  
 unsigned char sense;
  sense = ADMUX;   // Sense-Kanal merken
  ADMUX  =  0x06 | IntRef;  // Kanal 6
  SFIOR  =  0x00;  // Analog Comperator aus
  ADCSRA =  0xD3;  // Converter ein, single
  ADCSRA |= 0x10;  // Ready löschen
  ADMUX  =  0x06 | IntRef;  // Kanal 6
  ADCSRA |= 0x40;  // Start
  while (((ADCSRA & 0x10) == 0));
  ADMUX = sense;   // zurück auf den Sense-Kanal
  i = ADCW * 4;
//  if(i > 300) i = 300;
  Strom = (i + Strom * 7) / 8;  //数值滤波，减弱一些尖峰电流对测量值的影响,将本次测量的值取1/8权重，strom取7/8权重
  if (Strom_max < Strom) Strom_max = Strom;
  ADCSRA = 0x00;  
  SFIOR = 0x08;  // Analog Comperator ein
}



//############################################################################
//测量某一ADC通道的电压值并将其返回,channel为需要测量的通道号
unsigned int MessAD(unsigned char channel)
//############################################################################
{
 unsigned char sense;
  sense = ADMUX;   // Sense-Kanal merken
  channel |= IntRef;
  ADMUX  =  channel;  // Kanal 6
  SFIOR  =  0x00;  // Analog Comperator aus
  ADCSRA =  0xD3;  // Converter ein, single
  ADCSRA |= 0x10;  // Ready löschen
  ADMUX  =  channel;  // Kanal 6
  ADCSRA |= 0x40;  // Start
  while (((ADCSRA & 0x10) == 0));
  ADMUX = sense;   // zurück auf den Sense-Kanal
  ADCSRA = 0x00;  
  SFIOR = 0x08;  // Analog Comperator ein
return(ADCW);
}

//############################################################################
//Strom Analogwerte lesen 快速测量shunt上的电流大小
void FastADConvert(void)
//############################################################################
{
 unsigned int i=0;  
  i = MessAD(6) * 4;
//  i = ADCW * 4;
  if(i > 200) i = 200;
  Strom = i;//(i + Strom * 1) / 2; 
  if (Strom_max < Strom) Strom_max = Strom;
  ADCSRA = 0x00;  
  SFIOR = 0x08;  // Analog Comperator ein
}
