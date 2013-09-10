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
// +   * If sources or documentations are redistributet on other webpages, out webpage (http://www.MikroKopter.de) must be
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
// + Wird durch den Analogkomperator ausgelöst
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
