#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int inputError();

int main( int argc, const char* argv[] )
{
	/* Before any input processing, check the number of command
	 * line arguments given.  Return error if != 3*/
	if (argc != 3) { 
		printf("Error - incorrect number of arguments. \n\n");
		inputError();
		return -1;
	}
	
	time_t rawtime, eptime;				// rawtime stores our input time, eptime is epoch
	rawtime = time(NULL);				// initialize the value for rawtime
	struct tm * timeinfo;				// create a pointer to a tm structure
	struct tm epochtime = {0,0,0,1,0,70,0,0,0};		// epoch time stores epoch time 
	timeinfo = gmtime ( &rawtime );		// initialize the timeinfo with rawtime
	
	char *tempStr;						// a pointer to use in our loop
	char *inputStr[2] = {(char*) argv[1], (char*) argv[2]};		// create a pointer to our cmd arguments
	int inputAr[6];						// array for {year, month, day, hour, min, sec }
	int minMax[12] = {1900, 2038, 1, 12, 1, 31, 0, 23, 0, 59, 0, 59 };
	int i, count = 0;					// minMax is (min,max) pairs for each array value
	
	/* Now process the command arguments storing each value
	 * in our array sequentially.  */
	for (i = 0; i < 2; i++) 
	{
		tempStr = strtok(inputStr[i]," :-");
		inputAr[3 * i + count] = atoi(tempStr);
		while(tempStr != NULL && count < 2) {
			tempStr = strtok(NULL, " :-");
			count++;
			inputAr[3 * i + count] = atoi(tempStr);
		}
		/* Return an error if the argument does not produce three
		 * substrings from strtok function*/
		if (count != 2) {
			printf("Error - arguments not in proper format.\n\n");
			inputError();
		}
		count = 0;
	}
	/* Check each value against our array of min max values
	 * return an warning and suggest correct value
	 * program will not halt - produce output with values given.*/
	for (i = 0; i < 6; i++) {
		if (inputAr[i] < minMax[2*i] || inputAr[i] > minMax[2*i+1]) {
			printf("Warning - Input values out of range.\n");
			printf("Value should be %d < %d < %d\n\n",minMax[2*i],inputAr[i],minMax[2*i+1]);
        return -1;
		}
	}
	timeinfo->tm_year = inputAr[0] -1900;
	timeinfo->tm_mon = inputAr[1] - 1;
	timeinfo->tm_mday = inputAr[2];
	timeinfo->tm_hour = inputAr[3];
	timeinfo->tm_min = inputAr[4];
	timeinfo->tm_sec = inputAr[5];
	timeinfo->tm_isdst = 0;
	
	rawtime = mktime( timeinfo );
	eptime = mktime( &epochtime );
	printf("The time entered was: %s", ctime (&rawtime));
	printf("Which is Epoch time: %.0f\n",difftime(rawtime,eptime));
	return 0;
}

/* Function to display standard error message for all detected errors.*/
int inputError()
{
		printf("etime requires two command-line arguments;\n");
		printf("  date (yyyy-mm-dd) and time (hh:mm:ss)\n");
		printf("  example: etime 2011-07-01 10:30:15\n");
		printf("  only years 1901-2038 are accepted.\n");
		return -1; 
}
