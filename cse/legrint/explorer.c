#include <conio.h> 
#include <unistd.h>
#include <tm.h>
#include <sys/battery.h>
#include <sys/lcd.h>
#include <dmotor.h>
#include <dsensor.h>
#include "repos.h"
#include "motion.h"
#include "bump_recovery.h"
#include "scan_sensors.h"
#include "telemetry.h"
#include "metro_sound.h"

repos_t repo; // The TRUE repository
repos_t repos;
bool_t bump = FALSE;

/* shows bump status */
int show_bump ()
{
  
  while(1)
  {		
    if (get_act_mode(&repo)== SEEK_LIGHT){
    //if (repos.bumped == YES){
    /*if (TOUCH_1 || TOUCH_3){*/
    //if (bump){
      //cputs("bump");
      lcd_int(LIGHT(SENSOR_2));
      msleep(100);
      //set_bumped(&repo, NO);
      repos.bumped=NO;
      bump = FALSE;
    }
    else {
      cputs("fine");
      msleep(100);      
    }
    //wait_event(toggle, 0);
  }
  return 1;
}    	

/* shows battery status */
int show_battery ()
{
  int i; 
  while(1){
    i = get_battery_mv();
    lcd_int(i);	
    set_bumped(&repo, YES);
    sleep(3);
    //wait_event(toggle, 1);
  }
  return 1;
}    	

int main(int argc, char **argv) {
  pid_t pid1, pid2, pid3, pid4, pid5, pid6;
  repos.bumped=NO;
  init_repos(&repo);
  pid6 = execi(&metronome, 0, NULL, 1, 
	       DEFAULT_STACK_SIZE);
  pid2 = execi(&show_bump, 0, NULL, 1, 
	       DEFAULT_STACK_SIZE);
  pid1 = execi(&telemetry, 0, NULL, 1, 
	       DEFAULT_STACK_SIZE); 
  pid5 = execi(&scan_sensors, 0, NULL, 1, 
	       DEFAULT_STACK_SIZE);
  pid3 = execi(&motion, 0, NULL, 1, 
	       DEFAULT_STACK_SIZE);
  pid4 = execi(&recover_from_bump, 0, NULL, 1, 
	       DEFAULT_STACK_SIZE);
  return 1;
}

