#!/bin/bash

if [ $# -lt 1 ];
then
    echo "USAGE: $0 -c server.properties -p policy.info"
    exit 1
fi

base_dir=$(dirname $0)/..

if [ -z "$JAVA_HOME" ]; then
  JAVA="java"
else
  JAVA="$JAVA_HOME/bin/java"
fi

for file in $base_dir/lib/*.jar;
do
  CLASSPATH=$CLASSPATH:$file
done


OPTS="-server -Dlog4j.configuration=file:$base_dir/conf/log4j.properties"
echo $CLASSPATH

$JAVA $OPTS -cp $CLASSPATH com.nexr.hms.Application $@ 
