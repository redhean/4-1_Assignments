#!/bin/bash

# 1) compile your program with makefile created in Exercise 1
make

# 2) make answer sheet as answer.txt
echo "33 11" > answer.txt

# 3) run your program and save output as output.txt 
echo "22 11" | ./p2.out > output.txt

# 4) compare answer.txt and output.txt with diff command as result.txt
diff answer.txt output.txt > result.txt

# 5) print the contents of "result.txt"
cat result.txt

# 6) clean object files and executable file
make clean
