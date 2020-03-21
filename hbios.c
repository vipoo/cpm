#include "defs.h"

void hbiosCall(z80info *z80) {
  printf("\r\nRST 08\r\n");
  /* Hard coded for the one HBIOS call */
  D = 0xBE;
  A = 0;
}