CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = 
LDLIBS = 

INCLUDE = -Iinclude/
SRC_DIR = ./src
OBJ_DIR = ./obj

SRCS = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJS = $(patsubst %.o, $(OBJ_DIR)/%.o, $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

TARGET = tetris

## Detect OS
ifeq '$(findstring ;,$(PATH))' ';'
    detected_OS := Windows
else
    detected_OS := $(shell uname 2>/dev/null || echo Unknown)
    detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
    detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
    detected_OS := $(patsubst MINGW%,MSYS,$(detected_OS))
endif

ifeq ($(detected_OS),Windows)
    TARGET := $(TARGET).exe
endif

ifeq ($(detected_OS),Darwin)
    LDFLAGS += -lncurses
endif

all: $(TARGET)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -MD

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)