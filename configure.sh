#!/bin/sh

DEFAULTHOST=localhost
DEFAULTUSER=`whoami`
DEFAULTDBNAME="STOCKS"
DEFAULTTABLENAME="RECORDS"
DEFAULTTICKERTABLE="TICKERS"

read -p "Would you like to initialize a mysql or sqlite3 database [mysql]? " SQLTYPE

if [ -z "$SQLTYPE" ]
then

SQLOPTION=-m

elif [ $SQLTYPE = "mysql" ]
then

SQLOPTION=-m

elif [ $SQLTYPE = "sqlite3" ]
then

echo "sqlite3 is not yet supported"
SQLOPTION=-s
return

else

echo "not a supported database"
return

fi

#if the database chosen is mysql, then we need more info
read -p "host [$DEFAULTHOST]: " HOST

if [ -z "$HOST" ]
then

HOST=$DEFAULTHOST

fi

read -p "username [$DEFAULTUSER]: " USER

if [ -z "$USER" ]
then

USER=$DEFAULTUSER

fi

#turning off terminal input echo for password entry
stty -echo
read -p "password: " PASSWORD
stty echo
echo ""

read -p "database name [$DEFAULTDBNAME]: " DBNAME

if [ -z "$DBNAME" ]
then

DBNAME=$DEFAULTDBNAME

fi

read -p "table name [$DEFAULTTABLENAME]: " TABLENAME

if [ -z "$TABLENAME" ]
then

TABLENAME=$DEFAULTTABLENAME

fi

read -p "ticker table name [$DEFAULTTICKERTABLE]: " TICKERTABLE

if [ -z "$TICKERTABLE" ]
then

TICKERTABLE=$DEFAULTTICKERTABLE

fi

echo "configuration file created"
echo $SQLOPTION > ./.dbconf
echo $HOST >> ./.dbconf
echo $USER >> ./.dbconf
echo $PASSWORD >> ./.dbconf
echo $DBNAME >> ./.dbconf
echo $TABLENAME >> ./.dbconf
echo $TICKERTABLE >> ./.dbconf

read -p "would you also like to initialize a new database [y/n]? " MAKEDB

if [ -z "$MAKEDB" ]
then

return

elif [ $MAKEDB = "n" ]
then

return

elif [ $MAKEDB = "y" ]
then

./bin/initializedb $SQLTYPE ./.dbconf

if [ $SQLOPTION = "-m" ]
then

mysql --local-infile -h$HOST -u$USER -p$PASSWORD -e "CREATE TABLE $DBNAME.$TICKERTABLE (sym VARCHAR(8), name VARCHAR(128), exchange VARCHAR(8));LOAD DATA LOCAL INFILE './data/TICKERS' INTO TABLE $DBNAME.$TICKERTABLE COLUMNS TERMINATED BY '\t' LINES TERMINATED BY '\n';"

fi

else

return

fi

echo "finished"
