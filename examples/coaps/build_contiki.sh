#!/bin/bash

# Configuration, ToDo: Remove hard coding
DIRECTORY_CC2538DK="obj_cc2538dk"
DIRECTORY_OPENMOTE="obj_openmote-cc2538"
THREADS=2
DELETION=0

# Delete old object files
if [ $DELETION == 1 ]; then
	echo "Removing possible old object files..."
	if [ -d "$DIRECTORY_CC2538DK" ]; then
		echo "CC2538DK"
		rm -r $DIRECTORY_CC2538DK
	fi
	if [ -d "$DIRECTORY_OPENMOTE" ]; then
		echo "OpenMote"
		rm -r $DIRECTORY_OPENMOTE
	fi
fi

# Start new build
echo "Starting new build..."
echo "CC2538DK:"
make -j$THREADS TARGET=cc2538dk
if [[ $? != 0 ]]; then
	exit 255
else
	mv coaps.elf coaps-contiki-cc2538dk.elf
	mv coaps.bin coaps-contiki-cc2538dk.bin
fi
echo "OpenMote:"
make -j$THREADS TARGET=openmote-cc2538 BOARD_REVISION=REV_A1
if [[ $? != 0 ]]; then
	exit 255
else
	mv coaps.elf coaps-contiki-openmote.elf
	mv coaps.bin coaps-contiki-openmote.bin
fi

# Get size of new build
OUTPUT_NEW="$(arm-none-eabi-size coaps-contiki-cc2538dk.elf)"
RE_NEW="([0-9]{3,6})\s*([0-9]{3,6})\s*([0-9]{3,6})\s*([0-9]{3,6})"
[[ $OUTPUT_NEW =~ $RE_NEW ]]
	TEXT_NEW=${BASH_REMATCH[1]}
	BSS_NEW=${BASH_REMATCH[2]}
	DATA_NEW=${BASH_REMATCH[3]}
	DEC_NEW=${BASH_REMATCH[4]}
	
# Get size of old build
OUTPUT_OLD="$(cat build.old)"
RE_OLD="([0-9]{3,6}),([0-9]{3,6}),([0-9]{3,6}),([0-9]{3,6})"
[[ $OUTPUT_OLD =~ $RE_OLD ]]
	TEXT_OLD=${BASH_REMATCH[1]}
	BSS_OLD=${BASH_REMATCH[2]}
	DATA_OLD=${BASH_REMATCH[3]}
	DEC_OLD=${BASH_REMATCH[4]}

# Calculate differences
TEXT_DIF=`expr $TEXT_NEW - $TEXT_OLD`
BSS_DIF=`expr $BSS_NEW - $BSS_OLD`
DATA_DIF=`expr $DATA_NEW - $DATA_OLD`
DEC_DIF=`expr $DEC_NEW - $DEC_OLD`

# Print results
echo ""
echo "Old: TEXT($TEXT_OLD), BSS($BSS_OLD), DATA($DATA_OLD), DEC($DEC_OLD)"
echo "New: TEXT($TEXT_NEW), BSS($BSS_NEW), DATA($DATA_NEW), DEC($DEC_NEW)"
echo "Dif: TEXT($TEXT_DIF), BSS($BSS_DIF), DATA($DATA_DIF), DEC($DEC_DIF)"
echo "$TEXT_NEW,$BSS_NEW,$DATA_NEW,$DEC_NEW" > build.old