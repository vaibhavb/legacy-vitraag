#include <conio.h>
#include <unistd.h>
#include <dsensor.h>
#include <dmotor.h>
#include <lnp-logical.h>
#include <lnp.h>
#include <dbutton.h>
#include <sys/tm.h>
#include <rom/system.h>
#include <mem.h>
#include <string.h>
#include "repos.h"
#include "motion.h"

#define SENSE_TIME 100 //time to make sensory decision
#define MOTOR_DIR_TIME 2 //time allowed to change direction

int seek_light();
wakeup_t wait_for_nobump(wakeup_t );
inline void move_left(int );
inline void move_right(int );
inline void move_straight(int);

int motion()
{
  
  switch (get_act_mode(&repo)){
  case SEEK_LIGHT:
    seek_light();
    break;
  case GO_WALL:
    
    break;
  case ST_LINE:
    
    break;
  default:
    break;
  }
  return 1;
}
  
wakeup_t wait_for_nobump(wakeup_t data)
{
  if (get_bumped(&repo)==NO){
    return 1;
  }
  else return 0;
}


/*Note light intensity is betw 0-100
 *Motor speed is between 0-255
 */
int seek_light()
{
  int light_val_c =0;
  int light_val_l =0;
  int light_val_r =0;
  int light_max =0;
  int speed_prop = 0;
  move_straight(MAX_SPEED);
  msleep(SENSE_TIME); //don't help much
  ds_active(&SENSOR_2);
  while(1)
    {
      wait_event(wait_for_nobump, 0);
      /*sample light*/
      light_val_c = light_max = LIGHT(SENSOR_2);

      /*move left and sample*/
      move_left(MAX_SPEED);
      light_val_l = LIGHT(SENSOR_2);
      if (light_val_l > light_max)
        light_max = light_val_l;

      /*move right and sample*/
      move_right(MAX_SPEED);
      light_val_r = LIGHT(SENSOR_2);
      if (light_val_r > light_max)
        light_max = light_val_r;

      /*decide the max light
       *prefer straight and move proportional to
       *light intensity*/
      speed_prop = (MAX_SPEED*light_max)/LIGHT_MAX;
      if (light_max == light_val_l) move_left(speed_prop);
      else if (light_max == light_val_r) move_right(speed_prop);
      else move_straight(speed_prop);
      /*take some time before making next decision*/
      msleep(SENSE_TIME);
    }
  return 1;
}

inline void move_left(int speed)
{
  motor_a_dir(rev);
  motor_c_dir(fwd);
  motor_a_speed(speed/2);
  motor_c_speed(speed);
  msleep(MOTOR_DIR_TIME);
}

inline void move_right(int speed)
{
  motor_a_dir(fwd);
  motor_c_dir(rev);
  motor_a_speed(speed);
  motor_c_speed(speed/2);
  msleep(MOTOR_DIR_TIME);
}

inline void move_straight(int speed)
{
  motor_a_dir(fwd);
  motor_c_dir(fwd);
  motor_a_speed(speed);
  motor_c_speed(speed);
  msleep(MOTOR_DIR_TIME);
}

