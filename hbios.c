#include "defs.h"
#include "hbios_mocks.h"
#include <stdlib.h>

#define BF_VDA   0x40
#define BF_VDAIO (BF_VDA + 15)

#define BF_SND    0x50
#define BF_SNDINI (BF_SND + 0)
#define BF_SNDVOL (BF_SND + 1)
#define BF_SNDPIT (BF_SND + 2)

#define BF_SYSGET 0xF8
#define BF_SYSVER 0xF1

void    testMock(z80info *, char *, char *returnRow);
boolean isMatch(z80info *z80, char *row);
void    applyMock(z80info *z80, char *returnRow);

void hbiosCall(z80info *z80) {

  int i = 0;

  if (activeCommandCount) {
    for (i = 0; i < activeCommandCount; i++) {
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
    switch (C) {
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
    L = 7; /*RCZ80*/
    break;

  default:
    printf("\r\nHBIOS: %0x", B);
  }

  if (activeCommandCount) {
    for (i = 0; i < activeCommandCount; i++) {
      printf("> %s", strs[i][0]);

      if (strs[i][0][0] == '>') {
        printf("Matching >");
        if (isMatch(z80, strs[i][0]))
          applyMock(z80, strs[i][1]);
      }
    }
  }
}

void testMock(z80info *z80, char *row, char *returnRow) {

  char *item = row;
  int   mockB;

  while (*item) {
    printf("looking at item %c\r\n", *item);
    if (*item == 'B') {
      mockB = (int)strtol(&item[2], NULL, 16);
      if (mockB == B) {
        printf("Mocking b.....\r\n");
        *row = '-';
        applyMock(z80, returnRow);
        break;
      }
    }
    item++;
  }
}

boolean isMatch(z80info *z80, char *row) {
  int   regVal = 0;
  char *item   = row + 2;

  while (*item) {
    regVal = (int)strtol(&item[2], NULL, 16);

    switch (*item) {
    case 'A':
      if (A != regVal)
        return FALSE;
      break;

    case 'B':
      if (B != regVal)
        return FALSE;
      break;

    case 'C':
      if (C != regVal)
        return FALSE;
      break;

    case 'D':
      if (D != regVal)
        return FALSE;
      break;

    case 'E':
      if (E != regVal)
        return FALSE;
      break;

    case 'H':
      if (H != regVal)
        return FALSE;
      break;

    case 'L':
      if (L != regVal)
        return FALSE;
      break;
    }

    item += 5;
  }

  return TRUE;
}

void applyMock(z80info *z80, char *returnRow) {

  int   regVal = 0;
  char *item   = returnRow + 2;

  while (*item) {
    regVal = (int)strtol(&item[2], NULL, 16);

    switch (*item) {
    case 'A':
      A = regVal;
      break;

    case 'B':
      B = regVal;
      break;

    case 'C':
      C = regVal;
      break;

    case 'D':
      D = regVal;
      break;

    case 'E':
      E = regVal;
      break;

    case 'H':
      H = regVal;
      break;

    case 'L':
      L = regVal;
      break;
    }

    item += 5;
  }
}
