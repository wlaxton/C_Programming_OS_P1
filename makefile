CC = cc
CFLAGS = -g
OBJ1 = main.o stack.o
TARGET1 = P1

.SUFIXES:.c.o

all: $(TARGET1) clean

$(TARGET1): $(OBJ1)
	$(CC) -o $(CFLAGS) -o $(TARGET1) $(OBJ1) 
.c.o:
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm *.o
