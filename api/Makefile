CC=gcc
CFLAGS=-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11
PATHbin=./bin/
PATHrec=./src/
LAB6LIB=-lulfius -lorcania -ljansson

api: $(PATHbin)api

$(PATHbin)api: $(PATHrec)api.c $(PATHrec)funciones.c 
	mkdir -p $(PATHbin) 
	$(CC) $(CFLAGS) -o $(PATHbin)api $(PATHrec)api.c $(LAB6LIB)

clean:
	rm -f -d $(PATHbin)* $(PATHbin) 
