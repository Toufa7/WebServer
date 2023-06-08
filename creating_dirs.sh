#!/bin/bash

cd Test; mkdir aa; cd aa; touch {1..2}.txt; mkdir aaa;cd aaa;touch {5..7}.txt;cd ..; cd .. ; pwd
