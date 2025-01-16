CC = gcc
SRCS = $(wildcard *.c)
OUT = a.out
$(OUT): $(SRCS)
	$(CC) $(SRCS) -o $(OUT)
clean:
	rm -f $(OUT)
