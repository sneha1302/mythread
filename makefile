CC = gcc

# Compile options
#   -ggdb   Bakes in debugging symbols for debugging with gdb
#   -Wall   Turns on verbose compilation warnings

CFLAGS = -Wall -ggdb
NAME = mythread

all: lib

lib: $(NAME).o mystuff.o
	ar -q $(NAME).a $(NAME).o mystuff.o

mystuff.o: mystuff.h mystuff.c 
	$(CC) mystuff.c -c

$(NAME).o: $(NAME).h $(NAME).c 
	$(CC) $(NAME).c -c 


#
# Debugging 
#
debug: debug-lib

debug-lib: $(NAME)debug.o
	ar -cvq $(NAME)debug.a $(NAME)debug.o

$(NAME)debug.o:
	$(CC) $(CFLAGS) $(NAME).c -c -o $(NAME)debug.o

clean:
	rm -f *.o *.a $(NAME) *debug* *swp
