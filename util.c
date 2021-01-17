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

/*int is_interesting(libusb_device* dev, int bus, int port)
{
	return ((libusb_get_bus_number(dev) == bus) && libusb_get_port_number(dev) == port); 
}

void transfer_callback(struct libusb_transfer* transfer)
{
	printf ("We are now in the callback\n");

	printf ("Setup is: %s", libusb_control_transfer_get_setup(transfer));
	printf ("Data is: %s", libusb_control_transfer_get_data(transfer));
	// This seg faults, because no status is present
	//printf ("The status of transfer is: %s\n", transfer->status);
	
	printf ("buffer: %s\n", transfer->buffer);
	printf ("endpoint: %s\n", transfer->endpoint);
	printf ("flags: %s\n", transfer->flags);
	//printf ("length: %s\n", transfer->length);
	printf ("user data: %s\n", transfer->user_data);
	printf ("type: %s\n", transfer->type);
}

void print_device_info(libusb_device* dev) 
{
	struct libusb_device_descriptor desc;
	int err = libusb_get_device_descriptor(dev, &desc);
	if (err < 0) {
		printf ("failed to get device descriptor\n");
		return;
	}
	printf("Number of possible configurations: %d\n", (int)desc.bNumConfigurations);
	printf("Device Class: %d\n", (int)desc.bDeviceClass);

	struct libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);
	printf("Interfaces: %d\n", (int)config->bNumInterfaces);
	const struct libusb_interface *inter;
	const struct libusb_interface_descriptor *interdesc;
	const struct libusb_endpoint_descriptor *epdesc;
	for(int i=0; i<(int)config->bNumInterfaces; i++) 
	{
		inter = &config->interface[i];
		printf("\tINTER: Number of alternate settings: %d\n", inter->num_altsetting);
		printf("\n");
		for(int j=0; j<inter->num_altsetting; j++) 
		{
			interdesc = &inter->altsetting[j];
			printf("\t\tINTERDESC: bInterfaceNumber: %d\n", (int)interdesc->bInterfaceNumber);
			printf("\t\tINTERDESC: bNumEndpoints: %d\n", (int)interdesc->bNumEndpoints);
			printf("\t\tINTERDESC: bAlternateSetting: %d\n", (int)interdesc->bAlternateSetting);
			printf("\t\tINTERDESC: bDescriptorType: %d\n", (int)interdesc->bDescriptorType); // LIBUSB_DT_INTERFACE 
			printf("\t\tINTERDESC: bInterfaceClass: %d\n", (int)interdesc->bInterfaceClass); // LIBUSB_CLASS_HID 
			printf("\t\tINTERDESC: bInterfaceProtocol: %d\n", (int)interdesc->bInterfaceProtocol);
			printf("\t\tINTERDESC: bInterfaceSubClass: %d\n", (int)interdesc->bInterfaceSubClass);
			printf("\t\tINTERDESC: bLength: %d\n", (int)interdesc->bLength);
			//printf("\t\tINTERDESC: extra: %s\n", (int)interdesc->extra);
			printf("\t\tINTERDESC: extra_length: %d\n", (int)interdesc->extra_length);
			printf("\t\tINTERDESC: iInterface: %d\n", (int)interdesc->iInterface);
			printf("\n");

			for(int k=0; k<(int)interdesc->bNumEndpoints; k++) 
			{
				epdesc = &interdesc->endpoint[k];
				printf("\t\t\tEP bDescriptorType: %d\n", (int)epdesc->bDescriptorType); // LIBUSB_DT_ENDPOINT
				printf("\t\t\tEP bEndpointAddress: %d\n", (int)epdesc->bEndpointAddress); // Bits 0:3 are the endpoint number. Bits 4:6 are reserved. Bit 7 indicates direction,
				printf("\t\t\tEP bInterval: %d\n", (int)epdesc->bInterval); // 1
				printf("\t\t\tEP bLength: %d\n", (int)epdesc->bLength);
				printf("\t\t\tEP bmAttributes: %d\n", (int)epdesc->bmAttributes); // interrupt endpoints
				printf("\t\t\tEP bRefresh: %d\n", (int)epdesc->bRefresh); // only for audio devices, can be ignored
				printf("\t\t\tEP bSynchAddress: %d\n", (int)epdesc->bSynchAddress); // also only audio devices
				//printf("\t\t\tEP extra: %s\n", epdesc->extra);
				printf("\t\t\tEP extra_length: %d\n", (int)epdesc->extra_length);
				printf("\t\t\tEP wMaxPacketSize: %d\n", (int)epdesc->wMaxPacketSize);
				printf("\n");
			}
		}
	}
	printf("\n\n\n");
	libusb_free_config_descriptor(config);
}

int poolPresence(hid_device *handle){
	unsigned char buf[256];
	memset(buf,0,sizeof(buf));
	int res,i;

	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(handle, buf, sizeof(buf));
	if (res < 0) {
		printf("Unable to get a feature report.\n");
		printf("%ls", hid_error(handle));
	}
	else {
		// Print out the returned buffer.
		printf("Feature Report\n   ");
		for (i = 0; i < res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}
	if(buf[2]==0x02 && buf[3]==0x0b){
		printf("HRM not present.\n");
		return 0;
	}
	printf("HRM present.\n");
	return 1;
}

int executeCommand1(hid_device *handle, unsigned char *buf, int bufsize, unsigned char *command, int commandsize, int showdata){
	int res;
	memset(buf,0,bufsize*sizeof(unsigned char));
	memcpy(buf,command,commandsize*sizeof(unsigned char));
	res = hid_write(handle, buf, 17);
	if (res < 0) {
		printf("Unable to write()\n");
		printf("Error: %ls\n", hid_error(handle));
		return 0;
	}
	res=readData(handle,buf,bufsize, showdata);
	return res;
}

int readData(hid_device *handle, unsigned char *buf, int bufsize, int showdata){
	int i,res;
	memset(buf,0,bufsize*sizeof(unsigned char));
	res = 0;
	for(i=1;i<5;i++) {
		res = hid_read(handle, buf, bufsize*sizeof(unsigned char));
		if (res > 1) break;
		if (res == 0){
			printf("waiting...\n");
			usleep(500*1000);
		}
		if (res < 0)
			printf("Unable to read()\n");
	}
	if (res<=0) {
		printf("No data received!\n");
		return 0;
	}

	printf("Data received:\n   ");
	// Print out the returned buffer.
	if (showdata==TRUE){
		for (i = 0; i < res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}
	return 1;
}*/