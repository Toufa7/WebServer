#!/bin/bash

for i in `seq 20`
do
    randomstring=$(cat /dev/urandom | LC_CTYPE=C tr -dc '0-9' | fold -w 10 | head -n 1)
    curl --head -i --request GET http://127.0.0.1:8080/www/
done

