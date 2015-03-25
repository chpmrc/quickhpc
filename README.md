# Build

* Untar and build PAPI (./configure; make is enough)
* Run make

That's it!

# Usage

`./monitor OPTIONS`

where `OPTIONS` can be:

* `-c` Path to the file that contains the list of events (one per line) to monitor (make sure they are available by running `papi_avail`)
* `-a` PID of the process to attach to
* `-n` Number of iterations, each iteration lasts the duration of an interval
* `-i` Interval for each measurement (i.e. time between then the hpc is start and stopped) in the order of microseconds

The output of the program is organized as CSV data complete with headers and (for the time being) some debug prints at the top. An example follows:

	Using config file: /home/user/events.conf
	Monitoring the following events:
	PAPI_TOT_INS
	PAPI_L3_TCM
	PAPI_L3_TCA
	Monitoring for 10000 iterations
	Attaching to PID: 5403
	PAPI_TOT_INS,PAPI_L3_TCM,PAPI_L3_TCA
	142948,9,307
	128214,9,266
	78173,69,363
	124587,32,168
	116464,38,209
	126420,28,177
	126188,27,133
	118189,41,170
	127132,25,179
	130775,28,144
	...

Excellent format to be used for plotting, statistical analysis etc. in Octave/Matlab.

# Example

* Collect data during an openssl signature for 1000 iterations (each iteration is as fast as `papi_clockres` reports):

`./openssl-1.0.1e/apps/openssl dgst -sha1 -sign test_data/private.pem test_data/test.pdf & monitor -a ${!} -c ~/Projects/flushreload_agent/papi/events.conf -n 10000 > data`

* Collect data about process 1234 for 100 iterations of 100 ms each:

`monitor -a 1234 -n 100 -i 100000 -c my_events.conf`

# Events file

A simple file with the names of the events you want to monitor (you can retrieve the complete list of supported events by running `papi_avail`).

## Example

    PAPI_TOT_INS
    PAPI_L3_TCM
    PAPI_L3_TCA

Is used to retrieve total CPU instructions, total L3 cache misses and total L3 cache accesses.