#!/bin/bash

#Note: This script must be placed in the very same directory as makefile and mysh binary

mkdir test-sources

echo 'date' > test-sources/test1.sh
echo '/bin/echo X' > test-sources/test2.sh
echo 'echo X; date; echo Z' > test-sources/test3.sh
echo 'echo X;date;echo Z' > test-sources/test4.sh
echo 'echo x;' > test-sources/test5.sh
#Bash doesn't behave as described in assignment: unxepected token & wrong exit code, should be 254
echo 'date;;' > test-sources/test6.sh
echo 'exit' > test-sources/test7.sh
#Bash on - writes out OLDPWD, so there are 2 extra lines in its output
echo 'pwd
cd
pwd
cd -
pwd
cd /tmp
pwd
cd -
pwd' > test-sources/test8.sh
echo 'echo X # comment
   # another comment' > test-sources/test9.sh

echo 'grep XXX123 /etc/passwd
exit' > test-sources/test10.sh

echo 'grep XXX123 /etc/passwd' > test-sources/test11.sh
#Bash doesn't behave as described in assignment: unxepected token
echo 'ls; ;' > test-sources/test12.sh

for file in test-sources/test*.sh
do
    echo $file
    diff <(./test_mysh $file; echo $?;) <(bash $file; echo $?;)
    echo
    echo
done

echo test-sources/test13
    diff <(./mysh -c 'echo XXX; date') <(bash -c 'echo XXX; date')
echo
echo

echo test-sources/test14
    diff <(./mysh -c 'date; echo X # comment') <(bash -c 'date; echo X # comment')
echo
echo

echo 'echo X
date
;;
echo Y' > test-sources/input.txt

#Bash doesn't behave as described in assignment: unxepected token
echo test-sources/test15
diff <(./mysh test-sources/input.txt) <(bash test-sources/input.txt)
echo
echo

echo test-sources/test16
diff <(./mysh -c "grep XXX123 /etc/passwd"; echo $?;) <(bash -c "grep XXX123 /etc/passwd"; echo $?;)
echo
echo

#Bash doesn't behave as described in assignment: unxepected token
echo test-sources/test17
diff <(./mysh -c ";;"; echo $?) <( bash -c ";;"; echo 254;)
echo
echo

#Bash doesn't behave as described in assignment, prints command not found
echo test-sources/test18
diff <(./mysh -c 'xxx123'; echo $?) <(bash -c 'xxx123'; echo $?;)
echo
echo

rm -rf test-sources