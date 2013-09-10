#ifndef _MAIN_H
#define _MAIN_H

//#define MOTORADRESSE      0  // Adresse (1-4)     0 = Motoradresse über Lötjumper auf BL-Ctrl V1.1

#define MANUELL_PWM       0   // zur manuellen Vorgabe der PWM  Werte: 0-255
#define TEST_MANUELL      0   // zum Testen der Hardware ohne Kommutierungsdetektion  Werte: 0-255 (PWM)
#define TEST_SCHUB        0       // Erzeugt ein Schubmuster        Werte: 0 = normal 1 = Test
#define POLANZAHL        12   // Anzahl der Pole (Magnete) Wird nur zur Drehzahlausgabe über Debug gebraucht
#define MAX_PWM         255
#define MIN_PWM           3
#define MIN_PPM          10   // ab hier (PPM-Signal) schaltet der Regler erst ein
#define FILTER_PPM        7   // wie stark soll das PPM-Signal gefiltert werden (Werte: 0-30)?

#define SIO_DEBUG         0   // Testwertausgaben auf der seriellen Schnittstelle
#define X3D_SIO           0       // serielles Protokoll des X3D (38400Bd) Achtung: dann muss SIO_DEBUG = 0 sein

#define _16KHZ    // Schaltfrequenz  -- die gewünschte einkommentieren
//#define _32KHZ    // Schaltfrequenz  -- die gewünschte einkommentieren

//#define FDD6637_IRLR7843  1  // bessere MosFet bestückt? bewirkt höhere Stromgrenzen  


#ifdef _16KHZ
#if FDD_IRLR == 1                  // bessere Fets = mehr Strom zulassen
#define MAX_STROM        200  // ab ca. 20A PWM ausschalten
#define LIMIT_STROM      120  // ab ca. 12A PWM begrenzen
#else
#define MAX_STROM        130  // ab ca. 13A PWM ausschalten
#define LIMIT_STROM       65  // ab ca. 6,5A PWM begrenzen
#endif
#endif

#ifdef _32KHZ
#define MAX_STROM        130  // ab ca. 13A PWM ausschalten
#define LIMIT_STROM       50  // ab ca. 5,0A PWM begrenzen
#endif

#define SYSCLK  8000000L        //Quarz Frequenz in Hz

#define GRN_ON  {if(HwVersion < 12) PORTD |= GRUEN; else PORTD &= ~GRUEN;}
#define GRN_OFF {if(HwVersion > 11) PORTD |= GRUEN; else PORTD &= ~GRUEN;}


//Robbe Roxxy 2824-34 mit Todd 10x4,5 bei 16kHz
//PWM   Strom   Schub
//10   0,18A    21g
//20   0,30A    38g
//30   0,40A    49g
//40   0,54A    60g
//50   0,70A    83g
//60   0,95A   102g
//70   1,25A   128g
//80   1,50A   151g
//90   1,83A   175g
//100  2,25A   206g
//110  2,66A   228g
//120  3,10A   257g
//130  3,55A   278g
//140  4,00A   305g
//150  4,50A   330g
//160  5,30A   355g
//170  5,85A   387g
//180  6,40A   400g
//190  7,10A   425g
//200  7,60A   460g

//0,5A  63g 14%
//1,0A 110g 24%
//1,5A 170g 32%
//2,0A 195g 37%
//2,5A 230g 41%
//3,0A 260g 46%
//3,5A 290g 50%
//4,0A 310g 53%
//4,5A 340g 58%
//5,0A 360g 63%
//5,5A 380g 67%
//6,0A 400g 68%
//6,5A 420g 70%
//7,0A 450g 75%
//7,5A 460g 78%
//8,0A 475g 82%
//9,0A 550g 86%
//11 A 600g 100%


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
#include "uart.h"
#include "timer0.h"
#include "BLMC.h"

#include "PPM_Decode.h"

extern unsigned int  I2C_Timeout;
extern unsigned int SIO_Timeout;
extern unsigned int  PWM;
extern unsigned int  Strom;     //ca. in 0,1A
extern unsigned char Strom_max;
extern unsigned char Mittelstrom;
extern unsigned int  CntKommutierungen;
extern unsigned char MotorAnwerfen;
extern unsigned char MotorGestoppt;
extern unsigned char ZeitZumAdWandeln;
extern unsigned char MaxPWM;
extern unsigned char MotorAdresse;
extern unsigned char PPM_Betrieb;
extern unsigned char HwVersion;
extern unsigned char IntRef;
#define ROT    0x08
#define GRUEN  0x80    

#define ADR1   0x40    // für Motoradresswahl
#define ADR2   0x80    //

#if defined(__AVR_ATmega8__)
#  define OC1   PB1
#  define DDROC DDRB
#  define OCR   OCR1A
#  define PWM10 WGM10
#  define PWM11 WGM11
#endif


#endif //_MAIN_H
