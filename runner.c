#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "util.h"
#include "usb/runner_hid.h"

typedef struct goal
{
	char* name;
	double size;
	int nrData;
}goal;

// https://www.youtube.com/watch?v=tonwdcHvjVY
#define MAXFIELDS 20 		// this will stay static based on data gotten from watch
int LINESINFILE;			// global variable for number of lines in data 

int getAllRecords(double tables[LINESINFILE][MAXFIELDS], char* datafilename, int* actualfields);
int parse(char* line, char* delimiter, double table[MAXFIELDS], int* actualfields, int recordnr);
double getRecordSum(double tables[LINESINFILE][MAXFIELDS], int* records, int field);
double* getRecordsThisMonth(double tables[LINESINFILE][MAXFIELDS], int* records, int field);
int printAll(double tables[LINESINFILE][MAXFIELDS], int records, int* rows);
char* getFullWord(char* letter);
int showGoals(char* goalsfilename, double tables[LINESINFILE][MAXFIELDS], int* recordsThisMonth);
int setGoal(char* goalsfilename, goal* newGoal);
char* formatDate(double seconds);
char* formatTime(double seconds);
void printMonthInfo();
int* dataThisMonth(double tables[LINESINFILE][MAXFIELDS]);
int countLines(char* filename);

int main (int argc, char** argv)
{
	int c;                              		// holds options
	int flag = 0;                   		    // holds selected features
	int actualfields[LINESINFILE];				// holds actual fields count for record
	char *additionalArguments;					// holds additional arguments passed in
	LINESINFILE = countLines("data");			// count lines in data file
	
	int *testarr;
	int testint;

	// data tables
	// https://www.youtube.com/watch?v=_j5lhHWkbnQ
	double tables[LINESINFILE][MAXFIELDS];
	char* datafile = "data";

	char* test; // just for testing and holding returned pointer

	goal newGoal = {.size = 0};

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
				
				if(argc > 2)
				{	// convert to lowercase for easier processing
					for(int i = 0; argv[2][i]; i++){
  						argv[2][i] = tolower(argv[2][i]);
					}
					newGoal.name = argv[2];
				}
				else
				{
					newGoal.name = NULL;
				}

				if(argc > 3)
					newGoal.size = atof(argv[3]);
				
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
		testarr = dataThisMonth(tables);
		if(showGoals("goals", tables, testarr))
		{
			fprintf(stderr, "Couldn't locate file %s\n", "goals");
		}
	}

	if(flag & 0x08)
	{
		if(setGoal("goals", &newGoal))
		{
			fprintf(stderr, "Couldn't set goal(s)\n");
		}
	}

    return 0;
}

int countLines(char* filename)
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

	printf("|   date   |  start   | duration | distance|  avg hr |  max hr |  min hr | calories|   fat % | avg pace| max pace|"
	  "  index  | max alt |  ascent | descent |   zone1  |   zone2  |   zone3  |   zone4  |   zone5  |\n"
	  "------------------------------------------------------------------------------------------------------------------------"
	  "-----------------------------------------------------------------------------------------  ");

	for (i = 0; i < records; i++)
	{
		for (j = 0; j < rows[i]; j++)
		{	
			// This correctly formats the date and shows an understandable value
			if(j == 0)
			{
				time = formatDate(tables[i][j]);
				printf("|%9s | ", time);
				free(time);
			}
			else if(j <= 2 || j >= 15)
			{
				time = formatTime(tables[i][j]);
				printf("%8s | ", time);
				free(time);
			}
			else
			{
				if(tables[i][j] == (int)tables[i][j])
					printf("%7.0lf | ", tables[i][j]);
				else
					printf("%7.2lf | ", tables[i][j]);
			}
		}
		printf("\n");
	}

	return 0;
}

// getFieldByName(char* field)

char* getFullWord(char* letter)
{
	// THIS STRING WILL HAVE TO BE FREED OUTSIDE
	char* result;
	switch (*letter)
	{
		case 'd':
			result = (char*) malloc(sizeof(char) * strlen("Distance(km)") + 1);
			strcpy(result, "Distance(km)");
			break;
		case 't':
			result = (char*) malloc(sizeof(char) * strlen("Time(h)") + 1);
			strcpy(result, "Time(h)");
			break;
		case 'c':
			result = (char*) malloc(sizeof(char) * strlen("Calories") + 1);
			strcpy(result, "Calories");
			break;
		default:
			result = NULL;
			break;
	}
	return result;
}

