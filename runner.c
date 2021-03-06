#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "usb/runner_hid.h"

// https://www.youtube.com/watch?v=tonwdcHvjVY
#define LINESINFILE 12		// this will be dynamic in the future
#define MAXFIELDS 20 		// this will stay static based on data gotten from watch

int getAllRecords(double tables[LINESINFILE][MAXFIELDS], char* datafilename, int* actualfields);
int parse(char* line, char* delimiter, double table[MAXFIELDS], int* actualfields, int recordnr);
int getRecordDouble(double tables[LINESINFILE][MAXFIELDS], double array[LINESINFILE], int field);
int printAll(double tables[LINESINFILE][MAXFIELDS], int records, int* rows);
int showGoals(char* goalsfilename);
int setGoal(char* goalsfilename, char* goal);
char* timeFromSeconds(double seconds);

int main (int argc, char** argv)
{
	int c;                              		// holds options
	int flag = 0;                   		    // holds selected features
	int actualfields[LINESINFILE];				// holds actual fields count for record
	char *additionalArguments;					// holds additional arguments passed in
	int test;

	// data tables
	// https://www.youtube.com/watch?v=_j5lhHWkbnQ
	double tables[LINESINFILE][MAXFIELDS];
	char* datafile = "data";

	// get parameters
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
	while ((c = getopt(argc, argv, "adgn:")) != -1) 
	{
		switch (c)
		{
			// show all info
			case 'a':
				flag |= 0x01;
				break;

			// get data from watch
			case 'd':
				flag |= 0x02;
				break;

			// show goals
			case 'g':
				flag |= 0x04;
				break;

			//set new goal
			case 'n':
				flag |= 0x08;
				additionalArguments = optarg; // setting the pointer to additional args 
				break;

			default:
				//usage (argv[0]);
				// currently for testing

				
				break;
		}
	}

	if (getAllRecords(tables, datafile, actualfields))
	{
		fprintf(stderr, "Reading from file failed, exiting\n");
		return 1;
	}

	if (flag & 0x01)
	{
		printf("\nPrinting all data returned: %d\n", 
			printAll(tables, LINESINFILE, actualfields));

		double distance[LINESINFILE];
		int res = getRecordDouble(tables, distance, 3);
		double sum = 0;
		int i;
		for (i = 0; i < LINESINFILE; i++)
		{
			sum += distance[i];
		}
		printf ("Distance sum: %0.2f km\n", sum);
	}

	if (flag & 0x04)
	{
		if (showGoals("goals"))
		{
			fprintf(stderr, "Couldn't locate file %s\n", "goals");
		}
	}

	if (flag & 0x08)
	{
		if (setGoal("goals", additionalArguments))
		{
			fprintf(stderr, "Couldn't set goal(s)\n");
		}
	}

    return 0;
}

int getAllRecords(double tables[LINESINFILE][MAXFIELDS], char* datafilename, int* actualfields)
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
		if (parse(buffer, ",", tables[record], &actualfields[record], record))
		{
			fprintf(stderr, "Something went wrong with parse\n");
			return 2;
		}
		record++;
	}

	fclose(datafile);
	return 0;
}

// https://www.daniweb.com/programming/software-development/threads/97843/parsing-a-csv-file-in-c
int parse(char* line, char* delimiter, double table[MAXFIELDS], int* fieldcount, int recordnr)
{
	char* tokens = strtok(line, delimiter);
	int field = 0;
	double temp;
	char* remainder;

	while (tokens)
	{
		//strcpy(table[field], tokens);
		temp = strtod(tokens, &remainder);
		if (strlen(remainder) != 1)
		{
			fprintf(stderr, "Field %d on line %d in data has text in addition to number.\n",
					field, recordnr);
		} 
		table[field] = temp;

		field++;
		tokens = strtok('\0', delimiter);
	}
	*fieldcount = field;

	return 0;
}

int getRecordDouble(double tables[LINESINFILE][MAXFIELDS], double array[LINESINFILE], int field)
{
	int i;
	for (i = 0; i < LINESINFILE; i++)
	{
		array[i] = tables[i][field];
		//printf("%s\n", tables[i][field]);
	}

	return 0;
}

int printAll(double tables[LINESINFILE][MAXFIELDS], int records, int* rows)
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

int showGoals(char* goalsfilename)
{
	FILE* goalsfile;
	goalsfile = fopen(goalsfilename, "r");
	char buffer[50];

	// make sure file exists
	if (goalsfile == NULL)
		return 1;

	printf("Goals currently in file:\n");
	while (fgets(buffer, sizeof(buffer), goalsfile) != 0)
	{
		printf ("%s\n", buffer);
	}

	fclose(goalsfile);
	return 0;
}

int setGoal(char* goalsfilename, char* goal)
{	
	int goalNr = 0;
	float dist;

	FILE* goalsfile;
	goalsfile = fopen(goalsfilename, "a+");

	if (showGoals(goalsfilename))
	{
		printf("No goals currently set\n");
	}

	if (!memcmp(goal, "interactive", 3))
	{
		printf("No arguments given as goal, setting in interctive mode\n");

		// get available data points to choose from file?
		printf("Select which goal you want to set:\nDistance(1), Time(2), Calories(3) ");
		scanf("%d", &goalNr);

		switch (goalNr)
		{
			case 1:
				printf("Set desired distance: ");
				scanf("%f", &dist);

				if (dist <= 0)
					fprintf(stderr, "Cannot set distance to less than 0\n");
				else
					fprintf(goalsfile, "Distance - %f\n", dist);

				break;

			case 2:
				break;

			case 3:
				break;

			default:
				fprintf(stderr, "Not a valid option given as goal: %d", goalNr);
				break;
		}
	}
	else
	{
		printf ("%s\n", goal);
	}
	

	fclose(goalsfile);
	return 0;
}

char* timeFromSeconds(double seconds)
{
	// malloc will be used and pointer returned


}