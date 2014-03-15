.PHONY: clean

CC=gcc
EXECUTABLE=imkea

all: $(EXECUTABLE)

$(EXECUTABLE) :
	gcc src/google-img.c src/imkea.c -o imkea `pkg-config --libs --cflags opencv` -ldl -lm -lcurl -ljansson

clean :
	rm -f $(EXECUTABLE) lib/*.o
