#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include "util.h"

int main(int argc, char** argv)
{
	
	// Initializing the library
	const struct libusb_version* ver = libusb_get_version();
	int init_result = libusb_init(NULL);

	// Greet the user
	printf ("Runner v0.1, using libusb %d.%d.%d\n", ver->major, ver->minor, ver->micro);

	// Discovery of devices
	libusb_device** list;			// var for holding list of available devices
	libusb_device* found = NULL; 	// var for holding found device
	
	// Get list of devices into list and store count
	ssize_t count = libusb_get_device_list(NULL, &list);
	if (count < 0)
		error("Count of found devices is less than 0", ERROR);

	// Find specific device in found devices
	int i = 0;
	printf ("List length is: %d\n", count);

	// List all devices before checking idf bus and port are selected
	for (i = 0; i < count; i++)
	{
		libusb_device* device = list[i];
		
		// For debugging - prints all devices
		printf("Device %d(%d): bus %d, port %d\n", i, libusb_get_device_address(device), libusb_get_bus_number(device), libusb_get_port_number(device));
	}

	// Make sure user gave bus and port number
	if (argc < 3)
		error("Too few arguments\nUsage: runner bus port\n", ERROR);

	// Get bus and port from user
	int bus = atoi(argv[1]);
	int port = atoi(argv[2]);

	// Look for interesting device
	for (i = 0; i < count; i++)
	{
		libusb_device* device = list[i];
		
		if (is_interesting(device, bus, port))
		{
			found = device;
			printf("Found interesting device in position %d\n", i);
			break;
		}
	}

	// Open device for I/O
	if (found)
	{
		libusb_device_handle* handle;
		
		if (libusb_open(found, &handle)) // libusb_open returns 0 on success
		{
			// 
			// TODO: add better error logging, taking advantage of return codes
			//
			error("Could not open device\n", ERROR);
		}
		else // Device successfully opened
		{
			
			printf("Device opened successfully!\n");

			// Allocate a transfer, with argument 0 for interrupt endpoints
			struct libusb_transfer* transfer = libusb_alloc_transfer(0);

			// Create buffer for data
			char* buffer = (char*) malloc(500);
			void (* callback) (struct libusb_transfer*);
			callback = &transfer_callback;

			// Fill allocated transfer
			libusb_fill_interrupt_transfer(transfer, handle, 0x00, buffer, sizeof(buffer), callback, NULL, 0);

			// Send transfer
			int transfer_error = libusb_submit_transfer(transfer);
			printf("Submitted transfer, returned: %s\n", libusb_strerror(transfer_error));

			/*while(1)
			{
				sleep(1);
			}*/

			libusb_close(handle);
		}
	
	}

	// Free device list when done
	libusb_free_device_list(list, 1);
	return 0;
}

