#include "repos.h"
#include "time.h"

int mtsleep(int i)
{
  int j, k;
  for (j=0; j < i; j++){
    for (k=0; k< i; k++){
    }
  }
  return 1;
}


void init_repos(repos_t *repo)
{
  repo->bumped = NO;
  repo->act_mode = SEEK_LIGHT;
  repo->is_connected = FALSE;
  return;
}

/* access functions */

/* action mode */
void set_act_mode(repos_t *repo, mode_t mode) 
{
  repo->act_mode = mode;
  return;
}

mode_t get_act_mode(repos_t *repo)
{
  return repo->act_mode;
}

/* battery level */
void set_bat_level(repos_t *repo, bat_level_t val) 
{
  repo->battery_level = val;
  return;
}

bat_level_t get_bat_level(repos_t *repo)
{
  return repo->battery_level;
}

/* is bumped */
void set_bumped(repos_t *repo, bumped_t mode) 
{
  repo->bumped = mode;
  return;
}

bumped_t get_bumped(repos_t *repo)
{
  return repo->bumped;
}

/* is connected */
void set_connected(repos_t *repo, bool_t b) 
{
  repo->is_connected = b;
  return;
}

bool_t get_connected(repos_t *repo)
{
  return repo->is_connected;
}

/* is infrared mode */
void set_ir_mode(repos_t *repo, ir_t val) 
{
  repo->ir_mode = val;
  return;
}

ir_t get_ir_mode(repos_t *repo) 
{
  return repo->ir_mode;
}

bool_t get_ir_connected(repos_t *repo)
{
  return repo->is_connected;
}

void set_timestamp (repos_t * repo, timestamp_t t)
{
  repo->ts = t;
  return;
}

timestamp_t get_timestamp (repos_t * repo)
{
  return repo->ts;
}

/* the is_fresh bit */
void set_is_fresh(repos_t *repo, bool_t b) 
{
  repo->is_fresh = b;
  return;
}

bool_t get_is_fresh(repos_t *repo)
{
  return repo->is_fresh;
}


