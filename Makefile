CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -O3
LIBS = -lcurl -lgumbo
TARGET = scraper

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET) index.json
