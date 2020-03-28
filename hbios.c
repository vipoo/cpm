#include "defs.h"
#include "hbios_mocks.h"

#define BF_VDA 0x40
#define BF_VDAIO (BF_VDA + 15)

#define BF_SND 0x50
#define BF_SNDINI (BF_SND + 0)
#define BF_SNDVOL (BF_SND + 1)
#define BF_SNDPIT (BF_SND + 2)

#define BF_SYSGET 0xF8
#define BF_SYSVER 0xF1

void hbiosCall(z80info *z80) {

  int i = 0;

  if (activeCommandCount) {
    for(i = 0; i < activeCommandCount; i++) {
      printf("> %s", strs[i][0]);
      printf("< %s", strs[i][1]);
    }
  }


  switch (B) {
  case BF_VDAIO:
    printf("\r\nHBIOS: BF_VDAIO\r\n");
    /* Hard coded for the one HBIOS call */
    D = 0xBE;
    A = 0;
    break;

  case BF_SNDINI:
    printf("\r\nHBIOS: BF_SNDINI\r\n");
    break;

  case BF_SNDVOL:
    printf("\r\nHBIOS: BF_SNDVOL, Driver in C %02X, Channel in D: %02X, Volume in E: %02X\r\n", C, D, E);
    break;

  case BF_SNDPIT:
    printf("\r\nHBIOS: BF_SNDPIT, Driver in C %02X, Channel in D: %02X, Pitch in HL: %02X%02X \r\n", C, D, H, L);
    break;

  case BF_SYSGET: {
    printf("\r\nHBIOS: BF_SYSGET, Subfunction: in C %02X\r\n", C);
    switch(C) {
      case BF_SND:
        E = 1;
        A = 0;
        break;
    }
    break;
  }

  case BF_SYSVER:
    printf("\r\nHBIOS: BF_SYSVER\r\n");
    A = 0;    /* Previously Active Bank*/
    D = 0xDD; /* Version number*/
    E = 0xEE;
    L = 7;    /*RCZ80*/
    break;

  default:
    printf("\r\nHBIOS: %0x", B);
  }
}