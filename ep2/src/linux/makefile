all: a.c b.c mytime.c
	gcc a.c -o a.out
	gcc b.c -o b.out
	gcc mytime.c -o mytime
a.out: a.c
	gcc a.c -o a.out
b.out: b.c
	gcc b.c -o b.out
mytime: mytime.c
	gcc mytime.c -o mytime
clean:
	rm -f peda*
	rm -f .gdb*
	rm -f *.o
	rm -f *.out
	rm -f mytime
run:
	./mytime ./a.out
