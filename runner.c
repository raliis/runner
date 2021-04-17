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

void help();
int getAllRecords(double* tables, char* datafilename, int* actualfields);
int parse(char* line, char* delimiter, double* table, int* actualfields, int recordnr);
//double getRecordSum(double* tables, int* records, int field);
//double* getRecordsThisMonth(double* tables, int* records, int field);
int printAll(double* tables, int records, int* rows);
char* getFullWord(char* letter);
int showGoals(char* goalsfilename, double* tables, int* recordsThisMonth);
int setGoal(char* goalsfilename, goal* newGoal);
char* formatDate(double seconds);
char* formatTime(double seconds);
void printMonthInfo();
int* dataThisMonth(double* tables);
int countLines(char* filename);

int main (int argc, char** argv)
{
	int c;                              		// holds options
	int flag = 0;                   		    // holds selected features

	char* datafile = (char*) malloc (strlen(getenv("HOME")) + strlen(".config/runner/data"));
	char* goalsfile = (char*) malloc (strlen(getenv("HOME")) + strlen(".config/runner/goals"));
	sprintf(datafile, "%s/.config/runner/data", getenv("HOME"));
	sprintf(goalsfile, "%s/.config/runner/goals", getenv("HOME"));

	// data tables
	// https://www.youtube.com/watch?v=_j5lhHWkbnQ
	double* tables = NULL;

	// count lines in data file
	LINESINFILE = countLines(datafile);										

	if (LINESINFILE > 0)
		tables = (double*) malloc(LINESINFILE * MAXFIELDS * sizeof(int));

	// holds actual fields count for record
	int actualfields[(LINESINFILE > 0) ? LINESINFILE : 1];					

	// struct for goals
	goal newGoal = {.size = 0};

	// get parameters
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
	while((c = getopt(argc, argv, "adghn")) != -1) 
	{
		switch (c)
		{
			// show all info
			case 'a':
				flag |= 0x01;
				if (tables == NULL)
				{	
					fprintf(stderr, "No data present, exiting\n");
					return 1;
				}
				break;

			// get data from watch
			case 'd':
				flag |= 0x02;
				break;

			// show goals
			case 'g':
				flag |= 0x04;
				if (tables == NULL)
				{	
					fprintf(stderr, "No data present, exiting\n");
					return 1;
				}
				// THIS WILL JUST SHOW THE GOALS WITHOUT SUMS AND %
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

			case 'h':
				help();
				break;
			
			default:
				help();
				break;
		}
	}

	// special case for without flags
	if (argc < 2)
	{
		help();
		return 0;
	}
		
	if(flag & 0x01)
	{
		if(getAllRecords(tables, datafile, actualfields))
		{
			fprintf(stderr, "Reading from file failed, exiting\n");
			return 1;
		}

		printf("\nPrinting all data returned: %d\n", 
			printAll(tables, LINESINFILE, actualfields));
	}

	if(flag & 0x04)
	{
		if(getAllRecords(tables, datafile, actualfields))
		{
			fprintf(stderr, "Reading from file failed, exiting\n");
			return 1;
		}

		if(showGoals(goalsfile, tables, dataThisMonth(tables)))
		{
			//errors handled inside function
			return 1;
		}
	}

	if(flag & 0x08)
	{
		/*if(getAllRecords(tables, datafile, actualfields))
		{
			fprintf(stderr, "Reading from file failed, exiting\n");
			return 1;
		}*/

		if(setGoal(goalsfile, &newGoal))
		{
			fprintf(stderr, "Couldn't set goal(s)\n");
		}
	}

	/*free(tables);
	free(datafile);
	free(goalsfile);*/

    return 0;
}

void help()
{
	printf("Usage:\n\t runner -[adg]\n"
	 		"\t runner -n [type of goal] [size of goal]\nOptions:\n"
			"\t-h\t\t show help\n"
			"\t-a\t\t show all data from file\n"
			"\t-d\t\t get data from watch - CURRENTLY UNAVAILABLE\n"
			"\t-g\t\t show set goals\n"
			"\t-n\t\t add new goal, optional parameters are new goal and size of goal\n");

	printf("Examples:\n\trunner -a\t\t shows all data\n"
			"\trunner -g\t\t shows goals\n"
			"\trunner -n\t\t starts interactive goal adding\n"
			"\trunner -n distance\t asks for size of goal and sets it\n");
}

int countLines(char* filename)
{
	int lines = 0;
	char c;

	FILE* pfile;
	pfile = fopen(filename, "r");
	if(pfile == NULL)
	{
		fprintf(stderr, "Couldn't open file: %s\n", filename);
		return -1;
	}

	while((c=fgetc(pfile))!=EOF) {
      if(c == '\n')
         lines++;
   	}
	fclose(pfile);

	return lines;
}

