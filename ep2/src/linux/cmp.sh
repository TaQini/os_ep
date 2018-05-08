#!/bin/bash
echo -e "\nsource code: \n"
cat ./a.c

echo -e "\nmytime: \n"
./mytime ./a.out

echo -e "\nsystem's time cmd: "
time ./a.out
