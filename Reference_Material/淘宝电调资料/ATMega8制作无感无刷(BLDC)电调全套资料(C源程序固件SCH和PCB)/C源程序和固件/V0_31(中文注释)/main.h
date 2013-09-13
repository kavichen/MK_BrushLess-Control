#ifndef _MAIN_H
 #define _MAIN_H

#define MOTORADRESSE      4   // BLCD的地址 (1-4)
#define MANUELL_PWM       30  // PWM的值: 0-255
#define TEST_MANUELL      30  // 测试硬件  值: 0-255 (PWM)
#define TEST_SCHUB        0	  // 测试或正常模式       值: 0 = 正常 1 = 测试
#define POLANZAHL        12   // 磁铁级数,仅用在输出速度的调试上
#define MAX_PWM         255
#define MIN_PWM           3
#define MIN_PPM          10   // PPM-Signal开关的约束
#define FILTER_PPM        7   // 多少个PPM信号进行过滤(值0~30)

#define SIO_DEBUG         0   // 测试值在串行接口输出
#define X3D_SIO           1	  // serielles Protokoll des X3D (38400Bd) Achtung: dann muss SIO_DEBUG = 0 sein
//在X3D的串行协议（38400Bd）注：必须SIO_DEBUG = 0

#define _16KHZ    // 开关频率为16KHz
//#define _32KHZ  

#ifdef _16KHZ
#define MAX_STROM        130  // PWM最大值,大概为13A
#define LIMIT_STROM      65  // PWM限制值,大概为6.5A时的PWM值
#endif

#ifdef _32KHZ
#define MAX_STROM        130  // PWM最大值,大概为13A
#define LIMIT_STROM       50  // PWM限制值,大概为5A时的PWM值
#endif

#define SYSCLK	8000000L	//时钟频率,单位Hz

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/boot.h>
#include <avr/wdt.h>

#include "twislave.h"
#include "old_macros.h"
#include "analog.h"
//#include "uart.h"
#include "timer0.h"
#include "BLMC.h"

//#include "PPM_Decode.h"

extern unsigned int  I2C_Timeout;
extern unsigned char SIO_Timeout;
extern unsigned int  PWM;
extern unsigned int  Strom;     //ca. in 0,1A
extern unsigned char Strom_max;
extern unsigned char Mittelstrom;         //平均电流
extern unsigned int  CntKommutierungen;   //换向计数
extern unsigned char MotorAnwerfen;       //马达启动
extern unsigned char MotorGestoppt;       //马达停止
extern unsigned char ZeitZumAdWandeln;    //Ad的转换时间
extern unsigned char MaxPWM;

#define ROT    0x08
#define GRUEN  0x80	

#if defined(__AVR_ATmega8__)
#  define OC1   PB1
#  define DDROC DDRB
#  define OCR   OCR1A
#  define PWM10 WGM10
#  define PWM11 WGM11
#endif 


#endif //_MAIN_H





