#!/bin/sh

for SAMPLE in *
do
	if [ -d $SAMPLE ]
	then
		if [ -e $SAMPLE/JAVA/RunTest.sh ]
		then
			cd $SAMPLE/JAVA
			echo "$SAMPLE running"
			sh RunTest.sh
			cd ../..
			echo "$SAMPLE finished. Press enter to continue..."
			read -p "$*" a
		fi
	fi
done
