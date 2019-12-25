#include <sys/ptrace.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "config.h"
#include <time.h>
#include <errno.h>
#include <unistd.h>

#ifdef _AIX
#define _LINUX_SOURCE_COMPAT
#endif

#if defined(__FreeBSD__)
# define PTRACE_ATTACH PT_ATTACH
# define PTRACE_CONT PT_CONTINUE
#endif

inline unsigned long gettime() {
  volatile unsigned long tl;
  asm __volatile__("lfence\nrdtsc" : "=a" (tl): : "%edx");
  return tl;
}

struct timespec timer_start();
long timer_end(struct timespec start_time);
void initPapi();
void initEventSet(int *);
void addEvent(int, char *eventName);
void monitor(int, long long **values);
void buildCSVLine(char *line, long long *values, int numItems);
void cleanup(int *eventSet);
void usage(char *name);

int main( int argc, char *argv[] )
{
#ifdef _DEBUG
        unsigned long t1, t2; // Used to measure timing of various calls
        struct timespec vartime;
        long time_elapsed_nanos;
        long samples = 0;
        double time_sum = 0;
#endif
        int eventSet = PAPI_NULL;
        int numEventSets = 1; // One set should be enough
        int idx;
        int retval;
        long long **values;
        //long long elapsed_us, elapsed_cyc, elapsed_virt_us, elapsed_virt_cyc;
        char monitorLine[PAPI_MAX_STR_LEN];
        pid_t pid = -1;
        config cfg;
        bool processAlive = true;

        if (argc < 3) {
            usage(argv[0]);
        }

        initPapi();

        /* Load configuration from arguments */
        setup_config(argc, argv, &cfg);
       
        if (cfg.run && cfg.attach) {
            fprintf(stderr, "Cannot attach and run! Choose one.\n");
            exit(1);
        }

        if (cfg.run) {
            pid = 0; // TODO run process and get the PID
        }

        if (cfg.attach) {
            pid = cfg.pid;
            fprintf(stderr, "Attaching to PID: %d\n", pid);
        }

        if (pid < 0) {
            fprintf(stderr, "Please specify a PID to attach to with -a" /* or -r to run a process*/);
        }

        initEventSet(&eventSet);

        for (idx = 0; idx < cfg.numEvents; idx++) {
            addEvent(eventSet, cfg.events[idx]); // TODO
        }

        retval = PAPI_attach( eventSet, ( unsigned long ) pid );
        if ( retval != PAPI_OK )
                test_fail( __FILE__, __LINE__, "PAPI_attach", retval );

        values = allocate_test_space(numEventSets, cfg.numEvents);

        /* Print CSV headers */
        fprintf(stderr, "\n");
        for (idx = 0; idx < cfg.numEvents; idx++) {
            fprintf(stderr, "%s", cfg.events[idx]);
            if (idx < cfg.numEvents - 1) {
                fprintf(stderr, ",");
            }
        }
        retval = PAPI_start( eventSet );
        if ( retval != PAPI_OK )
            test_fail( __FILE__, __LINE__, "PAPI_start", retval );
        /* Start monitoring and print values */
        fprintf(stderr, "\n");
        for (idx = 0; processAlive && (idx < cfg.iterations || cfg.iterations == -1); idx++) {
#ifdef _DEBUG
            t1 = gettime();
            vartime = timer_start();  // begin a timer called 'vartime'
#endif
            monitor(eventSet, values);
            // retval = PAPI_read( eventSet, values[0] );
            // if ( retval != PAPI_OK )
            //     test_fail( __FILE__, __LINE__, "PAPI_read", retval );
            // retval = PAPI_reset( eventSet );
            // if ( retval != PAPI_OK )
            //     test_fail( __FILE__, __LINE__, "PAPI_reset", retval );
#ifdef _DEBUG
            time_elapsed_nanos = timer_end(vartime);
            t2 = gettime();
            fprintf(stderr, "Time for one sample (cycles, RDTSC): %lu\n", t2 - t1);
            fprintf(stderr, "Time for one sample (nanoseconds, clock_gettime): %lu\n", time_elapsed_nanos);
            samples++;
            time_sum += time_elapsed_nanos;
#endif      
            if (cfg.interval > 0)
                usleep(cfg.interval);

            // fprintf(stderr, "Time to monitor: %lu\n", t2 - t1);
            buildCSVLine(monitorLine, values[0], cfg.numEvents);
            fprintf(stdout, "%s\n", monitorLine);
            fflush(stdout);
            if (kill(pid, 0) < 0) {
                /* Process is not active anymore */
                if (errno == ESRCH) {
                    processAlive = false;
                }
            }
        }

        retval = PAPI_stop( eventSet, values[0]);
        if ( retval != PAPI_OK )
            test_fail( __FILE__, __LINE__, "PAPI_stop", retval );
        
        cleanup(&eventSet);

#ifdef _DEBUG
        fprintf(stderr, "Mean sampling time: %f\n", time_sum / samples);
#endif

        exit(0);
}