int showGoals(char* goalsfilename, double tables[LINESINFILE][MAXFIELDS], int* recordsThisMonth)
{
	FILE* goalsfile;
	goalsfile = fopen(goalsfilename, "r");
	char* tokens;
	char buffer[50];
	goal dist = { .name = NULL, .size = 0, .nrData = 3 };
	goal time = { .name = NULL, .size = 0, .nrData = 2 };
	goal cal = { .name = NULL, .size = 0, .nrData = 7 };
	float distanceTotal = 0;
	float timeTotal = 0;
	float caloriesTotal = 0;
	int i = 0;

	// make sure file exists
	if(goalsfile == NULL)
		return 1;

	while (fgets(buffer, sizeof(buffer), goalsfile) != 0)
	{
		tokens = strtok(buffer, " ");
		switch(*tokens)
		{
			case 'd':
				dist.name = (char*) malloc(sizeof(char) * 2);
				strcpy(dist.name, tokens);
				tokens = strtok(NULL, " ");
				dist.size = atof(tokens);
				break;
			case 't':
				time.name = (char*) malloc(sizeof(char) * 2);
				strcpy(time.name, tokens);
				tokens = strtok(NULL, " ");
				time.size = atof(tokens);
				//printf("%s\n", formatTime());
				break;
			case 'c':
				cal.name = (char*) malloc(sizeof(char) * 2);
				strcpy(cal.name, tokens);
				tokens = strtok(NULL, " ");
				cal.size = atof(tokens);
				break;
		}
	}

	// print only goals if no data from this month
	if(recordsThisMonth != NULL)
	{
		// loop and sum all points this month
		for(; i < recordsThisMonth[0]; i++)
		{
			distanceTotal += tables[recordsThisMonth[i]][dist.nrData];
			timeTotal += tables[recordsThisMonth[i]][time.nrData];
			caloriesTotal += tables[recordsThisMonth[i]][cal.nrData];
		}
	}

	// printing header for table
	printf("|----------------------------------------------|\n|   Goal   |    size    |    done    | %% done  |\n|==============================================|\n");

	if(dist.size > 0)
	{
		printf("| Distance | %8.02fkm | %8.02fkm | %6.0f% |\n", dist.size, distanceTotal, ((distanceTotal)/dist.size)*100);
		printf("|----------------------------------------------|\n");
		//printf("Distance goal: %.02fkm, of which %.02fkm(%.0f%) done.\n", dist.size, distanceTotal, ((distanceTotal)/dist.size)*100);
	}
	
	if(time.size > 0)
	{
		printf("|   Time   |  %8s  |  %8s  | %6.0f% |\n", formatTime(time.size), formatTime(timeTotal), ((timeTotal)/time.size)*100);
		printf("|----------------------------------------------|\n");
		//printf("Time goal: %s, of which %s(%.0f%) done.\n", formatTime(time.size), formatTime(timeTotal), ((timeTotal)/time.size)*100);
	}

	if(cal.size > 0)
	{
		printf("| Calories | %7.02fcal | %7.02fcal | %6.0f% |\n", cal.size, caloriesTotal, ((caloriesTotal)/cal.size)*100);
		printf("|----------------------------------------------|\n");
		//printf("Calories goal: %.0fcal, of which %.0fcal(%.0f%) done.\n", cal.size, caloriesTotal, ((caloriesTotal)/cal.size)*100);
	}
	
	printMonthInfo();

	// print days left
	//printf()

	free(dist.name);
	free(time.name);
	free(cal.name);

	fclose(goalsfile);
	return 0;
}

int setGoal(char* goalsfilename, goal* newGoal)
{	
	int goalNr = 0;

	FILE* goalsfile;
	goalsfile = fopen(goalsfilename, "a+");

	// Setting name for goal
	if(newGoal->name == NULL)
	{
		//printf("No arguments given, setting in interctive mode\n");
		newGoal->name = (char*) malloc(sizeof(char) * 2);
		do
		{
			printf("Enter number for the goal you want to set:\nDistance[1], Time[2], Calories[3]: ");
			scanf("%d", &goalNr);
		}
		while (goalNr > 3 || goalNr < 1);

		switch (goalNr)
		{
			case 1:
				strcpy(newGoal->name, "d");
				break;

			case 2:
				strcpy(newGoal->name, "t");
				break;

			case 3:
				strcpy(newGoal->name, "c");
				break;

			default:
				fprintf(stderr, "Not a valid option given as goal: %d", goalNr);
				break;
		}
	}
	
	// Setting size for goal
	if (newGoal->size <= 0)
	{
		char* temp = getFullWord(newGoal->name);
		printf("Set desired goal for %s: ", temp);
		free(temp);
		scanf("%lf", &(newGoal->size));
	}

	// write goal to file, time needs calculation
	fprintf(goalsfile, "%s %.02f\n", newGoal->name, (strcmp(newGoal->name, "t") ? newGoal->size : newGoal->size * 3600));

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
		sprintf(time, "%d:%02d", min, sec);
	}
	else if(hours > 0 && sec == 0)
	{
		sprintf(time, "%d:%02d:00", hours, min);
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

void printMonthInfo()
{
	size_t maxsize = 11;
	char date[maxsize];
 	time_t todayTimestamp;
	struct tm* today;

	time(&todayTimestamp);
	today = localtime(&todayTimestamp);

	// array of days in months, maybe ill need it
	int daysInMonth[12] = {31, (today->tm_year % 4 != 0) ? 28 : 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	char format[10];
	strcpy(format, "%B %Y");

	if(!strftime(date, maxsize, format, today))
	{
		fprintf(stderr, "Problem with converting time to formatted string\n");
	}

	float daysLeft = daysInMonth[today->tm_mon] - today->tm_mday;
	printf("\n%s, %.0f days left(%.0f%% done)\n", date, daysLeft, (today->tm_mday / (float)daysInMonth[today->tm_mon])*100);
}

int* dataThisMonth(double tables[LINESINFILE][MAXFIELDS])
{
	// this will make an array of the indexes of the lines that are recorded this month
	size_t maxsize = 50;
	time_t todayTimestamp;
	time_t firstDayTimestamp;
	int i = 0;
	int j = 0;						// holds amount of items in the returned array
	int temp[LINESINFILE];
	int* result = NULL;

	struct tm* today;
	struct tm* firstDay;

	// todays timestamp
  	time(&todayTimestamp);
	
	// init both so that firstDay could be modified
	today = localtime(&todayTimestamp);
	firstDay = gmtime(&todayTimestamp);

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
				temp[j] = i;
				j++;
			}
		}
	}

	if (j > 0)
	{
		result = (int*) malloc((j+1) * sizeof(int));
		result[0] = j; // storing the element count as first element so it can be read in another function easily
		for (i = 1; i < j+1; i++)
		{
			result[i] = temp[i-1];
		}
	}
	else
	{
		return NULL;
	}

	//for (i=0; i<j+1; i++)
	//	printf("element %d: %d\n", i, result[i]);

	return result;
}