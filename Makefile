.PHONY: clean

CC=gcc
EXECUTABLE=imkea

all: $(EXECUTABLE)

$(EXECUTABLE) :
	$(CC) -g src/google-img.c src/imkea.c src/imkea-img.c src/img-color-map.c -o imkea `pkg-config --libs --cflags opencv` -ldl -lm -lcurl -ljansson

clean :
	rm -f $(EXECUTABLE)
