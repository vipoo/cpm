#include "defs.h"

#define BF_VDA 0x40
#define BF_IOPORTS (BF_VDA + 15)

#define BF_SND 0x50
#define BF_SNDINI (BF_SND + 0)

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
  }
}