/*#######################################################################################
Flight Control
#######################################################################################*/
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Regler für Brushless-Motoren
// + ATMEGA8 mit 8MHz
// + Nur für den privaten Gebrauch / NON-COMMERCIAL USE ONLY
// + Copyright (c) 12.2007 Holger Buss
// + www.MikroKopter.com
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Es gilt für das gesamte Projekt (Hardware, Software, Binärfiles, Sourcecode und Dokumentation), 
// + dass eine Nutzung (auch auszugsweise) nur für den privaten (nicht-kommerziellen) Gebrauch zulässig ist. 
// + Sollten direkte oder indirekte kommerzielle Absichten verfolgt werden, ist mit uns (info@mikrokopter.de) Kontakt 
// + bzgl. der Nutzungsbedingungen aufzunehmen. 
// + Eine kommerzielle Nutzung ist z.B.Verkauf von MikroKoptern, Bestückung und Verkauf von Platinen oder Bausätzen,
// + Verkauf von Luftbildaufnahmen, usw.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Werden Teile des Quellcodes (mit oder ohne Modifikation) weiterverwendet oder veröffentlicht, 
// + unterliegen sie auch diesen Nutzungsbedingungen und diese Nutzungsbedingungen incl. Copyright müssen dann beiliegen
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Sollte die Software (auch auszugesweise) oder sonstige Informationen des MikroKopter-Projekts
// + auf anderen Webseiten oder sonstigen Medien veröffentlicht werden, muss unsere Webseite "http://www.mikrokopter.de"
// + eindeutig als Ursprung verlinkt werden
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Keine Gewähr auf Fehlerfreiheit, Vollständigkeit oder Funktion
// + Benutzung auf eigene Gefahr
// + Wir übernehmen keinerlei Haftung für direkte oder indirekte Personen- oder Sachschäden
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Die Portierung oder Nutzung der Software (oder Teile davon) auf andere Systeme (ausser der Hardware von www.mikrokopter.de) ist nur 
// + mit unserer Zustimmung zulässig
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Die Funktion printf_P() unterliegt ihrer eigenen Lizenz und ist hiervon nicht betroffen
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Redistributions of source code (with or without modifications) must retain the above copyright notice, 
// + this list of conditions and the following disclaimer.
// +   * Neither the name of the copyright holders nor the names of contributors may be used to endorse or promote products derived
// +     from this software without specific prior written permission.
// +   * The use of this project (hardware, software, binary files, sources and documentation) is only permittet 
// +     for non-commercial use (directly or indirectly)
// +     Commercial use (for excample: selling of MikroKopters, selling of PCBs, assembly, ...) is only permitted 
// +     with our written permission
// +   * If sources or documentations are redistributet on other webpages, our webpage (http://www.MikroKopter.de) must be 
// +     clearly linked as origin 
// +   * porting the sources to other systems or using the software on other systems (except hardware from www.mikrokopter.de) is not allowed
// +  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// +  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// +  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// +  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// +  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// +  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// +  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// +  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN// +  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// +  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// +  POSSIBILITY OF SUCH DAMAGE. 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "main.h"

unsigned int  PWM = 0; 
unsigned int  Strom = 0,RuheStrom; //ca. in 0,1A
unsigned char Strom_max = 0;
unsigned char Mittelstrom = 0; 
unsigned int  Drehzahl = 0;  // in 100UPM  60 = 6000
unsigned int  KommutierDelay = 10;
unsigned int  I2C_Timeout = 0;
unsigned int SIO_Timeout = 0;
unsigned int  SollDrehzahl = 0;
unsigned int  IstDrehzahl = 0;
unsigned int  DrehZahlTabelle[256];//vorberechnete Werte zur Drehzahlerfassung
unsigned char ZeitFuerBerechnungen = 1;
unsigned char MotorAnwerfen = 0;
unsigned char MotorGestoppt = 1;
unsigned char MaxPWM = MAX_PWM;
unsigned int  CntKommutierungen = 0;
unsigned int  SIO_Drehzahl = 0;
unsigned char ZeitZumAdWandeln = 1;
unsigned char MotorAdresse = 1;
unsigned char PPM_Betrieb = 1;
unsigned char HwVersion;
unsigned char IntRef = 0;
unsigned int MinUpmPulse;
//############################################################################
//
void SetPWM(void)
//############################################################################
{
    unsigned char tmp_pwm; 
    tmp_pwm = PWM;
    if(tmp_pwm > MaxPWM)    // Strombegrenzung
        {
        tmp_pwm = MaxPWM;
        PORTC |= ROT; 
        } 
    if(Strom > MAX_STROM)   // Strombegrenzung
        {
        OCR1A = 0; OCR1B = 0; OCR2 = 0;
        PORTD &= ~0x38;
        PORTC |= ROT; 
        DebugOut.Analog[6]++;
        Strom--;
        }
    else 
        {
        #ifdef  _32KHZ 
        OCR1A =  tmp_pwm; OCR1B =  tmp_pwm; OCR2  = tmp_pwm;
        #endif 

        #ifdef  _16KHZ 
        //OCR1A = 2 * (int)tmp_pwm; OCR1B = 2 * (int)tmp_pwm; OCR2  = tmp_pwm;
        OCR1A =  tmp_pwm; OCR1B =  tmp_pwm; OCR2  = tmp_pwm;
        #endif 
        }
}

