#pragma once
#include "reactor.h"

#define STATUS_WIN_X 0
#define STATUS_WIN_Y 0
#define STATUS_WIN_W 80
#define STATUS_WIN_H 9

typedef enum {
    status_safety_invalid,
    status_safety_enable,
    status_safety_disable,
    status_safety_blank,
} status_safety_t;

void status_init(void);

void status_end(void);

void status_update(const reactor_state_t* state, unsigned int tick);
