#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "util.h"
#include "usb/runner_hid.h"

// https://www.youtube.com/watch?v=tonwdcHvjVY
#define MAXFIELDS 20 		// this will stay static based on data gotten from watch
int LINESINFILE;			// global variable for number of lines in data 

int getAllRecords(double tables[LINESINFILE][MAXFIELDS], char* datafilename, int* actualfields);
int parse(char* line, char* delimiter, double table[MAXFIELDS], int* actualfields, int recordnr);
double getRecordSum(double tables[LINESINFILE][MAXFIELDS], int* records, int field);
double* getRecordsThisMonth(double tables[LINESINFILE][MAXFIELDS], int* records, int field);
int printAll(double tables[LINESINFILE][MAXFIELDS], int records, int* rows);
int showGoals(char* goalsfilename);
int setGoal(char* goalsfilename, char* goal);
char* formatDate(double seconds);
char* formatTime(double seconds);
int* dataThisMonth(double tables[LINESINFILE][MAXFIELDS]);
int readLines(char* filename);

int main (int argc, char** argv)
{
	int c;                              		// holds options
	int flag = 0;                   		    // holds selected features
	int actualfields[LINESINFILE];				// holds actual fields count for record
	char *additionalArguments;					// holds additional arguments passed in
	LINESINFILE = readLines("data");			// count lines in data file
	
	int *testarr;
	int testint;

	// data tables
	// https://www.youtube.com/watch?v=_j5lhHWkbnQ
	double tables[LINESINFILE][MAXFIELDS];
	char* datafile = "data";

	char* test; // just for testing and holding returned pointer

	// get parameters
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
	while((c = getopt(argc, argv, "adgnt")) != -1) 
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

				if(argc == 2)
					additionalArguments = NULL;
				else if(argc == 4)
					printf("setting now\n");
				//printf("Additional: %s\n", argv[2]);
				//additionalArguments = (argv[2] + 1); // setting the pointer to additional args 

				break;
			
			case 't': //purely for testing
				flag |= 0x16;
				testint = getAllRecords(tables, datafile, actualfields);
				testarr = dataThisMonth(tables);
				break;

			default:
				//usage (argv[0]);
				// currently for testing
				/*test = formatTime(30);
				test = formatTime(70);
				test = formatTime(3600);
				*/
				break;
		}
	}

	if(getAllRecords(tables, datafile, actualfields))
	{
		fprintf(stderr, "Reading from file failed, exiting\n");
		return 1;
	}

	if(flag & 0x01)
	{
		printf("\nPrinting all data returned: %d\n", 
			printAll(tables, LINESINFILE, actualfields));

		double distance[LINESINFILE];
		/*int res = getRecordDouble(tables, distance, 3);
		double sum = 0;
		int i;
		for (i = 0; i < LINESINFILE; i++)
		{
			sum += distance[i];
		}
		printf("Distance sum: %0.2f km\n", sum);*/
	}

	if(flag & 0x04)
	{
		if(showGoals("goals"))
		{
			fprintf(stderr, "Couldn't locate file %s\n", "goals");
		}
	}

	if(flag & 0x08)
	{
		if(setGoal("goals", additionalArguments))
		{
			fprintf(stderr, "Couldn't set goal(s)\n");
		}
	}

    return 0;
}

int readLines(char* filename)
{
	int lines = 0;
	char c;

	FILE* pfile;
	pfile = fopen(filename, "r");
	if(pfile == NULL)
		fprintf(stderr, "Couldn't open file: %s\n", filename);

	while((c=fgetc(pfile))!=EOF) {
      if(c == '\n')
         lines++;
   	}
	fclose(pfile);

	return lines;
}

