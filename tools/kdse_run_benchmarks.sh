#!/bin/bash
# This script runs the benchmarks used in the paper on K-periodic driven DSE.
# Tests for:
# - K-periodic/critical cycle driven DSE (KDSE)
# - Symbolic execution/storage dependency driven DSE (SDF3)
# - Symbolic execution/storage dependency drive DSE using SCC (SYMB_EXEC)
# - Periodic DSE (1-DSE)

MAX_TIME=2m

KITER="./Release/bin/kiter"
SDF3="./tools/sdf3/sdf3_custom/sdf3/build/release/Linux/bin/sdf3analysis-sdf"
CSDF3="./tools/sdf3/sdf3_custom/sdf3/build/release/Linux/bin/sdf3analysis-csdf"
SDF3_LIB="./tools/sdf3/sdf3_custom/sdf3/build/release/Linux/lib"

SDF3_BENCH_DIR="benchmarks/sdf3mem/"
KITER_BENCH_DIR="benchmarks/IB5CSDF/"


if [ "$#" -le 0 ]; then
    echo -ne "Error\nUsage: $0 LOGDIR [MAX_TIME]\n"
    exit 1
fi

if [ "$#" -ge 2 ]; then
    MAX_TIME=$2
fi


if [ ! -x ${SDF3} ]; then
    echo -ne "Error SDF3 is missing: ${SDF3} Not found.\n"
    exit 1
fi

if [ ! -x ${CSDF3} ]; then
    echo -ne "Error CSDF3 is missing: ${CSDF3} Not found.\n"
    exit 1
fi

if [ ! -x ${KITER} ]; then
    echo -ne "Error Kiter is missing: ${KITER} Not found.\n"
    exit 1
fi



LOGDIR=$1
if [ ! -d ${LOGDIR} ]; then
    echo -ne "Error directory ${LOGDIR} not found: create the directory and try again.\n"
    exit 1
fi

COMMAND_PREFIX="echo timeout --foreground  ${MAX_TIME}"


for graph in bipartite samplerate modem satellite fig8 h263decoder; do
    echo "Run ${graph} ThroughputBufferingDSE"
    ${COMMAND_PREFIX} ${KITER} -f ${SDF3_BENCH_DIR}${graph}.xml -a ThroughputBufferingDSE -p LOGDIR="${LOGDIR}/" # > /dev/null 2> /dev/null
    echo "Run ${graph} ThroughputBufferingDSE SYMB_EXEC_ORIGINAL"
    ${COMMAND_PREFIX} ${KITER} -f ${SDF3_BENCH_DIR}${graph}.xml -a ThroughputBufferingDSE -p SYMB_EXEC_ORIGINAL=t -p LOGDIR="${LOGDIR}/" # > /dev/null 2> /dev/null
    echo "Run ${graph} ThroughputBufferingDSE SYMB_EXEC_CORRECTED"
    ${COMMAND_PREFIX} ${KITER} -f ${SDF3_BENCH_DIR}${graph}.xml -a ThroughputBufferingDSE -p SYMB_EXEC_CORRECTED=t -p LOGDIR="${LOGDIR}/"  #> /dev/null 2> /dev/null
    echo "Run ${graph} PeriodicDSE"
    ${COMMAND_PREFIX} ${KITER} -f ${SDF3_BENCH_DIR}${graph}.xml -a PeriodicDSE -p LOGDIR="${LOGDIR}/"  #> /dev/null 2> /dev/null
    echo "Run ${graph}  SPeriodicDSE"
    ${COMMAND_PREFIX} ${KITER} -f ${SDF3_BENCH_DIR}${graph}.xml -a SPeriodicDSE -p LOGDIR="${LOGDIR}/"  #> /dev/null 2> /dev/null
    echo "Run ${graph}  SDF3"
    SDF3LOGDIR="${LOGDIR}/" ${COMMAND_PREFIX} ${SDF3} --graph ${SDF3_BENCH_DIR}${graph}.xml --algo buffersize  #> /dev/null 2> /dev/null
done

exit 0

for graph in BlackScholes Echo PDectect H264 JPEG2000; do
    echo "Run ${graph}"
    ${COMMAND_PREFIX} ${KITER} -f ${KITER_BENCH_DIR}${graph}.xml -a ThroughputBufferingDSE -p LOGDIR="${LOGDIR}/" > /dev/null 2> /dev/null
    ${COMMAND_PREFIX} ${KITER} -f ${KITER_BENCH_DIR}${graph}.xml -a ThroughputBufferingDSE -p SYMB_EXEC_ORIGINAL=t -p LOGDIR="${LOGDIR}/" > /dev/null 2> /dev/null
    ${COMMAND_PREFIX} ${KITER} -f ${KITER_BENCH_DIR}${graph}.xml -a ThroughputBufferingDSE -p SYMB_EXEC_CORRECTED=t LOGDIR="${LOGDIR}/" > /dev/null 2> /dev/null
    ${COMMAND_PREFIX} ${KITER} -f ${KITER_BENCH_DIR}${graph}.xml -a PeriodicDSE -p LOGDIR="${LOGDIR}/"  > /dev/null 2> /dev/null
    ${COMMAND_PREFIX} ${KITER} -f ${KITER_BENCH_DIR}${graph}.xml -a SPeriodicDSE -p LOGDIR="${LOGDIR}/"  > /dev/null 2> /dev/null
    LD_LIBRARY_PATH="${SDF3_LIB}" SDF3LOGDIR="${LOGDIR}" ${COMMAND_PREFIX} ${CSDF3} --graph ${KITER_BENCH_DIR}${graph}.xml --algo buffersize > /dev/null 2> /dev/null # NOTE manually specify LD_LIBRARY_PATH as a workaround for a bug in linker
done


./tools/dsereader.py --logdir ./"${LOGDIR}"/dse_logs/ bipartite samplerate modem satellite fig8 h263dec --opareto sdf3bench_pareto.png --odse sdf3bench_dse.png
./tools/dsereader.py --logdir ./"${LOGDIR}"/dse_logs/ Black-scholes echo MotionJPEG2000_CODEC_cad_V3 ViolaJones_Methode1 encoderH264 --opareto kiterbench_pareto.png --odse kiterbench_dse.png
