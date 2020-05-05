#include "defs.h"
#include "hbios_mocks.h"
#include <stdlib.h>

#define BF_VDA   0x40
#define BF_VDAIO (BF_VDA + 15)

#define BF_SNDQ_STATUS  0
#define BF_SNDQ_CHCNT	  BF_SNDQ_STATUS + 1	/* RETURN COUNT OF CHANNELS */
#define BF_SNDQ_VOLUME	BF_SNDQ_STATUS + 2	/* 8 BIT NUMBER */
#define BF_SNDQ_PERIOD	BF_SNDQ_STATUS + 3	/* 16 BIT NUMBER */
#define BF_SNDQ_DEV			BF_SNDQ_STATUS + 4	/* RETURN DEVICE TYPE CODE AND IO PORTS - TYPE IN B, PORTS IN DE, HL */

#define BF_SND              0x50
#define BF_SNDRESET         BF_SND + 0	/* RESET SOUND SYSTEM */
#define BF_SNDVOL           BF_SND + 1	/* REQUEST SOUND VOL - L CONTAINS VOLUME (255 MAX, 0 SILENT) - SCALED AS REQUIRED BY DRIVER (EG: MAPS TO JUST 4 BIT RESOLUTION FOR SN76489) */
#define BF_SNDPRD           BF_SND + 2	/* REQUEST SOUND PERIOD - HL CONTAINS DRIVER SPECIFIC VALUE */
#define BF_SNDNOTE          BF_SND + 3	/* REQUEST NOTE - L CONTAINS NOTE - EACH VALUE IS QUARTER NOTE */
#define BF_SNDPLAY          BF_SND + 4	/* INITIATE THE REQUESTED SOUND COMMAND */
#define BF_SNDQUERY         BF_SND + 5	/* E IS SUBFUNCTION */

#define BF_SYSGET 0xF8
#define BF_SYSSET 0xF9
#define BF_SYSVER 0xF1

void    testMock(z80info *, char *, char *returnRow);
boolean isMatch(z80info *z80, char *row);
void    applyMock(z80info *z80, char *returnRow);

void hbiosCall(z80info *z80) {

  int i = 0;

  switch (B) {
  case BF_VDAIO:
    printf("\r\nHBIOS: BF_VDAIO\r\n");
    /* Hard coded for the one HBIOS call */
    D = 0xBE;
    A = 0;
    break;

  case BF_SNDRESET:
    printf("\r\nHBIOS: BF_SNDRESET\r\n");
    break;

  case BF_SNDVOL:
    printf("\r\nHBIOS: BF_SNDVOL, Driver in C %02X, Volume in L: %02X\r\n", C, L);
    break;

  case BF_SNDPRD:
    printf("\r\nHBIOS: BF_SNDPRD, Driver in C %02X, Pitch in HL: %02X%02X \r\n", C, H, L);
    break;

  case BF_SNDNOTE:
    printf("\r\nHBIOS: BF_SNDNOTE, Driver in C %02X, Note in HL: %02X%02X\r\n", C, H, L);
    break;

  case BF_SNDPLAY:
    printf("\r\nHBIOS: BF_SNDPLAY, Driver in C %02X, Channel in D: %02X\r\n", C, D);
    break;

  case BF_SNDQUERY:
    printf("\r\nHBIOS: BF_SNDQUERY, Driver in C %02X, SubCommand: %02X\r\n", C, E);
    break;

  case BF_SYSGET: {
    printf("\r\nHBIOS: BF_SYSGET, Subfunction: in C %02X\r\n", C);
    break;
  }

  case BF_SYSSET: {
    printf("\r\nHBIOS: BF_SYSSET, Subfunction: in C %02X, DE:HL: %02X%02X:%02X%02X\r\n", C, D, E, H, L);
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
    printf("\r\nHBIOS: B: %0X, C: %0X, D: %0X, E: %0X, H: %0X, L: %0X", B, C, D, E, H, L);
  }

  if (activeCommandCount) {
    for (i = 0; i < activeCommandCount; i++) {
      /*printf("> %s", strs[i][0]);*/

      if (strs[i][0][0] == '>') {
        /*printf("Matching >");*/
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
        printf("Mocking .....\r\n");
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

    /*printf("Testing %c. %02X\r\n", *item, regVal);*/

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

  /*printf("Matched.\r\n");*/

  return TRUE;
}

void applyMock(z80info *z80, char *returnRow) {

  int   regVal = 0;
  char *item   = returnRow + 2;

  /*printf("Applying mock.\r\n");*/

  while (*item) {
    regVal = (int)strtol(&item[2], NULL, 16);

    printf("Setting %c to %02X\r\n", *item, regVal);


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
