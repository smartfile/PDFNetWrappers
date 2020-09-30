#!/bin/sh

for SAMPLE in *
do
    if [ -d $SAMPLE ]
	then
		if [ -e $SAMPLE/CPP/Makefile ]
		then
			cd $SAMPLE/CPP
			echo "$SAMPLE running"
            make
			./$SAMPLE
			cd ../..
			echo "$SAMPLE finished. Press enter to continue..."
			read -p "$*" a
		fi
	fi
done
exit 0
