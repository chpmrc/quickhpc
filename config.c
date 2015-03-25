#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

bool attach; // True if the monitor has to attach to a given PID
	bool run; // True if the monitor has to start the command given as argument
	char cmd[MAX_CMD_SIZE]; // The command to be run if run is true
	int pid;

void reset_config(config *cfg) {
	cfg->attach = false;
	cfg->run = false;
	cfg->pid = -128;
	cfg->numEvents = 0;
}

void readEventNames(char *filePath, config *cfg) {
	int i = 0;
	FILE *fp;
	fp = fopen(filePath, "r");
	while (fscanf(fp, "%s", cfg->events[i]) != EOF){
		i++;
	}
	cfg->numEvents = i;
	// Test
	printf("Monitoring the following events:\n");
	for (i = 0; i < cfg->numEvents; i++) {
		printf("%s\n", cfg->events[i]);
	}
}

void setup_config(int argc, char **argv, config *cfg) {
	int i = 1;
	char configFilePath[MAX_PATH_CHAR_SIZE];
	reset_config(cfg);
	for (; i < argc; i++) {
		if (!strcmp(argv[i], "-a")) {
			cfg->attach = true;
			cfg->pid = atoi(argv[i+1]);
		} else
		if (!strcmp(argv[i], "-c")) {
			strcpy(configFilePath, argv[i+1]);
			printf("Using config file: %s\n", configFilePath);
			readEventNames(configFilePath, cfg);
		} else
		if (!strcmp(argv[i], "-r")) {
			cfg->run = true;
		} else {
			if (i == 0){
				// The first argument must be an option
				fprintf(stderr, "The first argument must be an option! (E.g. -a, -r).\n");
				exit(1);
			}
		}
	}

}