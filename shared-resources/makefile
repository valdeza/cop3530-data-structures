#!bin/bash

#Define empty, non-defined variable.
_NUL:=

CC=g++
CFLAGS=-Wall -g -std=c++11
SOURCES="pa1-src-calc.cpp"
CLOG=compiler_output.log
EXECUTABLE=pa1.out
EXEOPTS:=< mass-let.in.txt

all:
# ifneq ($(EXECUTABLE),$(_NUL))
# 	# Expand \$EXECUTABLE
# 	EXECUTABLE=-o "$(EXECUTABLE)"
# 	echo crash1
# endif
# 	# Run full command
# 	echo crash2
# 	$(CC) $(CFLAGS) $(SOURCES) $(EXECUTABLE) >$(CLOG) 2>$(CLOG)
# 	echo crash3

ifeq ($(EXECUTABLE),$(_NUL))
	$(CC) $(CFLAGS) $(SOURCES) >$(CLOG) 2>$(CLOG)
else
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE) >$(CLOG) 2>$(CLOG)
endif

coffee:
	valgrind -v --log-file=valgrind.out.txt ./$(EXECUTABLE) $(EXEOPTS)

time:
	time ./$(EXECUTABLE) $(EXEOPTS)