void DebugAusgaben(void)
{
    DebugOut.Analog[0] = Strom;
    DebugOut.Analog[1] = Mittelstrom;
    DebugOut.Analog[2] = SIO_Drehzahl;
    DebugOut.Analog[3] = PPM_Signal;
    DebugOut.Analog[4] = OCR2;
//    DebugOut.Analog[5] = PWM;
}

//############################################################################
//
void PWM_Init(void)
//############################################################################
{
    PWM_OFF;
    TCCR1B = (1 << CS10) | (0 << CS11) | (0 << CS12) | (0 << WGM12) | 
             (0 << WGM13) | (0<< ICES1) | (0 << ICNC1);
/*    TCCR1B = (1 << CS10) | (0 << CS11) | (0 << CS12) | (1 << WGM12) | 
             (0 << WGM13) | (0<< ICES1) | (0 << ICNC1);
*/
}

//############################################################################
//
void Wait(unsigned char dauer)
//############################################################################
{
    dauer = (unsigned char)TCNT0 + dauer;
    while((TCNT0 - dauer) & 0x80);
}

void RotBlink(unsigned char anz)
{
sei(); // Interrupts ein
 while(anz--)
  {
   PORTC |= ROT;
   Delay_ms(300);    
   PORTC &= ~ROT;
   Delay_ms(300);    
  }
   Delay_ms(1000);    
}

//############################################################################
//
char Anwerfen(unsigned char pwm)
//############################################################################
{
    unsigned long timer = 300,i;
    DISABLE_SENSE_INT;
    PWM = 5;
    SetPWM();
    Manuell();
//    Delay_ms(200);
                    MinUpmPulse = SetDelay(300);
                    while(!CheckDelay(MinUpmPulse)) 
                    {
                     FastADConvert();
                      if(Strom > 120) 
                      {
                        STEUER_OFF; // Abschalten wegen Kurzschluss
                        RotBlink(10);
                        return(0);
                      }  
                    }
    PWM = pwm;
    while(1)
        {
        for(i=0;i<timer; i++) 
            {
            if(!UebertragungAbgeschlossen)  SendUart();
            else DatenUebertragung();
            Wait(100);  // warten
            } 
        DebugAusgaben();
        FastADConvert();
        if(Strom > 60) 
          {
            STEUER_OFF; // Abschalten wegen Kurzschluss
            RotBlink(10);
            return(0);
          }  
         
        timer-= timer/15+1;
        if(timer < 25) { if(TEST_MANUELL) timer = 25; else return(1); }
        Manuell();
        Phase++;
        Phase %= 6;
        AdConvert();
        PWM = pwm;
        SetPWM();
        if(SENSE) 
            {
            PORTD ^= GRUEN;
            } 
        }
}

/*
#define SENSE_A ADMUX = 0;
#define SENSE_B ADMUX = 1;
#define SENSE_C ADMUX = 2;

#define ClrSENSE            ACSR |= 0x10
#define SENSE               ((ACSR & 0x10))
#define SENSE_L             (!(ACSR & 0x20))
#define SENSE_H             ((ACSR & 0x20))
*/


#define TEST_STROMGRENZE 120
unsigned char DelayM(unsigned int timer)
{
 while(timer--)
  {
   FastADConvert();
   if(Strom > (TEST_STROMGRENZE + RuheStrom))
       {
        FETS_OFF;
        return(1);
       } 
  }
 return(0);  
}

