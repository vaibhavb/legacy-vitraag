/** RESOURCES: motors
               ir
	       repository
	       cpu
	       power
    REQUESTs:  ask, get, release
*/

typedef unsigned long power_t;
void ask_motors();
void get_motors();
void release_motors();

void ask_repository();
void get_repository();
void release_repository();

void ask_cpu();
void get_cpu();
void release_cpu();

void ask_power(power_t);
void get_power(power_t);
void release_power();

void ask_ir();
void get_ir();
void release_ir();

