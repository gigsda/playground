 export OPTS="-Xdebug -Xrunjdwp:transport=dt_socket,server=y,suspend=n,address=5005"
./bin/server-start.sh -c conf/server.properties
tail -f nohup.out

