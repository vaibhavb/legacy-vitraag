#include <conio.h> 
#include <unistd.h>
#include <tm.h>
#include <sys/battery.h>
#include <sys/lcd.h>
#include <dmotor.h>
#include "motion.h"
#include "bump_recovery.h"
#include "scan_sensors.h"

void show_resintf (void)
{
  while(1)
  {		
    cputs("res");
    sleep(1);
    cputs("intfz");
    sleep(1);
    //wait_event(toggle, 0);
  }
  return;
}    	


void show_battery (void)
{
  int i;	
  while(1){
    i = get_battery_mv(); 
    lcd_int(i);	
    sleep(4);	
    //wait_event(toggle, 1);
  }
  return;
}    	

void run (void)
{
  int speed=50, dir=1;
  motor_a_dir(dir);
  motor_a_speed(speed);
  motor_c_dir(dir);
  motor_c_speed(speed);
  return ;
}

int main(int argc, char **argv) {
  pid_t pid1, pid2, pid3, pid4, pid5;
  pid2 = execi(&show_battery, 0, NULL, 1, 
			DEFAULT_STACK_SIZE);
  pid1 = execi(&show_resintf, 0, NULL, 1, 
			DEFAULT_STACK_SIZE);
  pid3 = execi(&motion, 0, NULL, 1, 
			DEFAULT_STACK_SIZE);
  pid4 = execi(&recover_from_bump, 0, NULL, 1, 
			DEFAULT_STACK_SIZE);
  pid5 = execi(&scan_sensors, 0, NULL, 1, 
			DEFAULT_STACK_SIZE);
  return 1;
}


/* Battery Indicator Routine */
/* New processes by execi */
/* Waitevent wait for something to event to handle and 
proceeds when the function returns a non-NULL value */
/*
int turn=0;

wakeup_t toggle(wakeup_t i)
{
 if ((int)i == 0) {
    if (turn == 0){
	 turn = 1;
	 return 0;
    }
    else {
         turn = 0;
	 return 1;
    }
 }
 else {
    if (turn == 1){
	 turn = 0;
	 return 1;
    }
    else {
	 turn = 1;
	 return 0;
    }
 }
}	


*/






