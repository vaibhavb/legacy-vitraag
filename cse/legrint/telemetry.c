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
#include "telemetry.h"
#include "repos.h"
#include "annotations.h"

/** Communication with the base station through LNP */
#define LDIST_POWER 500
#define SDIST_POWER 200

/* lnp port definitions */ 
#define MY_PORT   0x2
#define DEST_PORT 0x1
#define MY_HOST   0x0
#define DEST_HOST 0x0
#define DEST_ADDR ( DEST_HOST << 4 | DEST_PORT )
/* range of IR sensor 
#define IR_NEAR 0
#define IR_FAR  1
*/


/* Stuff for receive buffer */
int gNewData = 0;
unsigned char gMessagingData[256];
int gDataLength;

/* This is the wake-up function that checks for new data. */ 
wakeup_t WaitForData(wakeup_t data)
{
  return gNewData;
}

/* Incoming packet handler */
void packet_handler(const unsigned char* data,
                    unsigned char length,
                    unsigned char src)
{
  /* If the buffer is full, we just lose the data. */
  if(gNewData == 0)
  {
    memcpy(gMessagingData, data, length);
    gDataLength = length;
    /* This is done to wake up the reader process */
    gNewData = 1;
  }
  return;
}

int send()
{  
  //TODO: check get_connected 
  unsigned long seed = MY_PORT; 
  int i; 
  int len, result; 
  char send_buffer [8]; 
  ir_t mode = get_ir_mode(&repo);
  int dist;  
  lnp_logical_range(mode);
  if (mode == NEAR){
    ask_power((power_t)SDIST_POWER);
  }
  else {
    ask_power((power_t)LDIST_POWER);
  }

  while (1) { 
    /* sets i to a new random action */ 
    seed = seed * 1664525L * seed + 1013904223L;
    i = seed % 3; //TODO: send from repository 
    send_buffer [0] = i; 
    len = 1; 
    /* This sends a byte to the other process; keeps 
       trying until it succeeds */ 
    do { 
      dlcd_show (LCD_CIRCLE_0); 
      dlcd_show (LCD_CIRCLE_1); 
      result = lnp_addressing_write(send_buffer, len, DEST_ADDR, MY_PORT);
      /* waits between attempts */ 
      lcd_int(i); 
      msleep (100); 
      dlcd_hide (LCD_CIRCLE_0); 
      dlcd_hide (LCD_CIRCLE_1); 
    } while (result); 
    /* interval of time before sending another command */ 
    msleep (900); 
  }
  release_power();
  return 1;
}

int telemetry ()
{
  lnp_addressing_set_handler (MY_PORT, packet_handler);
  /*SEND*/ 
  send();
  return 1;
}
