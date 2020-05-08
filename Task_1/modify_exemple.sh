#!/bin/bash

mkdir DIR_1
touch FILE_1
mkdir DIR_1/DIR_1_2
touch DIR_1/FILE_1_2

mkdir dir_2
touch dir_2/FILE_2
mkdir dir_2/DIR_2_1

echo "========== Here is the test files ! =========="
ls -R
echo "Press any key to continue"
read pause
clear


echo -e "========== -h ==========="
./modify.sh -h
echo "Press any key to continue"
read pause
clear


echo -e "========== FILE_1 --> file_1 (./modify.sh -l FILE_1) =========="
echo -e "\nBefore:\n"
ls
./modify.sh -l FILE_1
echo -e "\nAfter:\n"
ls
echo "Press any key to continue"
read pause
clear


echo -e "========== dir_2 --> DIR_2 (./modify.sh -u dir_2) =========="
echo -e "\nBefore:\n"
ls
./modify.sh -u dir_2
echo -e "\nAfter:\n"
ls
echo "Press any key to continue"
read pause
clear


echo -e "========== -r (./modify.sh -r -l DIR_1) =========="
echo -e "\nBefore:\n"
ls -R
./modify.sh -r -l DIR_1
echo -e "\nAfter:\n"
ls -R
echo "Press any key to continue"
read pause
clear


echo -e "========== <sed pattern> (./modify.sh 's/_/-/' file_1) =========="
echo -e "\nBefore:\n"
ls
./modify.sh 's/_/-/' file_1
echo -e "\nAfter:\n"
ls
echo "Press any key to continue"
read pause
clear


echo -e "========== Wrong name (./modify.sh -u wrongName) =========="
./modify.sh -u wrongName
echo "Press any key to continue"
read pause
clear


echo -e "========== Already in uppercase / lowercase =========="
./modify.sh -u DIR_2
./modify.sh -l file-1
echo "Press any key to continue"
read pause
clear






./modify.sh -u dir_1 > /dev/null
./modify.sh -u dir_2 > /dev/null
rm -r DIR_1
rm -r DIR_2
rm file-1
