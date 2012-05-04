#!/bin/sh

DEV=/dev/ttyACM0
PING_INTERVAL=60
READ_GATHER_TIMEOUT=1

wait_for_dev() {
  while [ ! -c $DEV ]; do
    logger "watchdog: $DEV does not exist or not a character device"
    sleep 3;
  done
}

#non-blocking read from $DEV
init_dev() {
  /bin/stty -F $DEV 115200 clocal raw -echo -icanon time 0 min 0
}

(
  wait_for_dev;
  init_dev;
  exec 3<>$DEV

  #allow arduino to boot
  sleep 3
  while true; do
    echo -ne "ping\r" >&3
    res=$?
    if [ $res -ne 0 ]; then
      logger "watchdog pinging failed with exit code=$res, trying to reopen $DEV"

      exec 3<&-
      wait_for_dev;
      init_dev;

      exec 3<>$DEV

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
  exec 3<&-
) &
