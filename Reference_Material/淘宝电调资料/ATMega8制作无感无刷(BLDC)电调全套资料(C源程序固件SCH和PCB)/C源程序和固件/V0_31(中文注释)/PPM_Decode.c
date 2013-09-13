/*############################################################################
 + 无刷电机控制器
 + ATMEGA8 with 8MHz
 + (c) 01.2007 霍尔格巴斯
 + 只作私人用途 / 非商业用途使用
 + 不承诺准确性的保证
 + 商业用途需要我的同意
 + 该代码使用BL_Ctrl 1.0版已经开发的硬件
 + www.mikrocontroller.com
############################################################################*/
#include "main.h"
#include "PPM_Decode.h"

volatile unsigned int  PPM_Signal = 0;
volatile unsigned char Timer1Overflow = 0;
volatile unsigned char PPM_Timeout = 0;   // 为0时无效

//############################################################################
//
void InitPPM(void)
//############################################################################
{
 TCCR1B |= (1<<ICES1)|(1<<ICNC1); //使能输入捕捉噪声抑制器,逻辑电平的上升沿触发输入捕捉
 ICP_POS_FLANKE;                  // 输入捕捉上升沿触发
 ICP_INT_ENABLE;                  // T/C1 输入捕捉中断使能
 TIMER1_INT_ENABLE;               // T/C1 溢出中断使能   
}

//############################################################################
//
SIGNAL(SIG_OVERFLOW1)  // T/C1(16位) 溢出中断
//############################################################################
{
 Timer1Overflow++;
}

//############################################################################
//
SIGNAL(SIG_INPUT_CAPTURE1) // T/C1 输入捕获中断 
//############################################################################
{
 static unsigned int tim_alt;
 static unsigned int ppm;
 if(TCCR1B & (1<<ICES1))   // 上升沿触发输入捕捉
  {
   Timer1Overflow = 0;
   tim_alt = ICR1; 
   ICP_NEG_FLANKE;
   PPM_Timeout = 100;
  }
 else                      // 下降沿触发输入捕捉
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
    if(FILTER_PPM)      ppm = (PPM_Signal * FILTER_PPM + ppm) / (FILTER_PPM + 1);  // PPM信号过滤
    PPM_Signal = ppm;

    ZeitZumAdWandeln = 1;
  } 
}

//############################################################################
//
SIGNAL(SIG_INTERRUPT0)  //外部中断 INT0 
//############################################################################
{
 CLR_INT0_FLAG;      // 清楚中断标志 
}
