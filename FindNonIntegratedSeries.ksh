#!/bin/ksh

STARTDATE=2014-01-01
ENDDATE=

DBCONF=`cat ./.dbconf | tr '\n' ' '`

SQL=`echo $DBCONF | cut -f1 -d' '`
HOST=`echo $DBCONF | cut -f2 -d' '`
USER=`echo $DBCONF | cut -f3 -d' '`
PASSWORD=`echo $DBCONF | cut -f4 -d' '`
DBNAME=`echo $DBCONF | cut -f5 -d' '`
TABLE=`echo $DBCONF | cut -f6 -d' '`
TICKERTABLE=`echo $DBCONF | cut -f7 -d' '`

SYMS=`mysql -B -N -h$HOST -u$USER -p$PASSWORD -e "SELECT sym FROM $DBNAME.$TICKERTABLE;"`

for SYM in $SYMS
do

./bin/pull -a .dbconf $SYM $STARTDATE $ENDDATE > temp

#conduct dickey-fuller tests
DF_PASS_SIMPLE=$(cat temp | ./bin/dftest -ps)

DF_PASS_CONST=$(cat temp | ./bin/dftest -pc)

DF_PASS_CONST_TREND=$(cat temp | ./bin/dftest -pct)

#conduct kpss tests
KPSS_PASS_SIMPLE=$(cat temp | ./bin/kpsstest -p $1)

KPSS_PASS_TREND=$(cat temp | ./bin/kpsstest -pt $1)

#determine if the series is non-integrated
if [[ "$DF_PASS_SIMPLE" == "TRUE" && "$KPSS_PASS_SIMPLE" == "FALSE" && "$KPSS_PASS_TREND" == "FALSE" ]]
then

echo "${SYM},I(0),no const,no trend"

elif [[ "$DF_PASS_CONST" == "TRUE" && "$KPSS_PASS_SIMPLE" == "FALSE" && "$KPSS_PASS_TREND" == "FALSE" ]]
then

echo "${SYM},I(0),const,no trend"

elif [[ "$DF_PASS_CONST_TREND" == "TRUE" && "$KPSS_PASS_SIMPLE" == "FALSE" && "$KPSS_PASS_TREND" == "FALSE" ]]
then

echo "${SYM},I(0),const,trend"

fi

rm temp

done

