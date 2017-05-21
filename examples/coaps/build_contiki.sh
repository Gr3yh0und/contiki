#!/bin/bash

# Configuration, ToDo: Remove hard coding
DIRECTORY="obj_cc2538dk"
THREADS=2

# Delete old object files
if [ -d "$DIRECTORY" ]; then
	echo "Removing old object files..."
	rm -r obj_cc2538dk
fi

# Start new build
echo "Starting new build..."
make -j$THREADS
if [[ $? != 0 ]]; then
	exit 255
fi

# Get size of new build
OUTPUT_NEW="$(arm-none-eabi-size coaps.elf)"
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
echo "Old: TEXT($TEXT_OLD), BSS($BSS_OLD), DATA($DATA_OLD), DEC($DEC_OLD)"
echo "New: TEXT($TEXT_NEW), BSS($BSS_NEW), DATA($DATA_NEW), DEC($DEC_NEW)"
echo "Dif: TEXT($TEXT_DIF), BSS($BSS_DIF), DATA($DATA_DIF), DEC($DEC_DIF)"
echo "$TEXT_NEW,$BSS_NEW,$DATA_NEW,$DEC_NEW" > build.old