CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -I.
LDFLAGS = -lpthread
TARGET  = os_sim
SRC     = main.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build successful! Run with: ./$(TARGET)"

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
