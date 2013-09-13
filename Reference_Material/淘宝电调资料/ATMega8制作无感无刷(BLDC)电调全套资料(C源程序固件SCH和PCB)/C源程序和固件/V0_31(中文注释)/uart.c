// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Regler für Brushless-Motoren
// + ATMEGA8 mit 8MHz
// + (c) 01.2007 Holger Buss
// + Nur für den privaten Gebrauch
// + Keine Garantie auf Fehlerfreiheit
// + Kommerzielle Nutzung nur mit meiner Zustimmung
// + Der Code ist für die Hardware BL_Ctrl V1.0 entwickelt worden
// + www.mikrocontroller.com
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "main.h"
//#include "uart.h"

#define MAX_SENDE_BUFF     100
#define MAX_EMPFANGS_BUFF  100

unsigned volatile char SIO_Sollwert = 0;
unsigned volatile char SioTmp = 0;
unsigned volatile char SendeBuffer[MAX_SENDE_BUFF];
unsigned volatile char RxdBuffer[MAX_EMPFANGS_BUFF];
unsigned volatile char NeuerDatensatzEmpfangen = 0;
unsigned volatile char UebertragungAbgeschlossen = 1;
unsigned char MeineSlaveAdresse;

struct str_DebugOut    DebugOut;


int Debug_Timer;


SIGNAL(INT_VEC_TX)
{
}

void SendUart(void)
{
 static unsigned int ptr = 0;
 unsigned char tmp_tx;
 if(!(UCSRA & 0x40)) return;
 if(!UebertragungAbgeschlossen)  
  {
   ptr++;                    // die [0] wurde schon gesendet
   tmp_tx = SendeBuffer[ptr];  
   if((tmp_tx == '\r') || (ptr == MAX_SENDE_BUFF))
    {
     ptr = 0;
     UebertragungAbgeschlossen = 1;
    }
   USR |= (1<TXC);
   UDR = tmp_tx; 
  } 
  else ptr = 0;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++ Empfangs-Part der Datenübertragung
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SIGNAL(INT_VEC_RX)
{
 static unsigned char serPacketCounter = 100;
 SioTmp = UDR;
#if  X3D_SIO == 1
	if(SioTmp == 0xF5)     // Startzeichen
	 {
  	  serPacketCounter = 0;
	 }
	else 
	 {
    	  if(++serPacketCounter == MOTORADRESSE) // (1-4)
    	  {
    		SIO_Sollwert = SioTmp;
            SIO_Timeout = 200; // werte für 200ms gültig
    	  }
    	  else
    	  { 
    	   if(serPacketCounter > 100) serPacketCounter = 100;
		  }
	 }
#endif
};


// --------------------------------------------------------------------------
void AddCRC(unsigned int wieviele)
{
 unsigned int tmpCRC = 0,i; 
 for(i = 0; i < wieviele;i++)
  {
   tmpCRC += SendeBuffer[i];
  }
   tmpCRC %= 4096;
   SendeBuffer[i++] = '=' + tmpCRC / 64;
   SendeBuffer[i++] = '=' + tmpCRC % 64;
   SendeBuffer[i++] = '\r';
  UebertragungAbgeschlossen = 0;
  UDR = SendeBuffer[0];
}


// --------------------------------------------------------------------------
void SendOutData(unsigned char cmd,unsigned char modul, unsigned char *snd, unsigned char len)
{
 unsigned int pt = 0;
 unsigned char a,b,c;
 unsigned char ptr = 0;
 

 SendeBuffer[pt++] = '#';               // Startzeichen
 SendeBuffer[pt++] = modul;             // Adresse (a=0; b=1,...)
 SendeBuffer[pt++] = cmd;		        // Commando

 while(len)
  {
   if(len) { a = snd[ptr++]; len--;} else a = 0;
   if(len) { b = snd[ptr++]; len--;} else b = 0;
   if(len) { c = snd[ptr++]; len--;} else c = 0;
   SendeBuffer[pt++] = '=' + (a >> 2);
   SendeBuffer[pt++] = '=' + (((a & 0x03) << 4) | ((b & 0xf0) >> 4));
   SendeBuffer[pt++] = '=' + (((b & 0x0f) << 2) | ((c & 0xc0) >> 6));
   SendeBuffer[pt++] = '=' + ( c & 0x3f);
  }
 AddCRC(pt);
}



//############################################################################
//Routine für die Serielle Ausgabe
int uart_putchar (char c)
//############################################################################
{
	if (c == '\n')
		uart_putchar('\r');
	//Warten solange bis Zeichen gesendet wurde
	loop_until_bit_is_set(USR, UDRE);
	//Ausgabe des Zeichens
	UDR = c;
	
	return (0);
}

// --------------------------------------------------------------------------
void WriteProgramData(unsigned int pos, unsigned char wert)
{
}

//############################################################################
//INstallation der Seriellen Schnittstelle
void UART_Init (void)
//############################################################################
{
	//Enable TXEN im Register UCR TX-Data Enable & RX Enable

	UCR=(1 << TXEN) | (1 << RXEN);
    // UART Double Speed (U2X)
	USR   |= (1<<U2X);           
	// RX-Interrupt Freigabe
#if  X3D_SIO == 1
	UCSRB |= (1<<RXCIE);    // serieller Empfangsinterrupt       
#endif
	// TX-Interrupt Freigabe
//	UCSRB |= (1<<TXCIE);           

	//Teiler wird gesetzt 
	UBRR= (SYSCLK / (BAUD_RATE * 8L) -1 );
	//öffnet einen Kanal für printf (STDOUT)
	fdevopen (uart_putchar, NULL);
  Debug_Timer = SetDelay(200);   
UDR = '*';
}




//---------------------------------------------------------------------------------------------
void DatenUebertragung(void)  
{
 if((CheckDelay(Debug_Timer) && UebertragungAbgeschlossen))	 // im Singlestep-Betrieb in jedem Schtitt senden
    	 {
    	  SendOutData('D',MeineSlaveAdresse,(unsigned char *) &DebugOut,sizeof(DebugOut));
       	  Debug_Timer = SetDelay(50);   // Sendeintervall
    	 } 
}
