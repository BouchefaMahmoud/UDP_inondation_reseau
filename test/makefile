CC = gcc
CFLAGS = -Wall -g
LIBS = -lcheck_pic -pthread -lrt -lm -lsubunit -lSDL2 -lSDL2_image -lSDL2_gfx
SOURCE = $(wildcard *.c)
OBJ = $(SOURCE: %.c=%.o)
EX = out
LIB =  ../src/protocole/*/*.a

all :
	
		make -C ../src/protocole
		make $(EX) 


 
$(EX) : $(OBJ) $(LIB)
	$(CC) -o $@ $^ $(LIBS) 
	
 
clean:
		make -C ../src/protocole clean 
		rm -f *.o $(EX)
