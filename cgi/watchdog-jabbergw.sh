#!/bin/sh

while [ 1 ]; do
  cd /var/www/jabbergw.com/cgi/
  /var/www/jabbergw.com/cgi/bot-jabbergw >>/var/www/jabbergw.com/shell.log 2>&1
  sleep 1
done
