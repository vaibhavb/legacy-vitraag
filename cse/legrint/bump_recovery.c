#include <conio.h>
#include <unistd.h>
#include <dsensor.h>
#include <dmotor.h>
#include <lnp-logical.h>
#include <lnp.h>
#include <dbutton.h>
#include <sys/tm.h>
#include <mem.h>
#include <string.h>

#include "repos.h"
#include "bump_recovery.h"
#include "annotations.h"

#define MOTOR_BUMP_TIME 700  //time to bump retrace

/*This is wake-up function checks for being bumped.*/ 
wakeup_t wait_for_bump(wakeup_t data)
{
  if (get_bumped(&repo)== YES){
    return 1;
  }
  else return 0;
}


/**Not a very smart recovery*/  
int recover_from_bump()
{
  while(1)
    {
      wait_event(wait_for_bump, 0);
      ask_motors();
      /*do the recovery*/
      /*back for a little-while with constant speed*/
      motor_a_dir (rev); 
      motor_c_dir (rev); 
      motor_a_speed(MAX_SPEED / 2); 
      motor_c_speed(MAX_SPEED / 2);
      msleep(MOTOR_BUMP_TIME);
      release_motors(); 
      set_bumped(&repo, NO);
    }
  return 1;
}

