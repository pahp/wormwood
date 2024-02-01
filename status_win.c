#include "status_win.h"
#include <assert.h>
#include <errno.h>
#include <ncurses.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "common.h"
#include "console_win.h"

static bool g_initialized = false;

static WINDOW* g_window = NULL;

static pthread_mutex_t g_status_mutex = PTHREAD_MUTEX_INITIALIZER;

static char g_depth_hist[256];

static char* g_safety_string[] = {
    "<<<DISABLED>>>",
    "[ENABLED]",
    "",
};

static bool g_last_safety_state = false; // default safety is true.

static const int g_flash_rate = 1; // Seconds
static bool g_enable_flash = false;
static pthread_t g_flash_thread;
static pthread_cond_t g_flash_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t g_flash_cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_flash_mutex = PTHREAD_MUTEX_INITIALIZER;

static char *_draw_rod_depth(char rod_depth) {
	int idx = 0;
	int i;
	memset(g_depth_hist, '\0', 256);

	/* If depth is negative, draw to left of bracket. */
	if (g_depth_hist < 0) {
		i = rod_depth * -1;

		for (i; i > 0; i--) {
			g_depth_hist[idx] = '=';
			idx++;
		}
	}

	/* Draw brackets and whitespace starting at 0. */
	g_depth_hist[idx] = '[';
	g_depth_hist[idx + 17] = ']';
	idx++;
	for (i = 0; i < 16; i++) {
		g_depth_hist[idx + i] = ' ';
	}

	/* Depth is positive, draw to right of bracket. */
	for (i = 0; i < rod_depth; i++) {
		g_depth_hist[idx] = '=';
		idx++;
	}

	/* Draw final bracket at max depth. */

	return g_depth_hist;
}

static void _set_safety_state(int state) {
    /* Update safety text. */
    mvwprintw(g_window, 7, 1, "SAFETY PROTOCOLS: %14s", g_safety_string[state]);
    wrefresh(g_window);
    console_reset_cursor();
}

static void* _safety_flash_loop(void*) {
    struct timespec timeout;
    bool state = true;
    bool done = false;
    while(!done) {
        /* Get current time and add [g_flash_rate] sec. */
        timespec_get(&timeout, TIME_UTC);
        timeout.tv_sec += g_flash_rate;

        /* Wait until we've either been signalled or 1 second passes. */
        int res = 0;
        while(res != ETIMEDOUT && g_enable_flash) {
            res = pthread_cond_timedwait(&g_flash_cond, &g_flash_cond_mutex, &timeout);
            assert(res == 0 || res == ETIMEDOUT);
        }

        /* Aquire lock for enable flag. */
        assert(pthread_mutex_lock(&g_flash_mutex) == 0);

        /* Toggle safety state if we're suppose to still be running, otherwise quit. */
        if(g_enable_flash) {
            _set_safety_state(state ? 2 : 0);
            state = !state;
        }
        else {
            done = true;
        }

        assert(pthread_mutex_unlock(&g_flash_mutex) == 0);
    }

    return NULL;
}

void _start_safety_flash(void) {
    if(g_enable_flash) {
        return;
    }

    g_enable_flash = true;
    assert(pthread_create(&g_flash_thread, NULL, _safety_flash_loop, NULL) == 0);
}

void _end_safety_flash(void) {
    if(!g_enable_flash) {
        return;
    }

    /* Set flag to disable flash. */
    assert(pthread_mutex_lock(&g_flash_mutex) == 0);
    g_enable_flash = false;
    assert(pthread_mutex_unlock(&g_flash_mutex) == 0);

    /* Try to signal thread. */
    assert(pthread_cond_signal(&g_flash_cond) == 0);
    assert(pthread_join(g_flash_thread, NULL) == 0);
}

void status_init(void) {
    if(g_initialized) {
        return;
    }

    /* Create our window. */
    g_window = newwin(STATUS_WIN_H, STATUS_WIN_W, STATUS_WIN_Y, STATUS_WIN_X);
    assert(g_window != NULL);
    box(g_window, 0, 0);
}

void status_end(void) {
    if(!g_initialized) {
        return;
    }

    /* End safety flash. */
    _end_safety_flash();

    /* Destroy our window. */
    delwin(g_window);
}

void status_update(void) {
	char depth_hist[256];

    /* Aquire lock, we only want one thread updating this at a time. */
    assert(pthread_mutex_lock(&g_status_mutex) == 0);

	/* Generate timestamp string. */
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char timestring[64];
	sprintf(timestring, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	/* Print status message. */
	mvwprintw(g_window, 1, 1, "JERICHO NUCLEAR REACTOR STATUS PANEL             (%s)", timestring);
    mvwhline(g_window, 2, 1, 0, STATUS_WIN_W - 2);
	mvwprintw(g_window, 3, 1, "reactor temp: %8.2f              coolant_temp: %8.2f", reactor_temp, coolant_temp); 
	mvwprintw(g_window, 4, 1, "rod_depth: %2d --[ %s ]--  coolant flow rate: %5.2f", rod_depth, _draw_rod_depth(rod_depth), coolant_flow); 
	mvwprintw(g_window, 5, 1, "User: %-10s", users[usermode]);
    mvwhline(g_window, 6, 1, 0, STATUS_WIN_W - 2);

    /* If safety is disabled, the safety message should flash. */
    if(safety_enabled != g_last_safety_state) {
        if(!safety_enabled) {
            _set_safety_state(0);
            _start_safety_flash();
        }
        else {
            _end_safety_flash();
            _set_safety_state(1);
        }

        g_last_safety_state = safety_enabled;
    }

    /* Update window. */
    wrefresh(g_window);

    assert(pthread_mutex_unlock(&g_status_mutex) == 0);
}
