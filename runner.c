#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include "util.h"

#define DEFAULT_PORT 4
#define DEFAULT_BUS 1

int main(int argc, char** argv)
{
	// Initializing the library
	const struct libusb_version* ver = libusb_get_version();
	int init_result = libusb_init(NULL);
	libusb_set_debug(NULL, 3); //set verbosity level to 4, get all the available info

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

	// List all devices before checking if bus and port are selected
	for (i = 0; i < count; i++)
	{
		libusb_device* device = list[i]; // var for holding each device 
		
		// For debugging - prints all devices
		printf("\tDevice %d(%d): bus %d, port %d\n", i, libusb_get_device_address(device), libusb_get_bus_number(device), libusb_get_port_number(device));
		//print_device_info (device);
	}

	int bus = DEFAULT_BUS;
	int port = DEFAULT_PORT;
	// Make sure user gave bus and port number
	if (argc < 3)
	{
		printf("Too few arguments provided,\nUsing default values\nBus: %d, Port: %d\n", DEFAULT_BUS, DEFAULT_PORT);
	}
	else
	{
		// Get bus and port from user
		bus = atoi(argv[1]);
		port = atoi(argv[2]);
	}

	// Look for interesting device
	for (i = 0; i < count; i++)
	{
		libusb_device* device = list[i];

		if (is_interesting(device, bus, port))
		{
			found = device;
			print_device_info (found);
			printf("Found interesting device in position %d\n", i);
			break;
		}
	}

	// Free device list when done
	libusb_free_device_list(list, 1);

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
			
			int ahah;
			if (!libusb_get_configuration(handle, &ahah))
			{
				printf("setting kernel autorelease: \nvalue of ahah: %d", ahah);
				int autorelease_error = libusb_set_auto_detach_kernel_driver(handle, 1);
				printf("\tAutorelease of interface 0: %d, %s \n", autorelease_error, libusb_strerror(autorelease_error));

				printf("claiming interface: \n");
				// maybe i need to claim more interfaces, but there seem to be no more ones, at least libusb cant find more
				// currently only claiming 0
				int claim_error = libusb_claim_interface(handle, 0);
				printf("\tclaim of interface 0: %d, %s \n", claim_error, libusb_strerror(claim_error));

				if (!claim_error)
				{
					printf("getting config: \n");
					int current_config;
					int config_error = libusb_get_configuration(handle, &current_config);
					printf("\tcurrent config is: %d\n\terror: %d, %s\n", current_config, config_error, libusb_strerror(config_error));
					
					//printf("kernel driver active: %d\n", libusb_kernel_driver_active(handle, 0));

					// Allocate a transfer, with argument 0 for interrupt endpoints
					struct libusb_transfer* transfer = libusb_alloc_transfer(0);

					// Create buffer for data
					char* buffer = (char*) libusb_dev_mem_alloc(handle, 64);
					void (* callback) (struct libusb_transfer*);
					callback = &transfer_callback;

					libusb_fill_control_setup(
						buffer, // buffer where data is stored
						0x81, // LIBUSB_RECIPIENT_DEVICE == 0 | LIBUSB_REQUEST_TYPE_STANDARD == 0 | ENDPOINT_IN == 81
						LIBUSB_REQUEST_GET_DESCRIPTOR, // == 0x06
						0x00, 
						0x00, 
						0xFF);
					
					libusb_fill_control_transfer(transfer, handle, buffer, callback, transfer, 0);

					int transfer_error = libusb_submit_transfer(transfer);

					// Fill allocated interrupt transfer
					/*libusb_fill_interrupt_transfer(transfer, handle, 0x00, buffer, sizeof(buffer), callback, NULL, 500);

					// Send transfer
					int transfer_error = libusb_submit_transfer(transfer);
					*/
					
					printf("Submitting transfer returned: %s\n", libusb_strerror(transfer_error));
					if (transfer_error != 0)
					{
						int release_error = libusb_release_interface(handle, 0x00);
						printf("release of interface with error: (%d)%s \n", release_error, libusb_strerror(release_error)); 			
						libusb_close(handle);
						exit(1);
					}
						
					while (1)
					{
						sleep(1);
						// This will need to be a while()
						libusb_handle_events(NULL);
						
					}				
				}
			}
			
			printf("releasing interface: \n");
			int release_error = libusb_release_interface(handle, 0x00);
			printf("release of interface with error: (%d)%s \n", release_error, libusb_strerror(release_error)); 			
			libusb_close(handle);
		}
	
	}

	return 0;
}

