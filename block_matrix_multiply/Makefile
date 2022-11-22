CC=gcc

LIBS = -lpthread -lstdc++

INCLUDES = -I./

SRC_FILES=$(foreach dir,$(PROJECT_DIRS),$(wildcard $(dir)/*.cpp))

.PHONY: block_mult
main:
	gcc -w -o block_mult $(SRC_FILES) main.cpp $(LIBS) $(INCLUDES)
