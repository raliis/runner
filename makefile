runner: runner.o util.o runner_hid.o hidapi.o parse_data.o
	gcc runner.o util.o runner_hid.o hid.o parse_data.o -o runner -lusb-1.0 -lpthread

runner.o: runner.c
	gcc -c runner.c

util.o: util.c
	gcc -c util.c

runner_hid.o: usb/runner_hid.c
	gcc -c usb/runner_hid.c

hidapi.o: usb/hid.c
	gcc -c usb/hid.c

parse_data.o: usb/parse_data.c
	gcc -c usb/parse_data.c

clean:
	rm *.o runner runner_hid
