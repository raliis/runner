
#include "runner_hid.h"

int getData(int vid, int pid)
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
    
    // make sure vid and pid have values
	if (!vid || !pid)
	{
		printf("Too few arguments provided,\nUsing default values\nVid: %d, Pid: %d\n\n", DEFAULT_VID, DEFAULT_PID);
		vid = DEFAULT_VID;
		pid = DEFAULT_PID;
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

        // THIS IS WHERE THE MAIN COMMUNICATION HAPPENS
        unsigned char buf[256];
        int i, n, present;

        // Dont quite understand these values yet, wil have to test more
        unsigned char cmd1[256]={0x01,0x00,0x02,0x00,0x00};
        unsigned char cmd2[256]={0x01,0x00,0x02,0x10,0x00};
        unsigned char cmdusr[256]={0x01,0x00,0x02,0x0E,0x00};
        unsigned char cmdtrain[256]={0x01,0x00,0x03,0x06,0x00,0x00};
        unsigned char cmd3[256]={0x01,0x00,0x02,0x01,0x00};
        unsigned char cmd4[256]={0x01,0x00,0x02,0x04,0x00};
        unsigned char cmd5[256]={0x01,0x00,0x02,0x14,0x00};
        unsigned char cmd6[256]={0x01,0x00,0x02,0x14,0x20};
        unsigned char cmd7[256]={0x01,0x00,0x02,0x14,0x30};

        present=poolPresence(opened);
        if(present){
            //usleep(500*1000);
            executeCommand1(opened,buf,256,cmd1,5, TRUE);
            n=parseCommand1(buf,256);
            //usleep(500*1000);
            executeCommand1(opened,buf,256,cmd2,5, TRUE);
            readData(opened,buf,256, TRUE);
            //usleep(500*1000);
            printf("Great, let's get personal data!");
            executeCommand1(opened,buf,256,cmdusr,5, TRUE);
            //parseUserData(buf,256, db);
            printf("Let's get training data!");	
            for(i=0;i<n;i++){
                cmdtrain[5]=i;
                executeCommand1(opened,buf,256,cmdtrain,6, TRUE);
                //parseTrainingData(buf,256,db);
            }
            executeCommand1(opened,buf,256,cmd3,5, TRUE);
            parseVO2maxMeasurements(buf,256);
            executeCommand1(opened,buf,256,cmd4,5, TRUE);
            parseActiveProgram(buf,256);
            executeCommand1(opened,buf,256,cmd4,5, TRUE);
            executeCommand1(opened,buf,256,cmd5,5, TRUE);
            while(buf[1]){
                readData(opened,buf,256, TRUE);
            }
            //readData(opened,buf,256);
            executeCommand1(opened,buf,256,cmd6,5, TRUE);
            while(buf[1]){
                readData(opened,buf,256, TRUE);
            }
        }

    }
    else
    {
        printf("Couldnt open device!\n");
        return 1;
    }

    hid_close(opened);
        
	return 0;
}

