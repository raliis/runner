#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#include "util.h"
#include "hidapi.h"

#define DEFAULT_VID 0x0da4
#define DEFAULT_PID 0x0006

int main(int argc, char** argv)
{
	// Get version
    const struct hid_api_version *ver = hid_version();

    // Initialize hidapi
    int init_result = hid_init();
    // if init fails, exit, otherwise greet user
    if (init_result)
        return 1;
    else // Greet user
        printf("Currently using hidapi-%d.%d\n", ver->major, ver->minor);
    
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

    // Get list of devices with given Vid and Pid
    struct hid_device_info *list = hid_enumerate(vid, pid);
    struct hid_device_info *device = list;
    wchar_t *serialnum;
    while (device->next);
    {
        // The %ls is for wide string that are returned
        printf("Device manufacturer string: %ls\n", device->manufacturer_string);
        printf("Device prod string: %ls\n", device->product_string);
        printf ("Device serial num: %ls\n", device->serial_number);
        wcscpy(serialnum, device->serial_number);
        printf("Device path: %s\n", device->path);
        printf("Device if num: %d\n\n", device->interface_number);
        device = device->next;
    }
    hid_free_enumeration(list);

    // Open device with vid and pid
    hid_device *opened = hid_open(vid, pid, serialnum);
    if (opened)
    {
        printf("Managed to open device!\n");
    }
    else
    {
        printf("Couldnt open device!\n");
        return 1;
    }
        

	return 0;
}

