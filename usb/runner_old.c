#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#include "hidapi.h"

#define DEFAULT_VID 0x0da4
#define DEFAULT_PID 0x0006


int main(int argc, char** argv)
{
	// Initializing the library, setting debug level
	const struct libusb_version* ver = libusb_get_version();
	int init_result = libusb_init(NULL);

	// Set verbosity level to 4 to get all the available info
	// Set verbosity to 3 for warnings only
	libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, 3); 

	// Greet the user
	printf ("\nRunner v0.1, using libusb %d.%d.%d\n\n", ver->major, ver->minor, ver->micro);

	// Using vid and pid to access device
	int vid, pid;
	if (argc < 3)
	{
		printf("Too few arguments provided,\nUsing default values\nVid: %d, Pid: %d\n\n", DEFAULT_VID, DEFAULT_PID);
		vid = DEFAULT_VID;
		pid = DEFAULT_PID;
	}
	else
	{
		// Get bus and port from user
		vid = atoi(argv[1]);
		pid = atoi(argv[2]);
	}

	//
	// STRECTH GOAL
	// Eventually there will be a dmenu script that will help choose a device
	//
	// Open device for I/O
	libusb_device_handle* handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
	if (handle) // libusb_open_device_with_vid_pid returns the handle on success
	{
		printf("Device opened successfully!\n\n");

		//printing all info on system
		libusb_device *dev = libusb_get_device(handle); 
		print_device_info(dev);

		int config;
		if (!libusb_get_configuration(handle, &config))
		{
			printf("setting kernel autorelease: \nvalue of config: %d", config);
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
				
				printf ("Setting of alt setting: %s\n", libusb_strerror(libusb_set_interface_alt_setting(handle, 0, 0)));

				//printf("kernel driver active: %d\n", libusb_kernel_driver_active(handle, 0));

				// Allocate a transfer, with argument 0 for interrupt endpoints
				//struct libusb_transfer* transfer1 = libusb_alloc_transfer(0);
				//struct libusb_transfer* transfer2 = libusb_alloc_transfer(0);
				struct libusb_transfer* transfer3 = libusb_alloc_transfer(0);

				// Create buffer for data
				//char* buffer1 = (char*) libusb_dev_mem_alloc(handle, 64 * 64);
				//char* buffer2 = (char*) libusb_dev_mem_alloc(handle, 64 * 64);
				char* buffer3 = (char*) libusb_dev_mem_alloc(handle, 64 * 64);

				void (* callback) (struct libusb_transfer*);
				callback = &transfer_callback;
				int transfer_error;

				/*libusb_fill_control_setup(
					buffer1, // buffer where data is stored
					0x80, // dir: in, endpoint , 
					LIBUSB_REQUEST_GET_DESCRIPTOR, // == 0x06
					0x00, 
					0x00, 
					0x12); // 18

				libusb_fill_control_setup(
					buffer2, // buffer where data is stored
					0x80, // dir: in, endpoint , 
					LIBUSB_REQUEST_GET_CONFIGURATION, // == 0x06
					0x00, 
					0x00, 
					0x12); // 18
*/
				// THIS IS THE SET CONFIG INTERRUPT
				libusb_fill_control_setup(
					buffer3, // buffer where data is stored
					0x00, // dir: out, endpoint 0, 
					LIBUSB_REQUEST_SET_CONFIGURATION, // == 0x09
					0x01, 
					0x00, 
					0x00);
				
				//libusb_fill_control_transfer(transfer1, handle, buffer1, callback, NULL, 0);
				//libusb_fill_control_transfer(transfer2, handle, buffer2, callback, NULL, 0);
				libusb_fill_control_transfer(transfer3, handle, buffer3, callback, NULL, 0);

				//transfer_error = libusb_submit_transfer(transfer1);
				//transfer_error = libusb_submit_transfer(transfer2);
				transfer_error = libusb_submit_transfer(transfer3);

				// Fill allocated interrupt transfer
				/*libusb_fill_interrupt_transfer(transfer, handle, 0x00, buffer, sizeof(buffer), callback, NULL, 500);

				// Send transfer
				int transfer_error = libusb_submit_transfer(transfer);
				*/
				
				while (1)
				{
					sleep(1);
					// This will need to be a while()
					libusb_handle_events(NULL);
					
				}

				printf("Submitting transfer returned: %s\n", libusb_strerror(transfer_error));
				if (transfer_error != 0)
				{
					int release_error = libusb_release_interface(handle, 0x00);
					printf("release of interface with error: (%d)%s \n", release_error, libusb_strerror(release_error)); 			
					libusb_close(handle);
					exit(1);
				}
			}
		}
		
		printf("releasing interface: \n");
		int release_error = libusb_release_interface(handle, 0x00);
		printf("release of interface with error: (%d)%s \n", release_error, libusb_strerror(release_error)); 			
		libusb_close(handle);
	}
	else // Device not opened
	{
		// 
		// TODO: add better error logging, taking advantage of return codes
		//
		error("Could not open device\n", ERROR);
	}

	return 0;
}*/

