#pragma once
#include <stdbool.h>

#define MAX_FLOW_RATE 100.0

#define REACTOR_UNSAFE_DEPTH 17

#define REACTOR_WARNING_TEMP 3000
#define REACTOR_WARNING_TEMP_2 4000
#define REACTOR_EXPLODE_TEMP 5000

typedef enum {
	usermode_none = 0,
	usermode_oper,
	usermode_super,
	usermode_count
} usermode_t;

/*
 * [usermode_none]		= "NA"
 * [usermode_oper]		= "oper"
 * [usermode_super]		= "super"
 */
extern const char* g_usermode_str[usermode_count];

usermode_t reactor_get_usermode(void);
void reactor_set_usermode(usermode_t mode);

bool reactor_get_safety(void);
void reactor_set_safety(bool enabled);

bool reactor_get_safety_active(void);

unsigned char reactor_get_rod_depth(void);
void reactor_set_rod_depth(unsigned char depth);

float reactor_get_coolant_flow(void);
void reactor_set_coolant_flow(float flow);

float reactor_get_temp(void);
float reactor_get_coolant_temp(void);


/**************************************************************
 *          Don't worry about anything below here :)          *
 **************************************************************/

typedef struct {
	unsigned int temp_error : 1;
	unsigned int rupture_error :1;
} warnings_t;

typedef struct {
	usermode_t usermode;
	warnings_t warns;
	float temp;
	float coolant_flow;
	float coolant_temp;
	unsigned char rod_depth;
	bool safety_enabled;
	bool safety_active;
} reactor_state_t;

reactor_state_t reactor_get_state(void);

void reactor_update(void);

typedef enum {
	reactor_mode_norealtime,
	reactor_mode_realtime,
	reactor_mode_count
} reactor_mode_t;

void reactor_init(reactor_mode_t mode);
void reactor_end(void);
reactor_mode_t reactor_get_mode(void);
