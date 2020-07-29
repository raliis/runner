# Doc 
Here i will be adding notes and other useful(imo) information about the project, libusb and usb itself.

## Flow
1. get list of available devices
	* libusb_get_device_list
2. find specific device from list, with bus and port given by user // in the future this will be automated, either with a script or within the program itself
	* iterate through list and check if given port and bus match with devices
3. open selected device for I/O
	* libusb_open()
4. set autorelease of kernel driver, otherwise wont be able to claim interface
	* libusb_set_auto_detach_kernel_driver
	* this will release kernel driver until you release the interface, then it will let kernel reclaim interface
5. claim the interface you wish to interact with
	* libusb_claim_interface
6. obtain configuration
	* libusb_get_configuration
	* this is useful just to know which configuration is currently selected, not sure if this has other uses
7. allocate transfer
	* libusb_alloc_transfer
	* using this helper function makes things a lot easier
8. fill transfer
	* select which transfer u want to use(control, isochronos, interrupt, bulk)
9. submit transfer
	* this adds your transfer to the queue, returns immediately and gives feedback if it was successful or there was a problem(i.e. interface was unclaimed or claimed by someone else, e.g kernel)
10. aaa
11. aaa
12. aaa
13. aaa
14. aaa
15. aaa
16. aaa
17. 



