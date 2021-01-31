#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "usb/runner_hid.h"

#define LINESINFILE 12		// this will be dynamic in the future
#define MAXFIELDS 20 		// this will stay static based on data gotten from watch
#define MAXFIELDLENGTH 100 	// just to make sure, once data is converted to ints it could change

int getRecords(char tables[LINESINFILE][MAXFIELDS][MAXFIELDLENGTH], char* datafilename, int* actualfields);
int parse(char* line, char* delimiter, char table[MAXFIELDS][MAXFIELDLENGTH], int* actualfields);
int printAll(char tables[LINESINFILE][MAXFIELDS][MAXFIELDLENGTH], int records, int* rows);

int main (int argc, char** argv)
{
	int c;                              		// holds options
	int flag = 0;                   		    // holds selected features
	int actualfields[LINESINFILE];				// holds actual fields count for record

	// data tables
	char tables[LINESINFILE][MAXFIELDS][MAXFIELDLENGTH];
	char* datafile = "data";

	// get parameters
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
	while ((c = getopt (argc, argv, "adgn:")) != -1) 
	{
		switch (c)
		{
			case 'a':
				flag |= 0x01;
				break;

			case 'd':
				flag |= 0x02;
				break;

			case 'g':
				flag |= 0x04;
				break;

			case 'n':
				flag |= 0x08;
				break;

			default:
				//usage (argv[0]);
				break;
		}
	}

	if (getRecords(tables, datafile, actualfields))
	{
		fprintf(stderr, "Reading from file failed, exiting\n");
		return 1;
	}

	if (flag & 0x01)
	{
		printf("\nPrinting all data returned: %d\n", printAll(tables, LINESINFILE, actualfields));
	}

    return 0;
}

int getRecords(char tables[LINESINFILE][MAXFIELDS][MAXFIELDLENGTH], char* datafilename, int* actualfields)
{	
	/* holds the line nr in file, 
	same as record number and table number in tables */
	int record = 0;
	char buffer[255];

	FILE* datafile;
	datafile = fopen(datafilename, "r");

	if (datafile == NULL)
	{
		fprintf(stderr, "Could not open file: %s\n", datafilename);
		return 1;
	}

	//https://solarianprogrammer.com/2019/04/03/c-programming-read-file-lines-fgets-getline-implement-portable-getline/
	while (fgets(buffer, sizeof(buffer), datafile) != 0)
	{
		//printf ("Current record: %d", record);
		if (parse(buffer, ",", tables[record], &actualfields[record]))
		{
			fprintf (stderr, "Something went wrong with parse\n");
			return 2;
		}
		record++;
	}

	fclose(datafile);
}

// https://www.daniweb.com/programming/software-development/threads/97843/parsing-a-csv-file-in-c
int parse(char* line, char* delimiter, char table[MAXFIELDS][MAXFIELDLENGTH], int* fieldcount)
{
	char* tokens = strtok(line, delimiter);
	int field = 0;

	while (tokens)
	{
		strcpy(table[field], tokens);
		field++;
		tokens = strtok('\0', delimiter);
	}
	*fieldcount = field;

	return 0;
}

int printAll(char tables[LINESINFILE][MAXFIELDS][MAXFIELDLENGTH], int records, int* rows)
{
	int i, j;

	printf("date, start time, duration, distance, avg hr, max hr, min hr, cal, fat %, avg pace, max pace, running index, max altitude, ascent, descent, time in zone1, time in zone2, time in zone3, time in zone4,time in zone5");

	for (i = 0; i < records; i++)
	{
		for (j = 0; j < rows[i]; j++)
		{
			printf("%s ", tables[i][j]);
		}
		printf("\n");
	}

	return 0;
}
