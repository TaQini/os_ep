all: a.c mytime.c
	gcc a.c
	gcc mytime.c -o mytime
a.out: a.c
	gcc a.c
mytime: mytime.c
	gcc mytime.c -o mytime
clean:
	rm -f peda*
	rm -f .gdb*
	rm -f *.o
	rm -f a.out
	rm -f mytime
