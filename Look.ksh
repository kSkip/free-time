#!/bin/ksh

SYM=$1
STARTDATE=$2
ENDDATE=$3

./bin/pull -da .dbconf $SYM $STARTDATE $ENDDATE > ./${SYM}_DATA

gnuplot -persist -e "data='./${SYM}_DATA';ticker='${SYM}';value='adj. close';" ./plotting/series.plg

rm ./${SYM}_DATA
