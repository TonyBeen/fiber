CC = g++
CPPFLAGS = -std=c++11 -Wall -g

PWD = $(shell pwd)

INCLUDE = -I$(PWD)
LIB_LIST = -lutils -llog -lpthread

SRC_LIST :			\
	thread.cpp		\
	fiber.cpp		\
	scheduler.cpp	\
	iomanager.cpp

OBJ_LIST :			\
	thread.o		\
	fiber.o			\
	scheduler.o		\
	iomanager.o

main : main.cc scheduler.cpp fiber.cpp thread.cpp
	$(CC) $^ -o $@ $(CPPFLAGS) $(INCLUDE) $(LIB_LIST)

test_thread : test_thread.cc thread.cpp
	$(CC) $^ -o $@ $(CPPFLAGS) $(INCLUDE) $(LIB_LIST)

test_fiber : test_fiber.cc fiber.cpp scheduler.cpp thread.cpp
	$(CC) $^ -o $@ $(CPPFLAGS) $(INCLUDE) $(LIB_LIST)

%.o : %.cpp
	$(CC) -c $^ -o $@ $(CPPFLAGS) $(INCLUDE)

.PHONY :
	main test_thread test_fiber debug

debug :
	@echo $(PWD)

clean :
	rm -rf $(OBJ_LIST) main test_thread test_fiber