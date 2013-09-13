/*
   For backwards compatibility only. 
  
   Ingo Busker   ingo@mikrocontroller.com
*/

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef inb
#define inb(sfr) _SFR_BYTE(sfr)
#endif

#ifndef outb
#define outb(sfr, val) (_SFR_BYTE(sfr) = (val))
#endif

#ifndef inw
#define inw(sfr) _SFR_WORD(sfr)
#endif

#ifndef outw
#define outw(sfr, val) (_SFR_WORD(sfr) = (val))
#endif

#ifndef outp
#define outp(val, sfr) outb(sfr, val)
#endif

#ifndef inp
#define inp(sfr) inb(sfr)
#endif

#ifndef BV
#define BV(bit) _BV(bit)
#endif


#ifndef PRG_RDB
#define PRG_RDB pgm_read_byte
#endif

