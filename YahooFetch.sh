#!/bin/bash

#Encode start date

URL_DATE1=`echo "$2" | awk -F'-' '{ printf("a=%s&b=%s&c=%s",$2-1,$3,$1) }'`

#Encode end date

URL_DATE2=`echo "$3" | awk -F'-' '{ printf("d=%s&e=%s&f=%s",$2-1,$3,$1) }'`

#Encode the url attachment

URL_ENCODING=`echo "$1 $URL_DATE1 $URL_DATE2" | awk -F' ' '{printf("s=%s&%s&%s&g=d",$1,$2,$3)}'`

#Make the http request

DATA=`curl -Ls "http://ichart.finance.yahoo.com/table.csv?$URL_ENCODING" | awk '{if (NR!=1) {print}}'`

#check that the csv data was actually acquired

if [ "`echo "$DATA" | grep -c "404 Not Found"`" == "0" ]
then

#print data to stdout

echo "$DATA"

fi

