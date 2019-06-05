/*
 * Crazy string quartet code.
 * partial copyright 2002, Allan Hsu <allan@counterpop.net>
 */

#include <dsound.h>
#include <conio.h>
#include <unistd.h>
#include <tm.h>
#include <dsound.h>
#include <stdlib.h>
#include "metro_sound.h"

#define METRONOME_INTERVAL 100

// the first violin part
static const note_t violin1[] = {
  //first line
  { PITCH_PAUSE, 32},
  { PITCH_C4, 3 }, { PITCH_D4, 1 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  { PITCH_C4, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_D3, 2 },
  { PITCH_D3, 3 }, { PITCH_D3, 1 }, { PITCH_D3, 2 }, { PITCH_D3, 2 },
  { PITCH_D3, 2 }, { PITCH_D3, 2 }, { PITCH_D3, 2 }, { PITCH_C4, 2 },
  { PITCH_C4, 3 }, { PITCH_D4, 1 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  { PITCH_C4, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_D3, 2 },
  //second line
  { PITCH_D3, 3 }, { PITCH_D3, 1 }, { PITCH_D3, 2 }, { PITCH_D3, 2 },
  { PITCH_D3, 2 }, { PITCH_D3, 2 }, { PITCH_D3, 2 }, { PITCH_C4, 2 },
  { PITCH_C4, 3 }, { PITCH_D4, 1 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  { PITCH_C4, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_D3, 2 },
  { PITCH_C4, 3 }, { PITCH_D4, 1 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  { PITCH_C4, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_D3, 2 },
  { PITCH_C4, 3 }, { PITCH_D4, 1 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  //third line
  { PITCH_C4, 3 }, { PITCH_D4, 1 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  { PITCH_C4, 3 }, { PITCH_D4, 1 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  { PITCH_F4, 2 }, { PITCH_D4, 2 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  { PITCH_F4, 2 }, { PITCH_C4, 2 }, { PITCH_Am4, 2}, { PITCH_G3, 2 },
  { PITCH_F4, 2 }, { PITCH_C4, 2 }, { PITCH_Am4, 2}, { PITCH_G3, 2 },
  { PITCH_F4, 2 }, { PITCH_C4, 2 }, { PITCH_Am4, 2}, { PITCH_G3, 2 },
  { PITCH_F4, 2 }, { PITCH_C4, 2 }, { PITCH_Am4, 2}, { PITCH_G3, 2 },
  //fourth line
  { PITCH_F3, 4 }, { PITCH_PAUSE, 4},
  { PITCH_PAUSE, 24},
  { PITCH_A5, 4 }, { PITCH_C5, 2}, { PITCH_PAUSE, 2},
  { PITCH_PAUSE, 8},
  { PITCH_A5, 4 }, { PITCH_C5, 2}, { PITCH_PAUSE, 2},
  { PITCH_PAUSE, 8},
  { PITCH_PAUSE, 4}, { PITCH_C5, 3 }, { PITCH_D5, 1 },
  { PITCH_C5, 2 }, { PITCH_A5, 2 }, { PITCH_F4, 2 }, { PITCH_D4, 2},
  { PITCH_C4, 2 }, { PITCH_A4, 2 }, { PITCH_C4, 4},
  // fifth line
  { PITCH_A4, 2 }, { PITCH_PAUSE, 4}, { PITCH_F4, 2},
  { PITCH_G4, 2 }, { PITCH_F4, 2}, { PITCH_D4, 4},
  { PITCH_D4, 2 }, { PITCH_C4, 2}, { PITCH_Am4, 2 }, { PITCH_F4, 2},
  { PITCH_C4, 4 }, { PITCH_A4, 2}, { PITCH_PAUSE, 2},
  { PITCH_PAUSE, 4}, { PITCH_C5, 3 }, { PITCH_D5, 1},
  { PITCH_C5, 2 }, { PITCH_A5, 2 }, { PITCH_F4, 2 }, { PITCH_D4, 2 },
  { PITCH_C4, 2 }, { PITCH_A4, 2 }, { PITCH_C4, 4 },
  // sixth line
  { PITCH_A4, 4 }, { PITCH_PAUSE, 2 }, { PITCH_F4, 2 },
  { PITCH_E4, 3 }, { PITCH_F4, 1 }, { PITCH_E4, 2 }, { PITCH_H4, 2 },
  { PITCH_E4, 3 }, { PITCH_F4, 1 }, { PITCH_E4, 2 }, { PITCH_C4, 2 },
  { PITCH_E4, 3 }, { PITCH_F4, 1 }, { PITCH_E4, 2 }, { PITCH_H4, 2 },
  { PITCH_A4, 2 }, { PITCH_E3, 2 }, { PITCH_A4, 2 }, { PITCH_E3, 2 },
  { PITCH_A4, 2 }, { PITCH_E3, 2 }, { PITCH_A4, 2 }, { PITCH_E3, 2 },
  { PITCH_A4, 2 }, { PITCH_PAUSE, 2 }, { PITCH_PAUSE, 4 },
  { PITCH_PAUSE, 8 },

  { PITCH_PAUSE, 16} ,  { PITCH_END, 0}
};

// the viola part
static const note_t viola[] = {
  //junky crap at the beginning.
  { PITCH_F2, 3}, { PITCH_F2, 1}, {PITCH_F2, 2}, {PITCH_F2, 4}, {PITCH_F2, 2}, {PITCH_F2, 2}, {PITCH_F2, 2},
  { PITCH_F2, 3}, { PITCH_F2, 1}, {PITCH_F2, 2}, {PITCH_F2, 4}, {PITCH_F2, 2}, {PITCH_F2, 2}, {PITCH_F2, 2},
  { PITCH_F2, 3}, { PITCH_F2, 1}, {PITCH_F2, 2}, {PITCH_F2, 4}, {PITCH_F2, 2}, {PITCH_F2, 2}, {PITCH_F2, 2},
  { PITCH_F2, 3}, { PITCH_F2, 1}, {PITCH_F2, 2}, {PITCH_F2, 4}, {PITCH_F2, 2}, {PITCH_F2, 2}, {PITCH_F2, 2},
  { PITCH_F2, 3}, { PITCH_F2, 1}, {PITCH_F2, 2}, {PITCH_F2, 4}, {PITCH_F2, 2}, {PITCH_F2, 2}, {PITCH_F2, 2},
  { PITCH_F2, 3}, { PITCH_F2, 1}, {PITCH_F2, 2}, {PITCH_F2, 4}, {PITCH_F2, 2}, {PITCH_F2, 2}, {PITCH_F2, 2},
  { PITCH_F2, 3}, { PITCH_F2, 1}, {PITCH_F2, 2}, {PITCH_F2, 4}, {PITCH_F2, 2}, {PITCH_F2, 2}, {PITCH_F2, 2},
  { PITCH_F2, 3}, { PITCH_F2, 1}, {PITCH_F2, 2}, {PITCH_F2, 4}, {PITCH_F2, 2}, {PITCH_F2, 2}, {PITCH_F2, 2},
  //third line, where the monotony changes a little
  { PITCH_F2, 3 }, { PITCH_F2, 1 }, { PITCH_F2, 2 }, { PITCH_F2, 2 },
  { PITCH_A3, 2 }, { PITCH_A3, 2 }, { PITCH_G2, 2 }, { PITCH_G2, 2 },
  { PITCH_A3, 2 }, { PITCH_A3, 2 }, { PITCH_G2, 2 }, { PITCH_G2, 2 },
  { PITCH_D3, 2 }, { PITCH_G2, 2 }, { PITCH_D3, 2 }, { PITCH_G2, 2 },
  { PITCH_A3, 2 }, { PITCH_F2, 2 }, { PITCH_C3, 4 },
  { PITCH_A3, 2 }, { PITCH_F2, 2 }, { PITCH_C3, 4 },
  //fourth line
  { PITCH_A3, 2 }, { PITCH_F2, 2 }, { PITCH_C3, 4 },
  { PITCH_A3, 2 }, { PITCH_F2, 2 }, { PITCH_C3, 4 },
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  //fifth line. this is the most boring viola part ever.
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  { PITCH_A3, 2 }, { PITCH_C3, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 },
  { PITCH_A3, 2 }, { PITCH_C3, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 },
  { PITCH_A3, 2 }, { PITCH_C3, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 },
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2},
  //sixth line
  { PITCH_D3, 2 }, { PITCH_F3, 2 }, { PITCH_Am4, 2 }, { PITCH_F3, 2 },
  { PITCH_Am4, 2 }, { PITCH_F3, 2 }, { PITCH_D3, 2 }, { PITCH_Am3, 2 },
  { PITCH_A3, 2 }, { PITCH_C3, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 },
  { PITCH_A3, 2 }, { PITCH_C3, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 },
  { PITCH_A3, 2 }, { PITCH_C3, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 },
  { PITCH_A3, 2 }, { PITCH_C3, 2 }, { PITCH_A4, 2 }, { PITCH_F3, 2 },
  { PITCH_A4, 2 }, { PITCH_F3, 2 }, { PITCH_C3, 2 }, { PITCH_A3, 2 },
  //seventh line
  { PITCH_H3, 2 }, { PITCH_Gm2, 2 }, { PITCH_H3, 2 }, { PITCH_E3, 2 },
  { PITCH_C3, 2 }, { PITCH_A3, 2 }, { PITCH_C3, 2 }, { PITCH_E3, 2 },
  { PITCH_H3, 2 }, { PITCH_Gm2, 2 }, { PITCH_H3, 2 }, { PITCH_E3, 2 },
  { PITCH_A3, 2 }, { PITCH_PAUSE, 2 }, { PITCH_A3, 2 }, { PITCH_PAUSE, 2 },
  { PITCH_A3, 2 }, { PITCH_PAUSE, 2 }, { PITCH_A3, 2 }, { PITCH_PAUSE, 2 },
  
  { PITCH_PAUSE, 16},

  { PITCH_PAUSE, 16} ,  { PITCH_END, 0}
};

// the cello part
static const note_t cello[] = {
  //first line
  { PITCH_PAUSE, 8},
  { PITCH_F1, 2 }, { PITCH_PAUSE, 2 }, { PITCH_F2, 2}, { PITCH_PAUSE, 2 },
  { PITCH_PAUSE, 8},
  { PITCH_F1, 2 }, { PITCH_PAUSE, 2 }, { PITCH_F2, 2}, { PITCH_PAUSE, 2 },
  { PITCH_PAUSE, 16},
  { PITCH_D1, 2 }, { PITCH_PAUSE, 2 }, { PITCH_F1, 2}, { PITCH_PAUSE, 2 },
  { PITCH_A2, 2 }, { PITCH_PAUSE, 2 }, { PITCH_D2, 2}, { PITCH_F2, 2},
  { PITCH_PAUSE, 16},
  { PITCH_D1, 2 }, { PITCH_PAUSE, 2 }, { PITCH_F1, 2}, { PITCH_PAUSE, 2 },
  //second line
  { PITCH_A2, 2 }, { PITCH_PAUSE, 2 }, { PITCH_D2, 2}, { PITCH_F2, 2},
  { PITCH_PAUSE, 8},
  { PITCH_D2, 4 }, { PITCH_PAUSE, 2 }, { PITCH_D2, 2},
  { PITCH_C2, 4 }, { PITCH_PAUSE, 4 },
  { PITCH_C2, 4 }, { PITCH_PAUSE, 2 }, { PITCH_C2, 2},
  { PITCH_Am2, 4}, { PITCH_A2, 4 },
  { PITCH_A2, 4 }, { PITCH_H2, 4 },
  { PITCH_C2, 4 }, { PITCH_H2, 4 },
  { PITCH_H2, 4 }, { PITCH_H2, 4 },
  { PITCH_C2, 2 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 }, { PITCH_C1, 2},
  // third line
  { PITCH_C2, 2 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 }, { PITCH_C1, 2},
  { PITCH_C2, 2 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 }, { PITCH_C1, 2},
  { PITCH_C2, 2 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 }, { PITCH_C1, 2},
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  // fourth line
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F2, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F2, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F2, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_Am2, 2 },
  { PITCH_F1, 4 }, { PITCH_PAUSE, 2}, { PITCH_Am2, 2 },
  // fifth line
  { PITCH_A2, 4 }, { PITCH_F1, 2 }, { PITCH_C2, 2 },
  { PITCH_F2, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F2, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F2, 4 }, { PITCH_PAUSE, 2}, { PITCH_C2, 2 },
  { PITCH_F2, 4 }, { PITCH_PAUSE, 2}, { PITCH_A2, 2 },
  { PITCH_Gm1, 2}, { PITCH_E1, 2 }, { PITCH_Gm1, 2}, { PITCH_E1, 2 },
  { PITCH_A2, 2}, { PITCH_E1, 2 }, { PITCH_A2, 2}, { PITCH_E1, 2 },
  { PITCH_Gm1, 2}, { PITCH_E1, 2 }, { PITCH_Gm1, 2}, { PITCH_E1, 2 },
  //sixth line
  { PITCH_A2, 2 }, { PITCH_PAUSE, 2 }, { PITCH_A2, 2 }, { PITCH_PAUSE, 2 },
  { PITCH_A2, 2 }, { PITCH_PAUSE, 2 }, { PITCH_A2, 2 }, { PITCH_PAUSE, 2 },
  { PITCH_A2, 2 }, { PITCH_PAUSE, 2 }, { PITCH_A2, 2 }, { PITCH_PAUSE, 2 },
  { PITCH_A2, 2 }, { PITCH_PAUSE, 2 }, { PITCH_PAUSE, 4 },


  { PITCH_PAUSE, 16}, { PITCH_END, 0}
};

typedef enum {
  VIOLIN1,
  //  VIOLIN2,
  VIOLA,
  CELLO
} quartet_instrument;

int metronome()
{
  time_t metrotime = 0; 
  int go_time = 200;
  quartet_instrument instr = VIOLIN1;  
  dsound_set_duration(100);
  while(1) {
    //lock that sucker
    metrotime++;
    if(metrotime == go_time) {
      if(instr == VIOLIN1) dsound_play(violin1);
      else if(instr == CELLO) dsound_play(cello);
      else if(instr == VIOLA) dsound_play(viola);
      wait_event(dsound_finished,0);
      metrotime = 0;
      instr = VIOLIN1;
    }
    lcd_int(go_time - metrotime);
    msleep(METRONOME_INTERVAL);
  }
  return(0);
}


