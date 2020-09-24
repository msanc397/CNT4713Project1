myftp: myftp.o
	gcc -o myftp myftp.o
myftp.o: myftp.c
	gcc -c myftp.c
clean:
	rm *.o myftp