unsigned char Delay(unsigned int timer)
{
 while(timer--)
  {
//   if(SENSE_H) { PORTC |= ROT; } else { PORTC &= ~ROT;}
  }
 return(0);  
}

/*
void ShowSense(void)
{
 if(SENSE_H) { PORTC |= ROT; } else { PORTC &= ~ROT;}

}
*/
#define HIGH_A_EIN PORTB |= 0x08
#define HIGH_B_EIN PORTB |= 0x04
#define HIGH_C_EIN PORTB |= 0x02
#define LOW_A_EIN  PORTD |= 0x08
#define LOW_B_EIN  PORTD |= 0x10
#define LOW_C_EIN  PORTD |= 0x20

void MotorTon(void)
//############################################################################
{
    unsigned char ADR_TAB[5] = {0,0,2,1,3};
    unsigned int timer = 300,i;
    unsigned int t = 0;
    unsigned char anz = 0,MosfetOkay = 0, grenze = 50;

    PORTC &= ~ROT;
    Delay_ms(300 * ADR_TAB[MotorAdresse]);    
    DISABLE_SENSE_INT;
    cli();//Globale Interrupts Ausschalten
    uart_putchar('\n');
    STEUER_OFF;
    Strom_max = 0;
    DelayM(50);
    RuheStrom = Strom_max;
//    uart_putchar(RuheStrom + 'A');
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ High-Mosfets auf Kurzschluss testen
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Strom = 0;
/*
    LOW_B_EIN; 
    HIGH_A_EIN; 
    if(DelayM(3))
       {
        anz = 1;
        uart_putchar('1');
       }
    FETS_OFF;
    Delay(1000);
    Strom = 0;
    LOW_A_EIN;
    HIGH_B_EIN; 
    if(DelayM(3))
       {
        anz = 2;
        uart_putchar('2');
       }
    FETS_OFF;
    Delay(1000);
    Strom = 0;
    LOW_B_EIN; // Low C ein
    HIGH_C_EIN; // High B ein
    if(DelayM(3))
       {
        anz = 3;
        uart_putchar('3');
       }
    FETS_OFF;
    Delay(1000);
    LOW_A_EIN; // Low  A ein; und A gegen C
    HIGH_C_EIN; // High C ein
    if(DelayM(3))
       {
        anz = 3;
        uart_putchar('7');
       }
    FETS_OFF;
    DelayM(10000);

if(anz) while(1) RotBlink(anz);  // bei Kurzschluss nicht starten
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ LOW-Mosfets auf Schalten und Kurzschluss testen
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 if(UDR == ' ') {t = 65535; grenze = 40; uart_putchar('_');} else t = 1000; // Ausführlicher Test
 Strom = 0;
 for(i=0;i<t;i++)
 {
  LOW_A_EIN; 
  DelayM(1);
  FETS_OFF;
  Delay(5);
  HIGH_A_EIN;
  DelayM(1);
  FETS_OFF;
  if(Strom > grenze + RuheStrom) {anz = 4; uart_putchar('4'); FETS_OFF; break;}
  Delay(5);
 }
 Delay(10000);

 Strom = 0;
 for(i=0;i<t;i++)
 {
  LOW_B_EIN; 
  DelayM(1);
  FETS_OFF;
  Delay(5);
  HIGH_B_EIN;
  DelayM(1);
  FETS_OFF;
  if(Strom > grenze + RuheStrom) {anz = 5; uart_putchar('5'); FETS_OFF;break;}
  Delay(5);
 } 

 Strom = 0;
 Delay(10000);

 for(i=0;i<t;i++)
 {
  LOW_C_EIN; 
  DelayM(1);
  FETS_OFF;
  Delay(5);
  HIGH_C_EIN;
  DelayM(1);
  FETS_OFF;
  if(Strom > grenze + RuheStrom) {anz = 6; uart_putchar('6'); FETS_OFF; break;}
  Delay(5);
 } 

 if(anz) while(1) RotBlink(anz);  // bei Kurzschluss nicht starten

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ High-Mosfets auf Schalten testen
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    SENSE_A;
    FETS_OFF;
    LOW_B_EIN; // Low B ein
    LOW_C_EIN; // Low C ein
    Strom = 0;
#define TONDAUER  40000    
#define SOUND_E 1  // 1
#define SOUND1_A 300
#define SOUND2_A 330
#define SOUND3_A 360

    for(i=0; i< (TONDAUER / SOUND2_A) ; i++)
     {
      HIGH_A_EIN; // Test A
      Delay(SOUND_E);
      if(MessAD(0) > 50) { MosfetOkay |= 0x01; } else { MosfetOkay &= ~0x01;};
      PORTB = 0;
      Delay(SOUND1_A);
     }
    FETS_OFF;

    LOW_A_EIN; // Low A ein
    LOW_C_EIN; // Low C ein
    for(i=0; i<(TONDAUER / SOUND1_A); i++)
     {
      HIGH_B_EIN; // Test B
      Delay(SOUND_E);
      if(MessAD(1) > 50) { MosfetOkay |= 0x02; } else { MosfetOkay &= ~0x02;};
      PORTB = 0;
      Delay(SOUND1_A);
     }

    FETS_OFF;
    LOW_A_EIN; // Low A ein
    LOW_B_EIN; // Low B ein
    for(i=0; i<(TONDAUER / SOUND3_A); i++)
     {
      HIGH_C_EIN; // Test C
      Delay(SOUND_E);
      if(MessAD(2) > 50) { MosfetOkay |= 0x04; } else { MosfetOkay &= ~0x04;};
      PORTB = 0;
      Delay(SOUND2_A);
     }
    FETS_OFF;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ Low-Mosfets auf Schalten testen
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    SENSE_B;
    LOW_A_EIN; // Low A ein
    for(i=0; i< (TONDAUER / SOUND2_A) ; i++)
     {
      HIGH_B_EIN; // Test B
      Delay(SOUND_E);
      if(MessAD(0) > 128) { MosfetOkay &= ~0x08;} else { MosfetOkay |= 0x08;};
      PORTB = 0;
      Delay(SOUND2_A);
     }

//++++++++++++++++++++++++++++++++++++
    LOW_C_EIN; // Low C ein
    for(i=0; i<(TONDAUER / SOUND1_A); i++)
     {
      HIGH_B_EIN; // Test B
      Delay(SOUND_E);
      if(MessAD(2) > 128) { MosfetOkay &= ~0x20;} else { MosfetOkay |= 0x20;};
      PORTB = 0;
      Delay(SOUND3_A);
     }
    FETS_OFF;
//++++++++++++++++++++++++++++++++++++
    FETS_OFF;
    LOW_B_EIN; // Low B ein
    for(i=0; i<(TONDAUER / SOUND3_A); i++)
     {
      HIGH_C_EIN; // Test C
      Delay(SOUND_E);
      if(MessAD(1) > 128) { MosfetOkay &= ~0x10;} else { MosfetOkay |= 0x10;};
      PORTB = 0;
      Delay(SOUND3_A);
     }
    FETS_OFF;
//++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    sei();//Globale Interrupts Einschalten
//    Delay_ms(250 * MotorAdresse);    
/*
    LOW_A_EIN; // Low B ein
#define SOUND8_A 650
    for(i=0; i<(TONDAUER / SOUND8_A); i++)
     {
      HIGH_B_EIN; // Test B
      Delay(SOUND_E);
      PORTB = 0;
      Delay(SOUND8_A);
     }
*/
 Delay_ms(300 * (4-ADR_TAB[MotorAdresse]));    
 if(!(MosfetOkay & 0x01))  { anz = 1; UDR='A'; } else
 if(!(MosfetOkay & 0x02))  { anz = 2; UDR='B'; } else
 if(!(MosfetOkay & 0x04))  { anz = 3; UDR='C'; } else
 if(!(MosfetOkay & 0x08))  { anz = 4; UDR='a'; } else
 if(!(MosfetOkay & 0x10))  { anz = 5; UDR='b'; } else
 if(!(MosfetOkay & 0x20))  { anz = 6; UDR='c'; }  

// if(anz) Delay_ms(1000); 
 if(anz) while(1) RotBlink(anz);  // bei Kurzschluss nicht starten
 RotBlink(anz);
 uart_putchar('.');
}

