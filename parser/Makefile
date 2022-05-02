UNAME := $(shell uname)
CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS= -L.

INC = include/
SRC = src/
BIN = bin/
PARSER_SRC_FILES = $(wildcard src/SVG*.c)
PARSER_OBJ_FILES = $(patsubst src/SVG%.c,bin/SVG%.o,$(PARSER_SRC_FILES))

ifeq ($(UNAME), Linux)
	XML_PATH = /usr/include/libxml2
endif
ifeq ($(UNAME), Darwin)
	XML_PATH = /System/Volumes/Data/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml2
endif

#parser: $(BIN)libsvgparser.so

parser: libsvgparser.so

#$(BIN)libsvgparser.so: $(PARSER_OBJ_FILES) $(BIN)LinkedListAPI.o
#	gcc -shared -o $(BIN)libsvgparser.so $(PARSER_OBJ_FILES) $(BIN)LinkedListAPI.o -lxml2 -lm

libsvgparser.so: $(PARSER_OBJ_FILES) $(BIN)LinkedListAPI.o
	gcc -shared $(PARSER_OBJ_FILES) $(BIN)LinkedListAPI.o -lxml2 -lm -o ../libsvgparser.so


#Compiles all files named SVG*.c in src/ into object files, places all corresponding SVG*.o files in bin/
$(BIN)SVG%.o: $(SRC)SVG%.c $(INC)LinkedListAPI.h $(INC)SVG*.h
	gcc $(CFLAGS) -I$(XML_PATH) -I$(INC) -c -fpic $< -o $@

$(BIN)liblist.so: $(BIN)LinkedListAPI.o
	$(CC) -shared -o $(BIN)liblist.so $(BIN)LinkedListAPI.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

clean:
	rm -rf $(BIN)StructListDemo $(BIN)xmlExample $(BIN)*.o $(BIN)*.so

#This is the target for the in-class XML example
xmlExample: $(SRC)libXmlExample.c
	$(CC) $(CFLAGS) -I$(XML_PATH) $(SRC)libXmlExample.c -lxml2 -o $(BIN)xmlExample

#These are sample targets for the list demo code included in the class examples.  They will not be used
#for A1, but they can help you figure out who to set up a target for your own test main

StructListDemo: $(BIN)StructListDemo.o $(BIN)liblist.so
	$(CC) $(CFLAGS) $(LDFLAGS) -L$(BIN) -o $(BIN)StructListDemo $(BIN)StructListDemo.o  -llist
	
$(BIN)StructListDemo.o: $(SRC)StructListDemo.c
	$(CC) $(CFLAGS) -I$(INC) -c $(SRC)StructListDemo.c -o $(BIN)StructListDemo.o

###################################################################################################

mainTester: mainTester.c parser $(BIN)libsvgparser.so
	$(CC) $(CFLAGS) -I$(INC) -I$(XML_PATH) -Lbin -lsvgparser mainTester.c -o $(BIN)mainTester