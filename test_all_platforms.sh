#!/bin/bash

#
# ARGV: full source path if build outside source tree
#
# ENV VARIABLE: 
#     CDASH_TARGET={ Nightly | Experimental | Continuous }
#

# set -x

case "$CDASH_TARGET" in
    "Nightly")
	MAKE_TARGET=$CDASH_TARGET
	MAKE_OPT=-j1
	;;
    "Experimental")
	MAKE_TARGET=$CDASH_TARGET
	MAKE_OPT=-j1
	;;
    "Continuous")
	MAKE_TARGET=$CDASH_TARGET
	MAKE_OPT=-j1
	;;
    *)
	MAKE_TARGET=all
	MAKE_OPT=-j12
	# Set -e to stop on first error
	set -e
	;;
esac

# ##################################################

ROOT_PATH=`pwd`

if [ "x$1" != "x" ] ; then
    SRCROOT=$1
else
    SRCROOT=`pwd`
fi

# Get the platforms
PLATFORMS=`find ${SRCROOT}/platform/ -type d -printf "%P "`

# ##################################################

START_TIME=`date`

run_build()
{
    COMMENT="$1" 
    PPATH=$2
    SROOT=$3

    if [ -d $PPATH ]
    then
	rm -rf $PPATH
    fi
    mkdir -p $PPATH
    cd $PPATH

    shift 3

    echo "===============================================================" >&2
    echo "== $COMMENT " >&2
    echo "== $PPATH :: $* " >&2
    echo "===============================================================" >&2
    cmake ${SROOT} $*
    make $MAKE_TARGET $MAKE_OPT
}

for p in $PLATFORMS
do
	# Test for scripts folder which is not a platform
	if [ "$p" = "scripts" ]
	then
		echo "Skipping scripts folder"
		continue
	fi
	
	# Test for native folder which is not a simple platform
	if [ "$p" = "native" ]
	then
		echo "Skipping native folder"
		continue
	fi

	echo "Platform is $p"
	TEST_FLAGS=
	if [ -f ${SRCROOT}/platform/$p/testflags.sh ]
	then
		source ${SRCROOT}/platform/$p/testflags.sh
	fi
	
	#
	# Normal Test
	#
	BASEDIR=`pwd`
	PLATFORM_PATH="build.$p"
	run_build "[$p] Using no CMake flag" \
	    ${PLATFORM_PATH} ${SRCROOT}      \
	    -DPLATFORM=$p
	
	#
	# Special Tests 
	#
	if [ "x$TEST_FLAGS" != "x" ]; then
		for i in ${!TEST_FLAGS[*]}; do
		        cd ${BASEDIR}
			run_build "[$p] Using CMake flags ${TEST_FLAGS[$i]}" \
			    ${PLATFORM_PATH}-F${i} ${SRCROOT}                \
			    -DPLATFORM=$p ${TEST_FLAGS[$i]}
		done
	fi
	
	cd ..
	echo "[$p] Compilation OK"
done

# ##################################################

STOP_TIME=`date`
echo "All platforms compile OK : start/stop $START_TIME --- $STOP_TIME"

# ##################################################