//############################################################################
//
unsigned char SollwertErmittlung(void)
//############################################################################
{
    static unsigned int sollwert = 0;
    unsigned int ppm;
    if(!I2C_Timeout)   // bei Erreichen von 0 ist der Wert ungültig
        {
        if(SIO_Timeout)  // es gibt gültige SIO-Daten
            {
             sollwert =  (MAX_PWM * (unsigned int) SIO_Sollwert) / 200;  // skalieren auf 0-200 = 0-255
             PPM_Betrieb = 0;
             ICP_INT_DISABLE;
             PORTC &= ~ROT;
            }
        else
            if(anz_ppm_werte > 20)  // es gibt gültige PPM-Daten
                {
                PPM_Betrieb = 1;
                ppm = PPM_Signal;
                if(ppm > 300) ppm =   0;  // ungültiges Signal
                if(ppm > 200) ppm = 200;
                if(ppm <= MIN_PPM) sollwert = 0;
                else 
                    {
                    sollwert = (int) MIN_PWM + ((MAX_PWM - MIN_PWM) * (ppm - MIN_PPM)) / (190 - MIN_PPM);
                    }
                PORTC &= ~ROT;
                }
            else   // Kein gültiger Sollwert
                {
                 if(!TEST_SCHUB) { if(sollwert) sollwert--; }   
                 PORTC |= ROT; 
                }
        }
    else // I2C-Daten sind gültig
        {
        sollwert = I2C_RXBuffer; 
        PPM_Betrieb = 0;
        PORTC &= ~ROT;
        ICP_INT_DISABLE;
        }
    if(sollwert > MAX_PWM) sollwert = MAX_PWM;
    return(sollwert); 
}



