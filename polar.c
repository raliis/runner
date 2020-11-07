#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <unistd.h>
#include "protocol.h"
#include "parse_data.h"
#include "database.h"

int main(int argc, char* argv[])
{
	unsigned char buf[256];
//	#define MAX_STR 255
//	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i, n,present;
	PGconn *db;
	unsigned char cmd1[256]={0x01,0x00,0x02,0x00,0x00};
	unsigned char cmd2[256]={0x01,0x00,0x02,0x10,0x00};
	unsigned char cmdusr[256]={0x01,0x00,0x02,0x0E,0x00};
	unsigned char cmdtrain[256]={0x01,0x00,0x03,0x06,0x00,0x00};
	unsigned char cmd3[256]={0x01,0x00,0x02,0x01,0x00};
	unsigned char cmd4[256]={0x01,0x00,0x02,0x04,0x00};
	unsigned char cmd5[256]={0x01,0x00,0x02,0x14,0x00};
	unsigned char cmd6[256]={0x01,0x00,0x02,0x14,0x20};
	unsigned char cmd7[256]={0x01,0x00,0x02,0x14,0x30};
	//struct hid_device_info *devs, *cur_dev;

printf("polar -- simple polar FT60 HRM FlowLink transfer into database and basic html analysis\n\n\
    Copyright (C) 2012  Samo Penic <samo.penic@opensarm.si>\n\n\\
\
    This program is free software: you can redistribute it and/or modify\n\
    it under the terms of the GNU General Public License as published by\n\
    the Free Software Foundation, either version 3 of the License, or\n\
    (at your option) any later version.\n\n\
\
    This program is distributed in the hope that it will be useful,\n\
    but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
    GNU General Public License for more details.\n\n\
\
    You should have received a copy of the GNU General Public License\n\
    along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n\n\
");


	db=NULL;
	/*db=1;
	if(test_db_connection("samo")){
		db=NULL;
	} else {
		db=connect_db("samo");
	}*/

	handle=openHID(0x0da4,0x0006);
	if(handle==NULL){
		printf("Error, could not connect to Polar FlowLink. Is Flowlink connected and do you have privileges? Maybe you should suid the program?\n");
		exit(1);
	}

	present=poolPresence(handle);
	if(present){
//		usleep(500*1000);
		executeCommand1(handle,buf,256,cmd1,5, TRUE);
		n=parseCommand1(buf,256);
//		usleep(500*1000);
		/*executeCommand1(handle,buf,256,cmd2,5, TRUE);
		readData(handle,buf,256, TRUE);
//		usleep(500*1000);
		printf("Great, let's get personal data!");
		executeCommand1(handle,buf,256,cmdusr,5, TRUE);
		parseUserData(buf,256, db);
		printf("Let's get training data!");	
		for(i=0;i<n;i++){
		cmdtrain[5]=i;
		executeCommand1(handle,buf,256,cmdtrain,6, TRUE);
		parseTrainingData(buf,256,db);
		}
		executeCommand1(handle,buf,256,cmd3,5, TRUE);
		parseVO2maxMeasurements(buf,256);
		executeCommand1(handle,buf,256,cmd4,5, TRUE);
		parseActiveProgram(buf,256);
		executeCommand1(handle,buf,256,cmd4,5, TRUE);
		executeCommand1(handle,buf,256,cmd5,5, TRUE);
		while(buf[1]){
		readData(handle,buf,256, TRUE);
		}
		//readData(handle,buf,256);
		executeCommand1(handle,buf,256,cmd6,5, TRUE);
		while(buf[1]){
		readData(handle,buf,256, TRUE);
		}*/
	/*	executeCommand1(handle,buf,256,cmd7,5);
		while(buf[1]){
		readData(handle,buf,256);
		}
*/
	}	
	closeHID(handle);

	return 0;
}
