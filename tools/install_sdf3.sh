#!/bin/bash

URL=http://www.es.ele.tue.nl/sdf3/download/files/releases/
URL=https://sstuijk.estue.nl/tools/sdf3/download/files/releases/
set -x


if [ "$#" -ne 1 ]; then
    echo -ne "Error\nUsage: $0 TARGET\nTARGET can be ./tools/sdf3/\n"
    exit 1
fi

#SDF3_VERSION=100927
SDF3_VERSION=140724

ORIGIN=$(pwd)
TARGET=$1

## CHECK TARGET FOLDER

if [ -d "${TARGET}" ]; then
    echo "Target '${TARGET}' exists and is a directory.";
else
    echo "Target '${TARGET}' does not exist. Please specify a valid directory.";
    exit 1;
fi

## DOWNLOAD SOURCE CODE
echo "77eadba886cf8c02b7c6831bd62cb675  ${TARGET}/sdf3-140724.zip" > "${TARGET}"/sdf3-140724.zip.md5

SDF3_ARCHIVE=sdf3-${SDF3_VERSION}.zip
    
if md5sum --status -c "${TARGET}/${SDF3_ARCHIVE}.md5"; then
    echo "Archive already downloaded.";
else
    rm -f "${TARGET}/${SDF3_ARCHIVE}"
    wget --tries=1 "${URL}${SDF3_ARCHIVE}" -O "${TARGET}/${SDF3_ARCHIVE}";
fi

## UNZIP AND COMPILE

SDF3_ROOT="${TARGET}/sdf3_${SDF3_VERSION}/"

SDF3ANALYSIS="${SDF3_ROOT}/sdf3/build/release/Linux/bin/sdf3analysis-csdf"

if find "${SDF3_ROOT}"/sdf3/build/release/*/bin/sdf3analysis-csdf; then
    SDF3ANALYSIS=$(find "${SDF3_ROOT}"/sdf3/build/release/*/bin/sdf3analysis-csdf)
    echo "EXECUTABLE FOUND SDF3ANALYSIS=${SDF3ANALYSIS} !"
fi




if "${SDF3ANALYSIS}" 2>&1 | grep -q version; then
    echo "Binary found, we assume it is done.";
else
    rm -rf "${SDF3_ROOT}";
    unzip -o "${TARGET}/${SDF3_ARCHIVE}" -d "${SDF3_ROOT}";
    if [[ "$OSTYPE" == "darwin"* ]]; then
	sed -i.bak "s/-fno-tree-fre//g" "${SDF3_ROOT}/sdf3/etc/Makefile.inc"
    fi;
    pushd "${SDF3_ROOT}/sdf3/" && make && popd || exit 1
fi;

cd "${ORIGIN}" || exit 1


if find "${SDF3_ROOT}"/sdf3/build/release/*/bin/sdf3analysis-csdf; then
    SDF3ANALYSIS=$(find "${SDF3_ROOT}"/sdf3/build/release/*/bin/sdf3analysis-csdf)
    echo "EXECUTABLE FOUND SDF3ANALYSIS=${SDF3ANALYSIS} !"
fi


if "${SDF3ANALYSIS}" 2>&1 | grep -q version; then
    echo "Script finished and binary is found.";
else
    echo "Error, sdf3analysis-csdf not found.";
    exit 1;
fi;


## UNZIP, PATCH AND COMPILE THE CUSTOM VERSION

SDF3_CUSTOM_ROOT="${TARGET}/sdf3_custom/"
export SDF3LOGDIR=./logs
SDF3ANALYSIS_CUSTOM="${SDF3_CUSTOM_ROOT}/sdf3/build/release/Linux/bin/sdf3analysis-csdf"

if find "${SDF3_CUSTOM_ROOT}"/sdf3/build/release/*/bin/sdf3analysis-csdf; then
    SDF3ANALYSIS_CUSTOM=$(find "${SDF3_CUSTOM_ROOT}"/sdf3/build/release/*/bin/sdf3analysis-csdf)
    echo "EXECUTABLE FOUND SDF3ANALYSIS_CUSTOM=${SDF3ANALYSIS_CUSTOM} !"
fi


if COARSE=false USE_SCC=false SDF3LOGDIR=  "${SDF3ANALYSIS_CUSTOM}"  2>&1 | grep -q version; then
    echo "Binary found, we assume it is done.";
else
    rm -rf "${SDF3_CUSTOM_ROOT}";
    unzip -o "${TARGET}/${SDF3_ARCHIVE}" -d "${SDF3_CUSTOM_ROOT}";
    pushd "${SDF3_CUSTOM_ROOT}/" && patch --verbose -p1 < ../../sdf3.patch && popd || exit 1
    if [[ "$OSTYPE" == "darwin"* ]]; then
	sed -i.bak "s/-fno-tree-fre//g" "${SDF3_CUSTOM_ROOT}/sdf3/etc/Makefile.inc"
    fi;
    pushd "${SDF3_CUSTOM_ROOT}/sdf3/" && make && popd || exit 1
fi;

cd "${ORIGIN}" || exit 1

if find "${SDF3_CUSTOM_ROOT}"/sdf3/build/release/*/bin/sdf3analysis-csdf; then
    SDF3ANALYSIS_CUSTOM=$(find "${SDF3_CUSTOM_ROOT}"/sdf3/build/release/*/bin/sdf3analysis-csdf)
    echo "EXECUTABLE FOUND SDF3ANALYSIS_CUSTOM=${SDF3ANALYSIS_CUSTOM} !"
fi

if COARSE=false USE_SCC=false SDF3LOGDIR= "${SDF3ANALYSIS_CUSTOM}" 2>&1 | grep -q version; then
    echo "Script finished and binary is found.";
else
    echo "Error at the end of the script.";
    exit 1;
fi;
