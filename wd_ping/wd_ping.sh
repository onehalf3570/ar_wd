#!/bin/sh

DEV=/dev/ttyACM0
PING_INTERVAL=60
READ_GATHER_TIMEOUT=1

#non-blocking read from $DEV
/bin/stty -F $DEV clocal raw -echo -icanon time 0 min 0

(
  exec 3<>$DEV
  #allow arduino to boot
  sleep 3
  while true; do
    echo -ne "ping\r" >&3
    res=$?
    if [ $res -ne 0 ]; then
      logger "watchdog pinging failed with exit code=$res"
      #exit 1
    else
      sleep $READ_GATHER_TIMEOUT
      #read ret<&3

      ret=''
      while :
      do
        key=$( dd bs=1 count=1 <&3 2> /dev/null; printf "." )
        key=${key%.}
        ret="${ret}${key}"
        if [ "$key" = '' ]; then
          break
        fi
      done
      logger "watchdog ping result: $ret"
    fi
    sleep `expr $PING_INTERVAL - $READ_GATHER_TIMEOUT`
  done
) &
