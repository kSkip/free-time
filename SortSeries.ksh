#!/bin/ksh

WORK_DIR=./WORK
RESULTS_DIR=./RESULTS

INTEGRATED=I_1.dat
NONINTEGRATED=I_0.dat
UNKNOWN=I_NA.dat

KPSS_REF=15

mkdir -p $WORK_DIR
mkdir -p $RESULTS_DIR

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

echo "Analyzing ${SYM}..."

./bin/pull -a .dbconf $SYM $STARTDATE $ENDDATE > $WORK_DIR/${SYM}_DATA

cat $WORK_DIR/${SYM}_DATA | ./bin/fur -p $KPSS_REF > $WORK_DIR/${SYM}_TYPE

TYPE=$(awk -F',' 'FNR == 1 {print $1}' $WORK_DIR/${SYM}_TYPE)

echo "${SYM} appears to be "$(cat $WORK_DIR/${SYM}_TYPE)
echo "Sorting ${SYM}"

if [[ $TYPE == 'I(0)' ]]
then

echo ${SYM}','$(cat $WORK_DIR/${SYM}_TYPE) >> $RESULTS_DIR/$NONINTEGRATED

elif [[ $TYPE == 'I(1)' ]]
then

echo ${SYM}','$(cat $WORK_DIR/${SYM}_TYPE) >> $RESULTS_DIR/$INTEGRATED

else

echo ${SYM}','$(cat $WORK_DIR/${SYM}_TYPE) >> $RESULTS_DIR/$UNKNOWN

fi

rm $WORK_DIR/${SYM}_DATA
rm $WORK_DIR/${SYM}_TYPE

done
