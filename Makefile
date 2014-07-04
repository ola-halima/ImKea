.PHONY: clean

CC=gcc
EXECUTABLE=imkea

LIBS_PATH = -L /usr/local/lib
INC_PATH = -I /usr/local/include

all: $(EXECUTABLE)

$(EXECUTABLE) :
	$(CC) $(INC_PATH) $(LIBS_PATH) -g src/google-img.c src/imkea.c src/imkea-img.c src/img-color-map.c -o imkea `pkg-config --libs --cflags opencv` -ldl -lm -lcurl -ljansson

clean :
	rm -f $(EXECUTABLE)
