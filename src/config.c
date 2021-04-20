#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "config.h"

bool attach; // True if the monitor has to attach to a given PID
	bool run; // True if the monitor has to start the command given as argument
	char cmd[MAX_CMD_SIZE]; // The command to be run if run is true
	int pid;

void reset_config(config *cfg) {
	cfg->attach = false;
	cfg->run = false; // TODO not used
	cfg->pid = -128;
	cfg->numEvents = 0;
	cfg->interval = 0; // As fast as possible
	cfg->iterations = -1; // Infinite
}

void readEventNames(char *filePath, config *cfg) {
	int i = 0;
	size_t len = 0;
	FILE *fp;
	char *tempStr;
	fp = fopen(filePath, "r");
	if (fp == NULL) {
		perror("Configuration file error:");
		exit(1);
	}
	while (getline(&tempStr, &len, fp) != -1){
		if (tempStr[0] != '#') {
			strncpy(cfg->events[i], tempStr, strlen(tempStr) - 1); // Remove final LF
			cfg->events[i][strlen(tempStr) - 1] = '\0';
			i++;
		}
	}
	free(tempStr);
	cfg->numEvents = i;
	// Test
	fprintf(stderr, "Monitoring %d events:\n", cfg->numEvents);
	for (i = 0; i < cfg->numEvents; i++) {
		fprintf(stderr, "%s\n", cfg->events[i]);
	}
	fprintf(stderr, "\n");
	fclose(fp);
}

void setup_config(int argc, char **argv, config *cfg) {
	int i = 1;
	char configFilePath[MAX_PATH_CHAR_SIZE];
	int interval = 0;
	char iterations[MAX_STR_LEN] = "infinite";
	reset_config(cfg);
	for (; i < argc; i++) {
		// Attached PID
		if (!strcmp(argv[i], "-a")) {
			cfg->attach = true;
			cfg->pid = atoi(argv[i+1]);
		} else
		// Configuration file
		if (!strcmp(argv[i], "-c")) {
			strcpy(configFilePath, argv[i+1]);
			fprintf(stderr, "Using config file: %s\n", configFilePath);
			readEventNames(configFilePath, cfg);
		} else
		if (!strcmp(argv[i], "-i")) {
			interval = atoi(argv[i+1]);
			fprintf(stderr, "Using interval: %d usec\n", interval);
			cfg->interval = interval;
		} else
		if (!strcmp(argv[i], "-n")) {
			if (atoi(argv[i]) > -1)
				strcpy(iterations, argv[i+1]);
			fprintf(stderr, "Monitoring for %s iterations\n", iterations);
			cfg->iterations = atoi(iterations);
		} else
		// Run new process
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