int getAllRecords(double* tables, char* datafilename, int* actualfields)
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
	
	if (tables == NULL)
	{
		fprintf(stderr, "No data in file: %s\n", datafilename);
		return 1;
	}

	//https://solarianprogrammer.com/2019/04/03/c-programming-read-file-lines-fgets-getline-implement-portable-getline/
	while (fgets(buffer, sizeof(buffer), datafile) != 0)
	{
		if(parse(buffer, ",", (tables + (record * MAXFIELDS)), actualfields, record))
		{
			fprintf(stderr, "Something went wrong with parse\n");
			return 2;
		}
		record++;
	}
	
	//fclose(datafile);
	return 0;
}

// https://www.daniweb.com/programming/software-development/threads/97843/parsing-a-csv-file-in-c
int parse(char* line, char* delimiter, double* table, int* fieldcount, int recordnr)
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
	fieldcount[recordnr] = field;

	return 0;
}

/*double getRecordSum(double* tables, int* records, int field)
{
	int lines = 0; //sizeof(records)/sizeof(records[0]);
	int* p = records;
	while (p != NULL)
	{
		lines++;
		p=p+1;
	}

	printf("%d\n", lines);

	double result;
	int i = 0;

	for(; i < lines; i++)
	{
		result += tables[i][field];
	}

	return result;
}*/

/*double* getRecordsThisMonth(double* tables, int* records, int field)
{
	//int lines = sizeof(records)/sizeof(records[0]);

	int lines = 0;
	int* p = records;
	while (p != NULL)
	{
		lines++;
		p=p+1;
	}

	double* result;
	result = (double*) malloc(sizeof(double) * lines); 
	int i = 0;

	for(; i < lines; i++)
	{
		result[i] = tables[i][field];
	}

	return result;
}*/
			//      (tables, LINESINFILE, actualfields)
int printAll(double* tables, int records, int* rows)
{
	int i, j;
	char* time;

	printf("|   date   |  start   | duration | distance|  avg hr |  max hr |  min hr | calories|   fat % | avg pace| max pace|"
	  "  index  | max alt |  ascent | descent |   zone1  |   zone2  |   zone3  |   zone4  |   zone5  |\n"
	  "------------------------------------------------------------------------------------------------------------------------"
	  "-----------------------------------------------------------------------------------------\n");

	for (i = 0; i < records; i++)
	{
		for (j = 0; j < MAXFIELDS; j++)
		{	
			// This correctly formats the date and shows an understandable value
			if(j == 0)
			{
				time = formatDate(*(tables + i*MAXFIELDS + j));
				printf("|%9s | ", time);
				free(time);
			}
			else if(j <= 2 || j >= 15)
			{
				time = formatTime(*(tables + i*MAXFIELDS + j));
				printf("%8s | ", time);
				free(time);
			}
			else
			{
				if(*(tables + i*MAXFIELDS + j) == (int) *(tables + i*MAXFIELDS + j))
					printf("%7.0lf | ", *(tables + i*MAXFIELDS + j));
				else
					printf("%7.2lf | ", *(tables + i*MAXFIELDS + j));
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

int showGoals(char* goalsfilename, double* tables, int* recordsThisMonth)
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
	int i;

	// make sure file exists
	if(goalsfile == NULL)
	{
		fprintf(stderr, "Couldn't open file %s\n", goalsfilename);
		return 1;
	}

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
		for(i = 1; i <= recordsThisMonth[0]; i++)
		{	
			distanceTotal += tables[recordsThisMonth[i]*MAXFIELDS + dist.nrData];
			timeTotal += tables[recordsThisMonth[i]*MAXFIELDS + time.nrData];
			caloriesTotal += tables[recordsThisMonth[i]*MAXFIELDS + cal.nrData];
		}
	}
	else
	{
		fprintf(stderr, "Found no data for this month, only showing goals\n");
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

	free(dist.name);
	free(time.name);
	free(cal.name);

	fclose(goalsfile);
	return 0;
}

int setGoal(char* goalsfilename, goal* newGoal)
{	
	int goalNr = 0;
	char* temp = NULL;

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
	}
	else
	{
		goalNr = newGoal->name[0];
	}
	
	switch (goalNr)
	{
		case 1:
		case 'd':
			strcpy(newGoal->name, "d");
			break;

		case 2:
		case 't':
			strcpy(newGoal->name, "t");
			break;

		case 3:
		case 'c':
			strcpy(newGoal->name, "c");
			break;

		default:
			fprintf(stderr, "Not a valid option given as goal\n", goalNr);
			break;
	}
	
	
	temp = getFullWord(newGoal->name);
	// Setting size for goal
	if (newGoal->size <= 0)
	{
		printf("Set desired goal for %s: ", temp);
		scanf("%lf", &(newGoal->size));
	}
	else
	{
		printf("Setting goal %s\n", temp);
	}
	free(temp);

	
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

int* dataThisMonth(double* tables)
{
	// this will make an array of the indexes of the lines that are recorded this month
	time_t todayTimestamp;
	time_t firstDayTimestamp;
	int i;
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
		for (i = 0; i < LINESINFILE; i++)
		{
			//printf("%lf %ld %ld\n", tables[i * MAXFIELDS], todayTimestamp, firstDayTimestamp);
			if(tables[i * MAXFIELDS] < todayTimestamp && tables[i * MAXFIELDS] > firstDayTimestamp)
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

	return result;
}