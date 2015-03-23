#include "papi_test.h"
#include <sys/ptrace.h>
#include <limits.h>

#ifdef _AIX
#define _LINUX_SOURCE_COMPAT
#endif

#if defined(__FreeBSD__)
# define PTRACE_ATTACH PT_ATTACH
# define PTRACE_CONT PT_CONTINUE
#endif

int
main( int argc, char *argv[] )
{
        int retval, num_tests = 1;
        int EventSet1 = PAPI_NULL;
        long long **values;
        //long long elapsed_us, elapsed_cyc, elapsed_virt_us, elapsed_virt_cyc;
        char event_name[PAPI_MAX_STR_LEN];;
        const PAPI_hw_info_t *hw_info;
        const PAPI_component_info_t *cmpinfo;
        pid_t pid;

        pid = atoi(argv[1]);

        retval = PAPI_library_init( PAPI_VER_CURRENT );
        if ( retval != PAPI_VER_CURRENT )
                test_fail_exit( __FILE__, __LINE__, "PAPI_library_init", retval );

        if ( ( cmpinfo = PAPI_get_component_info( 0 ) ) == NULL )
                test_fail_exit( __FILE__, __LINE__, "PAPI_get_component_info", 0 );

        if ( cmpinfo->attach == 0 )
                test_skip( __FILE__, __LINE__, "Platform does not support attaching",
                                   0 );

        hw_info = PAPI_get_hardware_info(  );
        if ( hw_info == NULL )
                test_fail_exit( __FILE__, __LINE__, "PAPI_get_hardware_info", 0 );

        /* add PAPI_TOT_CYC and one of the events in PAPI_FP_INS, PAPI_FP_OPS or
           PAPI_TOT_INS, depending on the availability of the event on the
           platform */
        retval = PAPI_create_eventset(&EventSet1);
        if ( retval != PAPI_OK )
                test_fail_exit( __FILE__, __LINE__, "PAPI_create_eventset", retval );

        /* Here we are testing that this does not cause a fail */

        retval = PAPI_assign_eventset_component( EventSet1, 0 );
        if ( retval != PAPI_OK )
                test_fail( __FILE__, __LINE__, "PAPI_assign_eventset_component",
                retval );

        retval = PAPI_attach( EventSet1, ( unsigned long ) pid );
        if ( retval != PAPI_OK )
                test_fail_exit( __FILE__, __LINE__, "PAPI_attach", retval );

        sprintf(event_name,"PAPI_LD_INS");

        retval = PAPI_add_event(EventSet1, PAPI_LD_INS);
        if ( retval != PAPI_OK )
                test_fail_exit( __FILE__, __LINE__, "PAPI_add_event", retval );

        retval = PAPI_add_event(EventSet1, PAPI_SR_INS);
        if ( retval != PAPI_OK )
                test_fail_exit( __FILE__, __LINE__, "PAPI_add_event", retval );

        retval = PAPI_add_event(EventSet1, PAPI_BR_INS);
        if ( retval != PAPI_OK )

                if ( retval != PAPI_OK )
                test_fail_exit( __FILE__, __LINE__, "PAPI_add_event", retval );

        values = allocate_test_space( 1, 3);

        printf("After %d\n",retval);

        retval = PAPI_start( EventSet1 );
        if ( retval != PAPI_OK )
                test_fail_exit( __FILE__, __LINE__, "PAPI_start", retval );

        printf("Continuing\n");

        //for( i = 0; i < 2000000000; i++) { j += i;}
        sleep ( 10 );
        retval = PAPI_stop( EventSet1, values[0] );
        if ( retval != PAPI_OK )
          test_fail_exit( __FILE__, __LINE__, "PAPI_stop", retval );

        retval = PAPI_cleanup_eventset(EventSet1);
        if (retval != PAPI_OK)
          test_fail_exit( __FILE__, __LINE__, "PAPI_cleanup_eventset", retval );

        retval = PAPI_destroy_eventset(&EventSet1);
        if (retval != PAPI_OK)
          test_fail_exit( __FILE__, __LINE__, "PAPI_destroy_eventset", retval );

        printf( "Test case: 3rd party attach start, stop.\n" );

        printf
                ( "-------------------------------------------------------------------------\n" );

        printf( "Test type    : \t           1\n" );
        printf( TAB1, "PAPI_LD_INS : \t", ( values[0] )[0] );
        printf( TAB1, "PAPI_SR_INS : \t", ( values[0] )[1] );
        printf( TAB1, "PAPI_BR_INS : \t", ( values[0] )[2] );
        printf
                ( "-------------------------------------------------------------------------\n" );

        printf( "Verification: none\n" );

        test_pass( __FILE__, values, num_tests );
        exit( 1 );
}