//############################################################################
//Hauptprogramm
int main (void)
//############################################################################
{
    char altPhase = 0;
    int test = 0;
    unsigned int Blink,TestschubTimer;
    unsigned int Blink2,MittelstromTimer,DrehzahlMessTimer,MotorGestopptTimer;

    DDRC  = 0x08;
    PORTC = 0x08;
    DDRD  = 0x3A;
    PORTD = 0x00;
    DDRB  = 0x0E;
    PORTB = 0x31;
	
#if (MOTORADRESSE == 0)
    PORTB |= (ADR1 + ADR2);   // Pullups für Adresswahl
    for(test=0;test<500;test++);
    if(PINB & ADR1)
	 {
	   if (PINB & ADR2) MotorAdresse = 1;
	    else MotorAdresse = 2;
	 }
	 else
	 {
	   if (PINB & ADR2) MotorAdresse = 3;
	    else MotorAdresse = 4;
	 }
    HwVersion = 11;
#else
    MotorAdresse  = MOTORADRESSE;
    HwVersion = 10;
#endif
    if(PIND & 0x80) {HwVersion = 12; IntRef = 0xc0;}    
    DDRD  = 0xBA;
    UART_Init();
    Timer0_Init();
    sei();//Globale Interrupts Einschalten
    
    // Am Blinken erkennt man die richtige Motoradresse
/*
    for(test=0;test<5;test++)
        {
        if(test == MotorAdresse) PORTD |= GRUEN;
        Delay_ms(150);
        PORTD &= ~GRUEN;
        Delay_ms(250);
        }	

    Delay_ms(500);
*/   
   // UART_Init();  // war doppelt
    PWM_Init();	

    InitIC2_Slave(0x50);			    
    InitPPM();

    Blink             = SetDelay(101);    
    Blink2            = SetDelay(102);
    MinUpmPulse       = SetDelay(103);
    MittelstromTimer  = SetDelay(254);
    DrehzahlMessTimer = SetDelay(1005);
    TestschubTimer    = SetDelay(1006);
    while(!CheckDelay(MinUpmPulse))
    {
     if(SollwertErmittlung()) break;
    }

    GRN_ON;
    PWM = 0;

    SetPWM();

    SFIOR = 0x08;  // Analog Comperator ein
    ADMUX = 1; 

    MinUpmPulse = SetDelay(10);
    DebugOut.Analog[1] = 1;
    PPM_Signal = 0;

    if(!SollwertErmittlung()) MotorTon();
//MotorTon();    
    PORTB = 0x31; // Pullups wieder einschalten

    // zum Test der Hardware; Motor dreht mit konstanter Drehzahl ohne Regelung
    if(TEST_MANUELL)    Anwerfen(TEST_MANUELL);  // kommt von dort nicht wieder

    while (1)
        {
//ShowSense();

        if(!TEST_SCHUB)   PWM = SollwertErmittlung();
        //I2C_TXBuffer = PWM; // Antwort über I2C-Bus
        if(MANUELL_PWM)   PWM = MANUELL_PWM;

        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(Phase != altPhase)   // es gab eine Kommutierung im Interrupt
            {
            MotorGestoppt = 0;
            ZeitFuerBerechnungen = 0;    // direkt nach einer Kommutierung ist Zeit 
            MinUpmPulse = SetDelay(250);  // Timeout, falls ein Motor stehen bleibt
            altPhase = Phase;
            }
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!PWM)    // Sollwert == 0
            {
            MotorAnwerfen = 0;      // kein Startversuch
            ZeitFuerBerechnungen = 0;
            // nach 1,5 Sekunden den Motor als gestoppt betrachten 
            if(CheckDelay(MotorGestopptTimer)) 
                {
                DISABLE_SENSE_INT;
                MotorGestoppt = 1;  
                STEUER_OFF;
                } 
            }
        else 
            {
            if(MotorGestoppt) MotorAnwerfen = 1;	// Startversuch
            MotorGestopptTimer = SetDelay(1500);
            }

        if(MotorGestoppt && !TEST_SCHUB) PWM = 0;
        SetPWM();
        // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!ZeitFuerBerechnungen++)
            {
            if(MotorGestoppt) 
             {
              GRN_ON; 
              FastADConvert();
             }
            if(SIO_DEBUG)
                {
                DebugAusgaben();  // welche Werte sollen angezeigt werden?
                if(!UebertragungAbgeschlossen)  SendUart(); 
                else DatenUebertragung();
                }
            // Berechnen des Mittleren Stroms zur (langsamen) Strombegrenzung
            if(CheckDelay(MittelstromTimer))   
                {
                MittelstromTimer = SetDelay(50); // alle 50ms
                if(Mittelstrom <  Strom) Mittelstrom++;// Mittelwert des Stroms bilden
                else if(Mittelstrom >  Strom) Mittelstrom--;
                if(Strom > MAX_STROM) MaxPWM -= MaxPWM / 32;               
                if((Mittelstrom > LIMIT_STROM))// Strom am Limit?
                    {
                    if(MaxPWM) MaxPWM--;// dann die Maximale PWM herunterfahren
                    PORTC |= ROT; 
                    }
                else 
                    {
                    if(MaxPWM < MAX_PWM) MaxPWM++;
                    }
                }

            if(CheckDelay(DrehzahlMessTimer))   // Ist-Drehzahl bestimmen
                {
                DrehzahlMessTimer = SetDelay(10);
                SIO_Drehzahl = CntKommutierungen;//(6 * CntKommutierungen) / (POLANZAHL / 2);
                CntKommutierungen = 0;
               // if(PPM_Timeout == 0) // keine PPM-Signale
                ZeitZumAdWandeln = 1;
                }

#if TEST_SCHUB == 1
           {
            if(CheckDelay(TestschubTimer))  
                {
                TestschubTimer = SetDelay(1500);
                    switch(test) 
                        {
                        case 0: PWM = 50; test++; break;
                        case 1: PWM = 130; test++; break;
                        case 2: PWM = 60;  test++; break;
                        case 3: PWM = 140; test++; break;
                        case 4: PWM = 150; test = 0; break;
                        default: test = 0;
                        } 
                }
            }  
#endif
          // Motor Stehen geblieben
            if((CheckDelay(MinUpmPulse) && SIO_Drehzahl == 0) || MotorAnwerfen) 
                {
                MotorGestoppt = 1;    
                DISABLE_SENSE_INT;
                MinUpmPulse = SetDelay(100);         
                if(MotorAnwerfen)
                  {
                   PORTC &= ~ROT;
                   Strom_max = 0;
                   MotorAnwerfen = 0;
                   if(Anwerfen(10))
                   {  
                    GRN_ON;
                    MotorGestoppt = 0;    
                    Phase--;
                    PWM = 1;
                    SetPWM();
                    SENSE_TOGGLE_INT;
                    ENABLE_SENSE_INT;
                    MinUpmPulse = SetDelay(20);
                    while(!CheckDelay(MinUpmPulse)); // kurz Synchronisieren
                    PWM = 15;
                    SetPWM();
                    MinUpmPulse = SetDelay(300);
                    while(!CheckDelay(MinUpmPulse)) // kurz Durchstarten
                    {
                      if(Strom > LIMIT_STROM/2) 
                      {
                        STEUER_OFF; // Abschalten wegen Kurzschluss
                        RotBlink(10);
                        MotorAnwerfen = 1;
                      }  
                    }
				    // Drehzahlmessung wieder aufsetzen
                    DrehzahlMessTimer = SetDelay(50);
                    altPhase = 7;
                   }
                   else if(SollwertErmittlung()) MotorAnwerfen = 1;
                  }
                }
            } // ZeitFuerBerechnungen
        } // while(1) - Hauptschleife
}

