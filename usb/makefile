runner_hid: runner_hid.o util.o hid.o parse_data.o
	gcc runner_hid.o util.o hid.o parse_data.o -o runner_hid -lusb-1.0 -lpthread

runner_hid.o: runner_hid.c
	gcc -c runner_hid.c

util.o: util.c
	gcc -c util.c

hidapi.o: hid.c
	gcc -c hid.c

parse_data.o: parse_data.c
	gcc -c parse_data.c

clean:
	rm *.o runner_hid runner
