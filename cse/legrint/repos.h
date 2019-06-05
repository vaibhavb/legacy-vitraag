#ifndef _REPO_H_
#define _REPO_H_

typedef struct repository repos_t;
typedef enum {SEEK_LIGHT, GO_WALL, ST_LINE, GO_HOME} mode_t;
typedef enum NR {NEAR, FAR} ir_t;
typedef enum {Near, Far} dist_t;
typedef enum {FALSE, TRUE} bool_t;
typedef enum {NO=1, YES=0} bumped_t;
typedef int  bat_level_t;
typedef unsigned long timestamp_t;

/* TODO: Add processes */ 
struct repository {
  /* sensor data */
  timestamp_t ts;
  bat_level_t battery_level;
  bumped_t bumped;
  bool_t is_connected;
  ir_t ir_mode;
  dist_t dist;
 
  bool_t is_fresh; 
  char* base_data;  
  mode_t act_mode;
};

void init_repos(repos_t *);
void set_act_mode(repos_t *, mode_t);
mode_t get_act_mode(repos_t *);
ir_t get_ir_mode (repos_t *);

void set_bumped(repos_t *, bumped_t);
bumped_t get_bumped(repos_t *);
void set_timestamp(repos_t *, timestamp_t );
void set_is_fresh(repos_t *, bool_t);

int mtsleep (int );

extern repos_t repo;

#endif
