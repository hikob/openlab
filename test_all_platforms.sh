#!/bin/bash

# Set -e to stop on first error
set -e
# Set -x to show all intermediate commands
#set -x

ROOT_PATH=`pwd`

if [ "x$1" != "x" ] ; then
    SRCROOT=$1
else
    SRCROOT=`pwd`
fi

# Get the platforms
PLATFORMS=`find ${SRCROOT}/platform/ -type d -printf "%P "`

for p in $PLATFORMS
do

	# Test for scripts folder which is not a platform
	if [ "$p" = "scripts" ]
	then
		echo "Skipping scripts folder"
		continue
	fi
	
	PLATFORM_PATH="build.$p"
	if [ -d $PLATFORM_PATH ]
	then
		rm -rf $PLATFORM_PATH
	fi
	mkdir $PLATFORM_PATH
	cd $PLATFORM_PATH
	cmake ${SRCROOT} -DPLATFORM=$p
	make -j 12 all
	cd ..
	echo "Platform $p compiles OK"
done

echo "All platforms compile OK"

