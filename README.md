# Why this tool?

You could use `perf-stat` (included in Linux) but its minimum resolution is 100 ms. This tool reached a resolution of 3 microseconds in some cases. More than 30000 times faster! I used this tool for some of my projects and even though it is extremely simple I could not find anything similar online (suggestions are welcome). So here it is.

# Build

* Download and untar [PAPI](http://icl.cs.utk.edu/papi/)
* Set the right path to PAPI's folder in the `Makefile` (variable `PAPI_DIR`)
* Build PAPI (`./configure; make` is enough)
* Run `make`

That's it!

# Usage

`./quickhpc OPTIONS`

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

A sane, standard format.

# Example

* Collect data during an openssl signature for 1000 iterations (each iteration is as fast as `papi_clockres` reports):

`openssl dgst -sha1 -sign test_data/private.pem test_data/test.pdf & quickhpc -a ${!} -c ~/papi/events.conf -n 10000 > data`

* Collect data about process 1234 for 100 iterations of 100 ms each:

`quickhpc -a 1234 -n 100 -i 100000 -c my_events.conf`

# Events file

A simple file with the names of the events you want to monitor (you can retrieve the complete list of supported events by running `papi_avail`).

## Example

    PAPI_TOT_INS
    PAPI_L3_TCM
    PAPI_L3_TCA

Is used to retrieve total CPU instructions, total L3 cache misses and total L3 cache accesses.
