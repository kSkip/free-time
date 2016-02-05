#!/bin/ksh

MAX_DESIRED_GAIN=0.05
MIN_DESIRED_GAIN=0.01
MAX_PRICE=30
MIN_PRICE=5

CORRELATION_LAGS=30

PROJECTION=21
TRIALS=100

LIST=./RESULTS_NASDAQ/I_0.dat
RESULTS_DIR=./RESULTS
WORK_DIR=./WORK

mkdir -p $WORK_DIR

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

#SYM=$(awk -v var="$I" -F',' 'FNR == var {print $1}' $LIST)
#CONST=$(awk -v var="$I" -F',' 'FNR == var {print $3}' $LIST)
#TREND=$(awk -v var="$I" -F',' 'FNR == var {print $4}' $LIST)
CONST='const'
TREND='no trend'

while true
do

PROC=$(jobs | wc -l);
if [[ $PROC -lt 4 ]]
then
break
else
sleep 0.1
fi

done

(
	echo "${SYM}: Pulling data..."

	./bin/pull -a .dbconf $SYM $STARTDATE $ENDDATE > $WORK_DIR/${SYM}_DATA 

	echo "${SYM}: Determining model..."

	D=0

	FIT_OPT=-x
	INFER_OPT="-x $D"
	if [[ "$CONST" == "const" ]]
	then
		FIT_OPT=${FIT_OPT}c
	fi
	if [[ "$TREND" == "trend" ]]
	then
		FIT_OPT="${FIT_OPT} -t"
		INFER_OPT="${INFER_OPT} -t"
	fi
	FIT_OPT=${FIT_OPT}po

	echo $INFER_OPT

	INFERRED_LAGS=${SYM}_INFERRED_LAGS

	cat $WORK_DIR/${SYM}_DATA | ./bin/infer -p $INFER_OPT $CORRELATION_LAGS > $WORK_DIR/$INFERRED_LAGS

	echo "${SYM}: Setting model parameters..."

	P=$(awk 'FNR == 1 {print $1}' $WORK_DIR/$INFERRED_LAGS)
	Q=$(awk 'FNR == 1 {print $2}' $WORK_DIR/$INFERRED_LAGS)

	MODEL=${SYM}_ARIMA.model

	echo "${SYM}: Fitting the Model..."

	cat $WORK_DIR/${SYM}_DATA | ./bin/arimafit $FIT_OPT $WORK_DIR/$MODEL $P $Q $D > $WORK_DIR/${SYM}_VALUE_RES

	cat $WORK_DIR/${SYM}_DATA | ./bin/acf -p $INFER_OPT $CORRELATION_LAGS | tee $WORK_DIR/${SYM}_ACF | ./bin/stepfit -p 2 > ./$WORK_DIR/${SYM}_ACF_FIT
	cat $WORK_DIR/${SYM}_DATA | ./bin/pacf -p $INFER_OPT $CORRELATION_LAGS | tee $WORK_DIR/${SYM}_PACF | ./bin/stepfit -p 2 > ./$WORK_DIR/${SYM}_PACF_FIT

	echo "${SYM}: Filing report..."

		gnuplot -e "outputfile='$WORK_DIR/${SYM}_ACF.svg'; function='$WORK_DIR/${SYM}_ACF'; fit='$WORK_DIR/${SYM}_ACF_FIT'" plotting/correlation.plg

		gnuplot -e "outputfile='$WORK_DIR/${SYM}_PACF.svg'; function='$WORK_DIR/${SYM}_PACF'; fit='$WORK_DIR/${SYM}_PACF_FIT'" plotting/correlation.plg

		gnuplot -e "outputfile='$WORK_DIR/${SYM}_RESIDUALS.svg'; datafile='$WORK_DIR/${SYM}_VALUE_RES';" plotting/histogram.plg

		gnuplot -e "outputfile='$WORK_DIR/${SYM}_DATA.svg'; data='$WORK_DIR/${SYM}_DATA';" plotting/series_notime.plg

	mkdir -p $RESULTS_DIR/${SYM}
	mv $WORK_DIR/${SYM}_DATA.svg $RESULTS_DIR/${SYM}/${SYM}_DATA.svg
	mv $WORK_DIR/${SYM}_ACF.svg $RESULTS_DIR/${SYM}/${SYM}_ACF.svg
	mv $WORK_DIR/${SYM}_PACF.svg $RESULTS_DIR/${SYM}/${SYM}_PACF.svg
	mv $WORK_DIR/${SYM}_RESIDUALS.svg $RESULTS_DIR/${SYM}/${SYM}_RESIDUALS.svg
	mv $WORK_DIR/$MODEL $RESULTS_DIR/${SYM}/$MODEL
	mv $WORK_DIR/${SYM}_VALUE_RES $RESULTS_DIR/${SYM}/${SYM}_VALUE_RES
	mv $WORK_DIR/${SYM}_ACF $RESULTS_DIR/${SYM}/${SYM}_ACF
	mv $WORK_DIR/${SYM}_ACF_FIT $RESULTS_DIR/${SYM}/${SYM}_ACF_FIT
	mv $WORK_DIR/${SYM}_PACF $RESULTS_DIR/${SYM}/${SYM}_PACF
	mv $WORK_DIR/${SYM}_PACF_FIT $RESULTS_DIR/${SYM}/${SYM}_PACF_FIT

	rm $WORK_DIR/$INFERRED_LAGS
	rm $WORK_DIR/${SYM}_DATA

	echo "${SYM}: :JOB DONE:"
) &

done
