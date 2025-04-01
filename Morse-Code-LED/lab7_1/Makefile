CC = clang
CFLAGS = -Wall
LIBS = -lpigpio -lrt -lpthread

TARGET = send

SRC = send.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(TARGET)

