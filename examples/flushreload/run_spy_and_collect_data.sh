#!/bin/bash

# Needed directories
VICTIM_DIR="${HOME}/Projects/flushreload_yarom/openssl/"
SPY_DIR="${HOME}/Projects/flushreload_yarom/spy/"
CLASSIF_DIR="${HOME}/Projects/malware_classifier/"
MONITOR_DIR="${HOME}/Projects/hpc_highres_monitor/"
OPENSSL_DIR="${HOME}/Projects/flushreload_yarom/openssl"

cd ${OPENSSL_DIR}

${SPY_DIR}/spy ${OPENSSL_DIR}/openssl.probe > /dev/null & 
${MONITOR_DIR}/monitor -a ${!} -c ${MONITOR_DIR}/conf/events_example.conf > ${CLASSIF_DIR}/data/spy_data.csv & 
${VICTIM_DIR}/openssl-root/apps/openssl dgst -sha1 -sign ${VICTIM_DIR}/test_data/private.pem ${VICTIM_DIR}/test_data/test.pdf > /dev/null & 
${MONITOR_DIR}/monitor -a ${!} -c ${MONITOR_DIR}/conf/events_example.conf > ${CLASSIF_DIR}/data/victim_data.csv; killall spy

