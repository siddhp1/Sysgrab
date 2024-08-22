sysgrab: sysgrab.o data.o
	gcc -o sysgrab sysgrab.o data.o

sysgrab.o: sysgrab.c data.h
	gcc -c sysgrab.c

data.o: data.c data.h
	gcc -c data.c

# Delete executable and object files from directory
clean:
	rm sysgrab sysgrab.o data.o