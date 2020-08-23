#include "defs.h"
#include "hbios_mocks.h"
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <sys/select.h>
#include <stropts.h>

int kbhit()
{
    struct termios term;
    struct termios term2;
    int byteswaiting;
    tcgetattr(0, &term);

    term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);
    ioctl(0, FIONREAD, &byteswaiting);
    tcsetattr(0, TCSANOW, &term);

    return byteswaiting > 0;
}

#define BF_CIO		  0x00
#define BF_CIOIN		BF_CIO + 0	/* CHARACTER INPUT */
#define BF_CIOOUT   BF_CIO + 1	/* CHARACTER OUTPUT */
#define BF_CIOIST   BF_CIO + 2  /* CHARACTER INPUT STATUS */

#define BF_SYS	    0xF0
#define BF_SYSGET	  BF_SYS + 8	/* ; GET HBIOS INFO */

#define BF_SYSGET_TIMER		0xD0	/* GET CURRENT TIMER VALUE */

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

#define BF_SYSSET 0xF9
#define BF_SYSVER 0xF1


struct registers {
  int a;
  int b;
  int c;
  int d;
  int e;
  int h;
  int l;
};

void    testMock(z80info *, char *, char *returnRow);
boolean isMatch(struct registers *, char *row);
void    applyMock(z80info *z80, char *returnRow);

long current_time_at_50hz(void) {
  long            ms;
  long  s;
  struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);

  ms = spec.tv_nsec / 10000;

  s  = spec.tv_sec - 1588911417;
  ms = spec.tv_nsec / 1.0e6;
  if (ms > 999) {
      s++;
      ms = 0;
  }

  return ((s * 1000 + ms)) * 50 / 1000;
}


void hbiosCall(z80info *z80) {

  int i = 0;
  long ticks;

  struct registers r;
  r.b = B;
  r.c = C;
  r.d = D;
  r.e = E;
  r.h = H;
  r.l = L;

  switch (B) {
  case BF_VDAIO:
    printf("\r\nHBIOS: BF_VDAIO\r\n");
    /* Hard coded for the one HBIOS call */
    D = 0xBE;
    A = 0;
    break;

  case BF_CIOIN:
    E = getchar();
    A = 0;
    break;

  case BF_CIOIST:
    A = kbhit();
    break;

  case BF_CIOOUT:
    printf("%c", E);
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
    switch(C) {
    case BF_SYSGET_TIMER:
      ticks = current_time_at_50hz();

      A = 0;
      D = (ticks >> 24) & 0xFF;
      E = (ticks >> 16) & 0xFF;
      H = (ticks >> 8) & 0xFF;
      L = ticks & 0xFF;

      C = 50;
      B = 0;
      break;

    default:
      printf("\r\nHBIOS: BF_SYSGET, Subfunction: in C %02X\r\n", C);
    }

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
    printf("\r\nHBIOS: B: %0X, C: %0X, D: %0X, E: %0X, H: %0X, L: %0X\r\n", B, C, D, E, H, L);
  }

  if (activeCommandCount) {
    for (i = 0; i < activeCommandCount; i++) {
      if (strs[i][0][0] == '>') {
        if (isMatch(&r, strs[i][0]))
          applyMock(z80, strs[i][1]);
      }
    }
  }
}

boolean isMatch(struct registers *r, char *row) {
  int   regVal = 0;
  char *item   = row + 2;

  while (*item) {
    regVal = (int)strtol(&item[2], NULL, 16);

    switch (*item) {
    case 'A':
      if (r->a != regVal)
        return FALSE;
      break;

    case 'B':
      if (r->b != regVal)
        return FALSE;
      break;

    case 'C':
      if (r->c != regVal)
        return FALSE;
      break;

    case 'D':
      if (r->d != regVal)
        return FALSE;
      break;

    case 'E':
      if (r->e != regVal)
        return FALSE;
      break;

    case 'H':
      if (r->h != regVal)
        return FALSE;
      break;

    case 'L':
      if (r->l != regVal)
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
