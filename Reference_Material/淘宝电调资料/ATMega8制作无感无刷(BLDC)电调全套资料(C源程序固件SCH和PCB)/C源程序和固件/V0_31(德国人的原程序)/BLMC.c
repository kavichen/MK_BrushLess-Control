#include "main.h"

volatile unsigned char Phase = 0,ShadowTCCR1A = 0;
volatile unsigned char CompFreigabeTimer = 100;
volatile unsigned char CompInterruptFreigabe = 0;


//############################################################################
//
SIGNAL(SIG_OVERFLOW2)
//############################################################################
{
}

//############################################################################
// + Interruptroutine
// + Wird durch den Analogkomperator ausgel?st
// + Dadurch wird das Kommutieren erzeugt
SIGNAL(SIG_COMPARATOR)
//############################################################################
{
unsigned char sense = 0;
do
{
 if(SENSE_H) sense = 1; else sense = 0;
 switch(Phase)
  {
   case 0:  
           STEUER_A_H;
           if(sense) 
            {
                      STEUER_C_L;
              TCNT2 = 1;
              if(ZeitZumAdWandeln) AdConvert();
              SENSE_FALLING_INT;
              SENSE_B; 
              Phase++; 
              CntKommutierungen++;
             }
                         else 
             {
              STEUER_B_L;
             } 
          break;
   case 1:  
               STEUER_C_L;
           if(!sense) 
            {
              STEUER_B_H;
              TCNT2 = 1;
              if(ZeitZumAdWandeln) AdConvert();
              SENSE_A; 
              SENSE_RISING_INT;
              Phase++; 
              CntKommutierungen++;
             }
             else 
             {
               STEUER_A_H;
             } 

          break;
   case 2:  
           STEUER_B_H;
           if(sense) 
            {
                      STEUER_A_L;
              TCNT2 = 1;
              if(ZeitZumAdWandeln) AdConvert();
              SENSE_C; 
              SENSE_FALLING_INT;
              Phase++; 
              CntKommutierungen++;
             }
             else 
             {
              STEUER_C_L;
             } 
              
          break;
   case 3:  
           STEUER_A_L;
           if(!sense) 
            {
                      STEUER_C_H;
              TCNT2 = 1;
              if(ZeitZumAdWandeln) AdConvert();
              SENSE_B; 
              SENSE_RISING_INT;
              Phase++; 
              CntKommutierungen++;
             }
             else 
             {
              STEUER_B_H;
             } 
              

          break;
   case 4:  
           STEUER_C_H;
           if(sense) 
            {
                      STEUER_B_L;
              TCNT2 = 1;
              if(ZeitZumAdWandeln) AdConvert();
              SENSE_A;                     
              SENSE_FALLING_INT;
              Phase++; 
              CntKommutierungen++;
             }
                     else 
             {
              STEUER_A_L;
             } 
              
          break;
   case 5:  
              STEUER_B_L;
           if(!sense) 
            {
                      STEUER_A_H;
              TCNT2 = 1;
              if(ZeitZumAdWandeln) AdConvert();
              SENSE_C; 
              SENSE_RISING_INT;
              Phase = 0; 
              CntKommutierungen++;
             }
             else 
             {
              STEUER_C_H;
             } 
          break;
  }
}
 while((SENSE_L && sense) || (SENSE_H && !sense));
 ZeitZumAdWandeln = 0;
}

//############################################################################
//
void Manuell(void)
//############################################################################
{
 switch(Phase)
  {
   case 0:  
           STEUER_A_H;
                   STEUER_B_L;
           SENSE_C; 
           SENSE_RISING_INT;
          break;
   case 1:  
           STEUER_A_H;
                   STEUER_C_L;
           SENSE_B; 
           SENSE_FALLING_INT;
          break;
   case 2:  
           STEUER_B_H;
                   STEUER_C_L;
           SENSE_A; 
           SENSE_RISING_INT;
          break;
   case 3:  
           STEUER_B_H;
                   STEUER_A_L;
           SENSE_C; 
           SENSE_FALLING_INT;
          break;
   case 4:  
           STEUER_C_H;
                   STEUER_A_L;
           SENSE_B; 
           SENSE_RISING_INT;
          break;
   case 5:  
           STEUER_C_H;
                   STEUER_B_L;
           SENSE_A; 
           SENSE_FALLING_INT;
          break;
  }
}
