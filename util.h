#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>


#define MAX_STR 255
#define TRUE 1
#define FALSE 0
#define ERROR 1
#define WARNING 2

// device specific endpoints
#define ENDPOINT_OUT 1
#define ENDPOINT_IN 81

int select_mode(char*);

// severity 0 means warning, 1 means error, quits
void error(char*, int);

// determine if this is the device were looking for
/*int is_interesting(libusb_device*, int, int);

// transfer callback
void transfer_callback(struct libusb_transfer*);

// print all available device info
void print_device_info(libusb_device *dev);

int poolPresence(hid_device *handle);

int executeCommand1(hid_device *handle, unsigned char *buf, int bufsize, unsigned char *command, int commandsize, int showdata);

int readData(hid_device *handle, unsigned char *buf, int bufsize, int showdata);
*/
#endif

