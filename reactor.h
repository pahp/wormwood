#pragma once
#include <stdbool.h>
#include <pthread.h>

#define MAX_SAFE_DEPTH 16
#define MAX_FLOW_RATE 100.0

typedef enum {
    usermode_none = 0,
    usermode_oper,
    usermode_super
} usermode_t;

usermode_t reactor_get_usermode(void);
void reactor_set_usermode(usermode_t mode);

bool reactor_get_safety(void);
void reactor_set_safety(bool enabled);

char reactor_get_rod_depth(void);
void reactor_set_rod_depth(char depth);

float reactor_get_coolant_flow(void);
void reactor_set_coolant_flow(float flow);

float reactor_get_temp(void);
float reactor_get_coolant_temp(void);

void reactor_update(void);

void reactor_set_realtime_enabled(bool enabled);
bool reactor_is_realtime(void);

void reactor_start_realtime_update(void);
void reactor_end_realtime_update(void);
