runner: runner.o util.o
	gcc runner.o util.o -o runner -lusb-1.0

runner.o: runner.c
	gcc -c runner.c

util.o: util.c
	gcc -c util.c

clean:
	rm *.o runner