int getAllRecords(double tables[LINESINFILE][MAXFIELDS], char* datafilename, int* actualfields)
{	
	/* holds the line nr in file, 
	same as record number and table number in tables */
	int record = 0;
	char buffer[255];

	FILE* datafile;
	datafile = fopen(datafilename, "r");

	if(datafile == NULL)
	{
		fprintf(stderr, "Could not open file: %s\n", datafilename);
		return 1;
	}

	//https://solarianprogrammer.com/2019/04/03/c-programming-read-file-lines-fgets-getline-implement-portable-getline/
	while (fgets(buffer, sizeof(buffer), datafile) != 0)
	{
		//printf("Current record: %d", record);
		if(parse(buffer, ",", tables[record], &actualfields[record], record))
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
		//check if field has any more info in addition to numbers
		temp = strtod(tokens, &remainder);
		if(strlen(remainder) > 1)
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

double getRecordSum(double tables[LINESINFILE][MAXFIELDS], int* records, int field)
{
	int lines = sizeof(records)/sizeof(records[0]);
	double result;
	int i = 0;

	for(; i < lines; i++)
	{
		result += tables[i][field];
	}

	return result;
}

double* getRecordsThisMonth(double tables[LINESINFILE][MAXFIELDS], int* records, int field)
{
	int lines = sizeof(records)/sizeof(records[0]);
	double* result;
	result = (double*) malloc(sizeof(double) * lines); 
	int i = 0;

	for(; i < lines; i++)
	{
		result[i] = tables[i][field];
	}

	return result;
}

int printAll(double tables[LINESINFILE][MAXFIELDS], int records, int* rows)
{
	int i, j;
	char* time;

	printf("date, start time, duration, distance, avg hr, max hr, min hr, cal, fat %, avg pace, max pace, running index, max altitude, ascent, descent, time in zone1, time in zone2, time in zone3, time in zone4,time in zone5\n");

	for (i = 0; i < records; i++)
	{
		for (j = 0; j < rows[i]; j++)
		{	
			// This correctly formats the date and shows an understandable value
			if(j == 0)
			{
				time = formatDate(tables[i][j]);
				printf("%s ", time);
				free(time);
			}
			else if(j <= 2 || j >= 15)
			{
				time = formatTime(tables[i][j]);
				printf("%s ", time);
				free(time);
			}
			else
			{
				if(tables[i][j] == (int)tables[i][j])
					printf("%.0lf ", tables[i][j]);
				else
					printf("%.2lf ", tables[i][j]);
			}
		}
		printf("\n");
	}

	return 0;
}

// getFieldByName(char* field)

int showGoals(char* goalsfilename)
{
	FILE* goalsfile;
	goalsfile = fopen(goalsfilename, "r");
	char* tokens;
	char buffer[50];

	// make sure file exists
	if(goalsfile == NULL)
		return 1;

	printf("Goals currently in file:\n");
	while (fgets(buffer, sizeof(buffer), goalsfile) != 0)
	{
		tokens = strtok(buffer, " ");
		switch(*tokens)
		{
			case 'd':
				printf("Distance: ");
				tokens = strtok(NULL, " ");
				printf("%d km\n", atoi(tokens));
				break;
			case 't':
				printf("Time: ");
				tokens = strtok(NULL, " ");
				printf("%s\n", formatTime(atof(tokens)));
				break;
			case 'c':
				printf("Calories: ");
				break;
		}

		
	}

	/* 
	int field = 0;
	double temp;
	char* remainder;

	while (tokens)
	{
		//check if field has any more info in addition to numbers
		temp = strtod(tokens, &remainder);
		if(strlen(remainder) > 1)
		{
			fprintf(stderr, "Field %d on line %d in data has text in addition to number.\n",
					field, recordnr);
		} 
		table[field] = temp;

		field++;
		tokens = strtok('\0', delimiter);
	}*/

	fclose(goalsfile);
	return 0;
}

int setGoal(char* goalsfilename, char* goal)
{	
	int goalNr = 0;
	float dist = 0;
	float time = 0;
	int cal = 0;

	FILE* goalsfile;
	goalsfile = fopen(goalsfilename, "a+");

	if(showGoals(goalsfilename))
	{
		printf("No goals currently set\n");
	}

	if(goal == NULL)
	{
		printf("No arguments given as goal, setting in interctive mode\n");

		// get available data points to choose from file?
		do
		{
			printf("Select which goal you want to set:\nDistance(1), Time(2), Calories(3): ");
			scanf("%d", &goalNr);
		}
		while (goalNr > 3 || goalNr < 1);

		switch (goalNr)
		{
			case 1:
				printf("Set desired distance(km): ");
				scanf("%f", &dist);

				if(dist <= 0)
					fprintf(stderr, "Cannot set distance to less than 1\n");
				else
					fprintf(goalsfile, "d %f\n", dist);

				break;

			case 2:
				printf("Set desired time(h): ");
				scanf("%f", &time);

				if(time <= 0)
					fprintf(stderr, "Cannot set time to less than 1s\n");
				else
					fprintf(goalsfile, "t %f\n", (time * 3600));

				break;

			case 3:
				printf("Set desired amount of calories burned: ");
				scanf("%d", &cal);

				if(cal <= 0)
					fprintf(stderr, "Cannot set calories to less than 1\n");
				else
					fprintf(goalsfile, "c %d\n", cal);

				break;

			default:
				fprintf(stderr, "Not a valid option given as goal: %d", goalNr);
				break;
		}
	}
	else
	{
		printf("%s\n", goal);
	}
	
	fclose(goalsfile);
	return 0;
}

char* formatDate(double seconds)
{
	size_t maxsize = 50;
	time_t timestamp = (time_t) seconds;
	
	char* date;
	date = (char*) malloc(maxsize);

	char format[10];
	strcpy(format, "%d.%m.%y");
	
	// break timestamp down to its parts and local timezone
	struct tm* local_time = localtime(&timestamp);

	// print time in a desired format
	// returns number of copied characters, when buffer is larger than the size of text, otherwise 0
	if(!strftime(date, maxsize, format, local_time))
	{
		fprintf(stderr, "Problem with converting time to formatted string\n");
		return NULL;
	}
	else
	{
		return date;
	}
}

char* formatTime(double seconds)
{
	size_t maxsize = 50;
	int sec = 0;
	int min = 0;
	int hours = 0;

	char* time;
	time = (char*) malloc(maxsize);

	min = seconds / 60;
	sec = seconds - ((int)min * 60);
	if(min > 59)
	{
		hours = min / 60;
		min = min % 60;
	} 
	
	// turn calculated numbers into a string and return the pointer to it.
	if(hours == 0 && sec > 0)
	{
		sprintf(time, "%dmin, %02dsec", min, sec);
	}
	else if(hours > 0 && sec == 0)
	{
		sprintf(time, "%dh, %02dmin", hours, min);
	}
	else if(seconds == 0)
	{
		sprintf(time, "0");
	}
	else
	{
		sprintf(time, "%d:%02d:%02d", hours, min, sec);
	}

	return time;
}

int* dataThisMonth(double tables[LINESINFILE][MAXFIELDS])
{
	// this will make an array of the indexes of the lines that are recorded this month
	size_t maxsize = 50;
	time_t todayTimestamp;
	time_t firstDayTimestamp;
	int i = 0;
	int j = 0;						// holds amount of items in the returned array
	int* result = (int*) malloc(LINESINFILE * sizeof(int));

	struct tm* today;
	struct tm* firstDay;

	// todays timestamp
  	time(&todayTimestamp);
	
	// init both so that firstDay could be modified
	today = localtime(&todayTimestamp);
	firstDay = gmtime(&todayTimestamp);

	// array of days in months, maybe ill need it
	int daysInMonth[12] = {31, (today->tm_year % 4 != 0) ? 28 : 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// set parameters for first day of current month
	firstDay->tm_year = today->tm_year;
	firstDay->tm_mon = today->tm_mon;
	firstDay->tm_mday = 1;
	firstDay->tm_hour = 0;
	firstDay->tm_min = 0;
	firstDay->tm_sec = 0;
	// roughly dailyght savings time
	firstDay->tm_isdst = (today->tm_year > 3 && today->tm_year < 11) ? 1 : -1;

	firstDayTimestamp = mktime(firstDay);
	if( firstDayTimestamp == -1 ) 
	{
		fprintf(stderr, "Error: unable to make time using mktime\n");
	} 
	else 
	{
		// see which data is from this month
		for (; i < LINESINFILE; i++)
		{
			if(tables[i][0] < todayTimestamp && tables[i][0] > firstDayTimestamp)
			{
				//printf("Data on line %d is in this month\n", i);
				result[j] = i;
				j++;
			}
		}
	}

	return result;
}