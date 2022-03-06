CC = g++
CPPFLAGS = -std=c++11 -Wall -g

PWD = $(shell pwd)

INCLUDE = -I$(PWD)
LIB_LIST = -lutils -llog -lpthread

test_thread : test_thread.cc thread.cpp
	$(CC) $^ -o $@ $(CPPFLAGS) $(INCLUDE) $(LIB_LIST)

test_fiber : test_fiber.cc fiber.cpp scheduler.cpp thread.cpp
	$(CC) $^ -o $@ $(CPPFLAGS) $(INCLUDE) $(LIB_LIST)

debug :
	@echo $(PWD)