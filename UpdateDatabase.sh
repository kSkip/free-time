#!/bin/sh

DBCONF=`cat ./.dbconf | tr '\n' ' '`

SQL=`echo $DBCONF | cut -f1 -d' '`
HOST=`echo $DBCONF | cut -f2 -d' '`
USER=`echo $DBCONF | cut -f3 -d' '`
PASSWORD=`echo $DBCONF | cut -f4 -d' '`
DBNAME=`echo $DBCONF | cut -f5 -d' '`
TABLE=`echo $DBCONF | cut -f6 -d' '`
TICKERTABLE=`echo $DBCONF | cut -f7 -d' '`

SYMS=`mysql -B -N -h$HOST -u$USER -p$PASSWORD -e "SELECT symbol FROM $DBNAME.$TICKERTABLE WHERE marketcap != 'n/a';"`

for SYM in $SYMS
do

echo "Updating " $SYM

#Determine the date at which to pull data from

RECENT=`mysql -B -N -h$HOST -u$USER -p$PASSWORD -e "SELECT MAX(date) FROM $DBNAME.$TABLE WHERE sym='$SYM';"`

if [ "$RECENT" != "NULL" ]; then

DATE=`date +%Y-%m-%d -d "$RECENT + 1 day"`

else

DATE="1970-01-01"

fi;

echo "From " $DATE

#fetch the data the parsed data

./bin/yfetch -h $DATE $SYM | awk -v var="$SYM" '{print var","$0}' >> ./.data

done

#send to database

./bin/populatedb -f ./.data .dbconf

#delete temporary file
#rm ./.data
