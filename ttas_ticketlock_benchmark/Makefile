CC=g++

LIBS = -lpthread -lstdc++

INCLUDES = -I./

SRC_FILES=$(foreach dir,$(PROJECT_DIRS),$(wildcard $(dir)/*.cpp))

.PHONY: LockTest
main:
	gcc -std=c++11 -w -o LockTest $(SRC_FILES) tester.cpp $(LIBS) $(INCLUDES)
