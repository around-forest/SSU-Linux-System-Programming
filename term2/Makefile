CC = gcc
TARGET = ssu_score
SRCS = blank.c ssu_score.c main.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

blank.o: blank.c
	$(CC) -c -o blank.o blank.c

ssu_score.o: ssu_score.c
	$(CC) -c -o ssu_score.o ssu_score.c

main.o: main.c
	$(CC) -c -o main.o main.c

clean:
	rm -rf $(OBJS) $(TARGET)
