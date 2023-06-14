#!/bin/bash

for i in `seq 20`
do
    randomstring=$(cat /dev/urandom | LC_CTYPE=C tr -dc '0-9' | fold -w 10 | head -n 1)
    curl -i --request GET http://10.11.3.1:8888/www/
done

