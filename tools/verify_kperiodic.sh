#!/bin/bash

#set -x

KITER_ROOT_DIR="./"

SDF3_BINARY_ROOT="${KITER_ROOT_DIR}/tools/sdf3/sdf3_140724/sdf3/build/release/Linux/bin/"
SDF3ANALYSIS_CSDF="timeout --foreground 180 ${SDF3_BINARY_ROOT}/sdf3analysis-csdf"
SDF3ANALYSIS_SDF="timeout --foreground 180 ${SDF3_BINARY_ROOT}/sdf3analysis-sdf"
KITER="timeout --foreground 180 ${KITER_ROOT_DIR}/Release/bin/kiter"


LOG_DIR=${KITER_ROOT_DIR}/logs/kperiodic_verification/
SDF3_LOG=${LOG_DIR}/sdf3
KITER_LOG=${LOG_DIR}/kiter


# make directories to store logs for verification
mkdir -p "${LOG_DIR}" "${SDF3_LOG}" "${KITER_LOG}"

echo "Arguments: $*"
BENCHMARK_ARRAY=( "$@" )

# compare logs
TOTAL_TESTS=0
PASSED=0

# assumes that the benchmarks are all SDFs (rather than CSDFs) - use $SDF3ANALYSIS_CSDF for CSDF benchmarks
for BENCHMARK in "${BENCHMARK_ARRAY[@]}"; do
    GRAPH=${BENCHMARK##*/}
    GRAPH_NAME=${GRAPH%.xml}
    echo "running Throughput on ${GRAPH_NAME}...";
    # generate respective pareto logs
    echo "${KITER}" -f "${BENCHMARK}" -a KPeriodicThroughput;
    ${KITER} -f "${BENCHMARK}" -a KPeriodicThroughput | grep "Maximum throughput" | sed -E "s/.*is\s+(.*)\s*/\1/"  >  "${KITER_LOG}/${GRAPH_NAME}".txt
    
    if head -n 3 "${BENCHMARK}" | grep -q csdf ; then
	echo "${SDF3ANALYSIS_CSDF}" --graph "${BENCHMARK}" --algo throughput
	${SDF3ANALYSIS_CSDF} --graph "${BENCHMARK}" --algo throughput > tmp.txt;
    else 
	echo "${SDF3ANALYSIS_SDF}" --graph "${BENCHMARK}" --algo throughput
	${SDF3ANALYSIS_SDF} --graph "${BENCHMARK}" --algo throughput >  tmp.txt;
    fi
    
    grep "thr" tmp.txt | sed -E "s/.*=\s+(.*)\s*/\1/"  >  "${SDF3_LOG}/${GRAPH_NAME}".txt
    rm tmp.txt
	
    # convert SDF3 DSE output to CSV (from XML):
    echo "Throughput complete"

    
    SDF3_RES="${SDF3_LOG}/${GRAPH_NAME}.txt"
    KITER_RES="${KITER_LOG}/${GRAPH_NAME}.txt"

    
    if [ -f "${SDF3_RES}" ] && [ -f "${KITER_RES}" ]; then
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        printf "Checking %s" "${GRAPH_NAME}..."
        if diff -q "$KITER_RES"  "$SDF3_RES"; then
            PASSED=$((PASSED + 1))
            echo "results match!"
        else
            echo "      Run 'diff $KITER_RES $SDF3_RES' to see differences"
        fi
    else
	echo "***** One of them failed."
    fi
done

if [ $PASSED -eq $TOTAL_TESTS ]; then
    echo "All tests passed; Throughput produce identical results!"
else
    echo "$PASSED/$TOTAL_TESTS matching Throughput results"
fi