CC = gcc
CFLAGS = -Wall -Werror -pthread
OBJFILES = thread_manager.c CommandNode.c
TARGET = thread_manager

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -D_REENTRANT $(CFLAGS) -g -o $(TARGET) $(OBJFILES)


clean:
	rm -f $(TARGET) *~
