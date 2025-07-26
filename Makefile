CFLAGS = -Wall
OBJFLAGS = -Wall -c

SRC = nsh.c
OBJ = nsh.o
TARGET = nsh

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

$(OBJ): $(SRC)
	$(CC) $(OBJFLAGS) $(SRC)

clean:
	rm $(TARGET)
