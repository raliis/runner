#include "util.h"


int select_mode(char* mode)
{
	if (!(strcmp (mode, "add") && strcmp (mode, "a")))
	{
		return 0;
	}
	else if (!(strcmp (mode, "remove") && strcmp (mode, "rm") && strcmp (mode, "r")))
	{
		return 1;
	}
	else if (!(strcmp (mode, "edit") && strcmp (mode, "e")))
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

void error(char* msg, int severity)
{
	if (severity)
	{
		fprintf(stderr, "Error: %s\n", msg);
		exit(1);
	}
	else
	{
		fprintf(stderr, "Warning: %s\n", msg);
	}
}

int is_interesting(libusb_device* dev, int bus, int port)
{
	return ((libusb_get_bus_number(dev) == bus) && libusb_get_port_number(dev) == port); 
}

void transfer_callback(struct libusb_transfer* transfer)
{
	printf ("We are now in the callback\n");
	printf ("The status of transfer is: %s\n", transfer->status);
	printf ("The data received is: %s\n", transfer->buffer);
}
