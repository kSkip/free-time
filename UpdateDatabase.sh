#!/bin/sh

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

echo "Updating " $SYM

RECENT=`mysql -B -N -h$HOST -u$USER -p$PASSWORD -e "SELECT MAX(date) FROM $DBNAME.$TABLE WHERE sym='$SYM';"`

DATE=`date +%Y-%m-%d -d "$RECENT + 1 day"`

./bin/yfetch -h $DATE $SYM | awk -v var="$SYM" '{print var","$0}' >> ./.data

done

./bin/populatedb -f ./.data .dbconf

rm ./.data
