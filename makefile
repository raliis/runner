runner: runner.o util.o
	gcc runner.o util.o -o runner

runner.o: runner.c
	gcc -c runner.c

util.o: util.c
	gcc -c util.c

clean:
	rm *.o runner
