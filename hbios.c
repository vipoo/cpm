#include "defs.h"

#define BF_VDA 0x40
#define BF_IOPORTS (BF_VDA + 15)

#define BF_SND 0x50
#define BF_SNDINI (BF_SND + 0)
#define BF_SNDVOL (BF_SND + 1)
#define BF_SNDPIT (BF_SND + 2)

void hbiosCall(z80info *z80) {

  switch (B) {
  case BF_IOPORTS:
    printf("\r\nHBIOS: BF_IOPORTS\r\n");
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

  default:
    printf("\r\nHBIOS: %0x", B);
  }
}