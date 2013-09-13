/*############################################################################
 + 无刷电机控制器
 + ATMEGA8 At 8MHz
 + (c) 01.2007 Holger Buss
 + 只作私人用途
 + 准确性没有保证
 + 未经我同意不得商业使用
 + 该代码使用BL_Ctrl 1.0版已经开发的硬件
 + www.mikrocontroller.com
############################################################################*/
#include "main.h"
#include <avr/signal.h> 

volatile unsigned char Phase = 0,ShadowTCCR1A = 0;
volatile unsigned char CompFreigabeTimer = 100;
volatile unsigned char CompInterruptFreigabe = 0;


//############################################################################
//
SIGNAL(SIG_OVERFLOW2) // T/C2 溢出中断
//############################################################################
{
}

//############################################################################
// + Interruptroutine
// + Wird durch den Analogkomperator ausgel鰏t
// + Dadurch wird das Kommutieren erzeugt
SIGNAL(SIG_COMPARATOR) //模拟比较器中断 
//############################################################################
/*补充注释：
比较器中断，实现闭环中断过零检测并换向
注意其中并不包含电角度延迟，即：换向提前角是30度
*/
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
/*补充注释：
开环顺序换向算法，注意换向时必须同步修改比较器端口及触发沿
以便在反相感生电动势到达切换条件时，自动切换到闭环运转状态
*/
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

