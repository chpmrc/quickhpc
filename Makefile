CC=clang
PROGRAM=monitor
PAPI_DIR=./papi-5.4.1/src
LIBS_STATIC=${PAPI_DIR}/libpapi.a
EXTERNAL_OBJS=${PAPI_DIR}/testlib/do_loops.o \
	 ${PAPI_DIR}/testlib/test_utils.o \
	 ${PAPI_DIR}/testlib/dummy.o \

all:
	${CC} -I${PAPI_DIR} -I${PAPI_DIR} -I${PAPI_DIR}/testlib -g -Wall ${PROGRAM}.c \
		${EXTERNAL_OBJS} \
		${LIBS_STATIC} -o ${PROGRAM}

clean:
	rm ${PROGRAM}
