runner_hid: runner_hid.o util.o hid.o
	gcc runner_hid.o util.o hid.o -o runner_hid -lusb-1.0 -lpthread

runner_hid.o: runner_hid.c
	gcc -c runner_hid.c

util.o: util.c
	gcc -c util.c

hidapi.o: hid.c
	gcc -c hid.c

clean:
	rm *.o runner_hid runner
