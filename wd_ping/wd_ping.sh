#!/bin/sh

DEV=/dev/ttyACM0

/bin/stty -F $DEV clocal raw -echo

(
  while true; do
    echo "ping" > $DEV
    res=$?
    if [ $res -ne 0 ]; then
      logger "watchdog pinging failed with exit code=$res"
    else
      logger "watchdog ping result: `cat $DEV`"
    fi
    sleep 60
  done
) &
