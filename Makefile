myftp: myftp.o
	gcc -o mywords myftp.o
myftp.o: myftp.c
	gcc -c myftp.c
clean:
	rm *.o myftp
