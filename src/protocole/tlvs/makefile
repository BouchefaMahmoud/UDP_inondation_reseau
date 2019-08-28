SOURCES = $(wildcard *.c)
CFLAGS = -Wall -g
OBJS = $(SOURCES:%.c=%.o)
EX = tlvs.a 
all : $(EX)

$(EX) : $(OBJS)
			ar crUu $@ $^
			ranlib $@

%.o : %.c
		gcc -c $(CFLAGS) $<

%.o : %.h

clean:
		rm -f $(EX) *.o
