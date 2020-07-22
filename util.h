#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define ERROR 1
#define WARNING 2

int select_mode(char*);

// severity 0 means warning, 1 means error, quits
void error(char*, int);

// determine if this is the device were looking for
int is_interesting(libusb_device*, int, int);

// transfer callback
void transfer_callback(struct libusb_transfer*);
#endif

