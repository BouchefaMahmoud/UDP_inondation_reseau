CC = gcc
CFLAGS = -Wall -g
LIBS = -lm
SOURCE = $(wildcard *.c)
OBJ = $(SOURCE: %.c=%.o)
EX = out
LIB =  */*/*.a

all :
		make -C pair
		make -C protocole
		make $(EX) 


 
$(EX) : $(OBJ) $(LIB)
	$(CC) -o $@ $^ $(LIBS) 
	
 
clean:
		make -C pair clean
		make -C protocole clean 
		rm -f *.o $(EX)
