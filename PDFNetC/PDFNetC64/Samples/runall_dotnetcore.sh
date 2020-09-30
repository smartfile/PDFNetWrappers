#!/bin/sh

for SAMPLE in *
do
if [ -d $SAMPLE ]
then
  if [ -e $SAMPLE/CS ]
  then
      cd $SAMPLE/CS
      echo "$SAMPLE running"
      dotnet run
      cd ../..
      echo "$SAMPLE finished."
  fi
fi
done
exit 0