void initPapi(){
    int retval;
    const PAPI_hw_info_t *hw_info;
    const PAPI_component_info_t *cmpinfo;
    retval = PAPI_library_init( PAPI_VER_CURRENT );
    if ( retval != PAPI_VER_CURRENT )
            test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

    if ( ( cmpinfo = PAPI_get_component_info( 0 ) ) == NULL )
            test_fail( __FILE__, __LINE__, "PAPI_get_component_info", 0 );

    if ( cmpinfo->attach == 0 )
            test_skip( __FILE__, __LINE__, "Platform does not support attaching",
                               0 );

    hw_info = PAPI_get_hardware_info(  );
    if ( hw_info == NULL )
            test_fail( __FILE__, __LINE__, "PAPI_get_hardware_info", 0 );
}

void initEventSet(int *eventSet) {
    int retval;
    /* add PAPI_TOT_CYC and one of the events in PAPI_FP_INS, PAPI_FP_OPS or
       PAPI_TOT_INS, depending on the availability of the event on the
       platform */
    retval = PAPI_create_eventset(eventSet);
    if ( retval != PAPI_OK )
            test_fail( __FILE__, __LINE__, "PAPI_create_eventset", retval );

    /* Here we are testing that this does not cause a fail */

    retval = PAPI_assign_eventset_component( *eventSet, 0 );
    if ( retval != PAPI_OK )
            test_fail( __FILE__, __LINE__, "PAPI_assign_eventset_component",
            retval );

    /* Enable multiplexing to be able to probe more counters (less precision) */
    // retval = PAPI_set_multiplex(*eventSet);
    // if ( retval != PAPI_OK )
    //         test_fail( __FILE__, __LINE__, "PAPI_set_multiplex",
    //         retval );
}

void addEvent(int eventSet, char *eventName) {
    int eventCode = 0;
    int retval;
    char msg[PAPI_MAX_STR_LEN];
    PAPI_event_name_to_code(eventName, &eventCode );
    retval = PAPI_add_event(eventSet, eventCode);
    if ( retval != PAPI_OK ) {
        sprintf(msg, "PAPI_add_event (%s)", eventName);
        test_fail( __FILE__, __LINE__, msg, retval );
    }
}

void monitor(int eventSet, long long **values) {
    int retval;

    retval = PAPI_read( eventSet, values[0] );
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_read", retval );
	
    retval = PAPI_reset( eventSet );
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_reset", retval );
}

void buildCSVLine(char *line, long long *values, int numItems) {
    int i = 0;
    if (values == NULL || numItems <= 0) {
        fprintf(stderr, "Error: invalid CSV line! (%s, line %d)\n", __FILE__, __LINE__);
        exit(1);
    }
    for (; i < numItems; i++) {
        if (i == 0) {
            sprintf(line, "%lld", values[i]);
        } else {
            sprintf(line, "%s,%lld", line, values[i]);            
        }
    }
}

void cleanup(int *eventSet) {
    int retval;
    retval = PAPI_cleanup_eventset(*eventSet);
    if (retval != PAPI_OK)
      test_fail( __FILE__, __LINE__, "PAPI_cleanup_eventset", retval );

    retval = PAPI_destroy_eventset(eventSet);
    if (retval != PAPI_OK)
      test_fail( __FILE__, __LINE__, "PAPI_destroy_eventset", retval );
}

void usage(char *name) {
    fprintf(stderr, "Usage: %s -a PID -c EVENTS_FILE [-i INTERVAL (usecs)] [-n SAMPLES]\n", name);
    fprintf(stderr, "\t(If no interval is specified the sampling is performed as quickly as possible)\n");
    fprintf(stderr, "\t(If no number of samples is specified the execution continues until either %s is killed or the attached process terminates)\n", name);
    exit(1);
}

// call this function to start a nanosecond-resolution timer
struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    long diffInNanos = end_time.tv_nsec - start_time.tv_nsec;
    return diffInNanos;
}
