#include <unistd.h>
#include <dsensor.h>
#include <sys/tm.h>
#include <sys/lcd.h>
#include "repos.h"
#include "annotations.h"
#include <time.h>
#include <sys/battery.h>

#define SENSE_TIME 10 //time to make sensory decision
extern bool_t bump;
extern repos_t repos;

/*Avant garde: experimental electronic music*/
inline bumped_t check_bump()
{
  if (TOUCH_1 || TOUCH_3){
    //    set_bumped(&repo, YES);
    repos.bumped = YES; //Debug
    bump = TRUE; //Debug
    return YES;
  }
  else {
    //  set_bumped(&repo, NO);
    return NO;
  }
}


int scan_sensors()
{
  timestamp_t t;
  bat_level_t bat; 
  bumped_t     b;
  while (1)
    {            
      /* scan the sensors */
      /* add more sensor data later */
      ask_cpu();
      lcd_refresh(); 
      t = sys_time;
      bat = get_battery_mv(); 
      b = check_bump();
      /* bool_t is_con = get_is_connected();*/
      
      /* write the data into repos */
      get_repos();
      set_is_fresh(&repo, FALSE);
      set_timestamp(&repo, t);
      set_bat_level(&repos, bat);
      set_bumped (&repo, b);
      set_is_fresh(&repo, TRUE);
      release_repos();
      release_cpu();
      msleep(SENSE_TIME);
    }
  return 1;
}



