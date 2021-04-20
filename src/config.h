#ifndef _CONFIG_H
#define _CONFIG_H

#include "papi_test.h"
#include "papi.h"

#define bool int
#define true 1
#define false 0
#define MAX_CMD_SIZE 2048
#define MAX_EVENTS 50
#define MAX_STR_LEN PAPI_MAX_STR_LEN
#define MAX_PATH_CHAR_SIZE 4096

typedef struct config_struct {
	bool attach; // True if the monitor has to attach to a given PID
	bool run; // True if the monitor has to start the command given as argument
	char cmd[MAX_CMD_SIZE]; // The command to be run if run is true
	int pid;
	int numEvents;
	char events[MAX_EVENTS][MAX_STR_LEN];
	int interval;
	int iterations;
} config;

/**
 * @brief Build the config from the given parameters.
 */
void setup_config(int, char **, config *);

void reset_config(config *);


#endif // #CONFIG_H
