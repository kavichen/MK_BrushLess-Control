/*############################################################################
 + Regler für Brushless-Motoren
 + ATMEGA8 mit 8MHz
 + (c) 01.2007 Holger Buss
 + Nur für den privaten Gebrauch / NON-COMMERCIAL USE ONLY
 + Keine Garantie auf Fehlerfreiheit
 + Kommerzielle Nutzung nur mit meiner Zustimmung
 + Der Code ist für die Hardware BL_Ctrl V1.0 entwickelt worden
 + www.mikrocontroller.com
############################################################################*/
#include "main.h"
volatile unsigned int PPM_Signal = 0;
volatile unsigned char Timer1Overflow = 0;
volatile unsigned char PPM_Timeout = 0, anz_ppm_werte = 0;   // Ungültig, wenn Null

//############################################################################
//
void InitPPM(void)
//############################################################################
{
 TCCR1B |= (1<<ICES1)|(1<<ICNC1);
 ICP_POS_FLANKE;
 ICP_INT_ENABLE;
 TIMER1_INT_ENABLE;
}

//############################################################################
//
SIGNAL(SIG_OVERFLOW1)
//############################################################################
{
 Timer1Overflow++;
}

//############################################################################
//
SIGNAL(SIG_INPUT_CAPTURE1)
//############################################################################
{
 static unsigned int tim_alt;
 static unsigned int ppm;
 if(TCCR1B & (1<<ICES1))   // Positive Flanke
  {
   Timer1Overflow = 0;
   tim_alt = ICR1; 
   ICP_NEG_FLANKE;
   PPM_Timeout = 100;
  }
 else                      // Negative Flanke
  {
    ICP_POS_FLANKE;
#ifdef  _32KHZ 
    ppm = (ICR1 - tim_alt + (int) Timer1Overflow * 256) / 32;
#endif 
#ifdef  _16KHZ 
    ppm = (ICR1 - tim_alt + (int) Timer1Overflow * 512) / 32;
#endif 
    if(ppm < 280) ppm = 280;
    ppm -= 280;
	if(PPM_Signal < ppm)  PPM_Signal++;
	else if(PPM_Signal > ppm)  PPM_Signal--;
    if(FILTER_PPM)	ppm	= (PPM_Signal * FILTER_PPM + ppm) / (FILTER_PPM + 1);  // Filtern
    PPM_Signal = ppm;
    if(anz_ppm_werte < 255) anz_ppm_werte++;
    ZeitZumAdWandeln = 1;
  } 
}

//############################################################################
//
SIGNAL(SIG_INTERRUPT0)
//############################################################################
{
 CLR_INT0_FLAG;      // IntFlag Loeschen 
}

