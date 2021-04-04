CC = gcc
CFLAGS = -Wall -g
USBFLAGS = -lusb-1.0 -lpthread	

runner: runner.o util.o runner_hid.o hidapi.o parse_data.o
	$(CC) $(CFLAGS) runner.o util.o runner_hid.o hid.o parse_data.o -o runner $(USBFLAGS)

runner.o: runner.c
	$(CC) $(CFLAGS) -c runner.c 

util.o: util.c
	$(CC) $(CFLAGS) -c util.c

runner_hid.o: usb/runner_hid.c
	$(CC) -c usb/runner_hid.c

hidapi.o: usb/hid.c
	$(CC) -c usb/hid.c

parse_data.o: usb/parse_data.c
	$(CC) -c usb/parse_data.c

clean:
	rm *.o runner usb/runner_hid

install:
#mkdir -p $(HOME)/.local/bin/runner
	cp runner $(HOME)/.local/bin/