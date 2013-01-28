kill -9 `ps -ef | grep com.nexr.hms.Application | grep -v grep| awk '{print $2}'`

