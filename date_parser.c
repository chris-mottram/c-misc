/* date_parser.c
** Parse a date of the format "2007-05-03T07:38:48.099 UTC"
** cc date_parser.c -o date_parser
** $Header: /home/cjm/cjm/c/misc/RCS/date_parser.c,v 1.3 2010/01/22 14:33:07 cjm Exp $
*/

#include <stdio.h>
#include <string.h>
/**
 * Add more fields to struct tm (tm_tm_zone)
 */
/*#define _BSD_SOURCE*/
#include <time.h>

/**
 * Parse a date of the format "2007-05-03T07:38:48.099 UTC"
 * @version $Revision: 1.3 $
 * @author cjm
 */

/* has defines */
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif
/**
 * Timezone offset for 1 hour.
 */
#define TIMEZONE_OFFSET_HOUR (3600)
/**
 * Timezone offset for BST.
 */
#define TIMEZONE_OFFSET_BST  (TIMEZONE_OFFSET_HOUR)
/**
 * Timezone offset for HST (roughly!).
 */
#define TIMEZONE_OFFSET_HST  (-10*TIMEZONE_OFFSET_HOUR)

/* internal functions */
static void Help(void);
static int Parse_Date(char *time_string,int *time_secs);

/**
 * Main program
 */
int main(int argc, char *argv[])
{
	int time_secs;

	if(argc != 2)
	{
		Help();
		return 1;
	}
	if(!Parse_Date(argv[1],&time_secs))
		return 1;
	fprintf(stdout,"%d\n",time_secs);
	return 0;
}

static void Help(void)
{
	fprintf(stderr,"date_parser <date>\n");
	fprintf(stderr,"Parses a date of the form: 2007-05-03T07:38:48.099 UTC\n");
	fprintf(stderr,"Also parses a date of the form: 2007-05-03\n");
	fprintf(stderr,"Prints out results in seconds since 1970.\n");
}

/**
 * Parse a date of the form "2007-05-03T07:38:48.099 UTC" into number of seconds since 1970 (unix time)
 * Also now parses a date of the form "2007-05-03" into number of seconds since 1970 (unix time)
 * @param time_string The string.
 * @param time_secs The address of an integer to store the number of seconds.
 * @return The routine returns TRUE on success and FALSE on failure.
 * @see #TIMEZONE_OFFSET_BST
 * @see #TIMEZONE_OFFSET_HST
 */
static int Parse_Date(char *time_string,int *time_secs)
{
	struct tm time_data;
	int year,month,day,hours,minutes,retval;
	double seconds;
	char timezone_string[16];
	time_t time_in_secs;

	/* check parameters */
	if(time_string == NULL)
	{
		fprintf(stderr,"Parse_Date:time_string was NULL.\n");
		return FALSE;
	}
	if(time_secs == NULL)
	{
		fprintf(stderr,"Parse_Date:time_secs was NULL.\n");
		return FALSE;
	}
	/* parse time_string into fields */
	strcpy(timezone_string,"UTC");
	retval = sscanf(time_string,"%d-%d-%d T %d:%d:%lf %15s",&year,&month,&day,
			&hours,&minutes,&seconds,timezone_string);
	if((retval < 6)&&(retval != 3))
	{
		fprintf(stderr,"Parse_Date:Failed to parse '%s', only parsed %d fields: year=%d,month=%d,day=%d,"
			"hour=%d,minute=%d,second=%.2f,timezone_string=%s.\n",time_string,retval,year,month,day,
			hours,minutes,seconds,timezone_string);
		return FALSE;
	}
	/* construct tm */
	time_data.tm_year  = year-1900; /* years since 1900 */
	time_data.tm_mon = month-1; /* 0..11 */
	time_data.tm_mday  = day; /* 1..31 */
	if(retval >= 6)
	{
		time_data.tm_hour  = hours; /* 0..23 */
		time_data.tm_min   = minutes;
		time_data.tm_sec   = seconds;
	}
	else
	{
		time_data.tm_hour  = 0;
		time_data.tm_min   = 0;
		time_data.tm_sec   = 0;
	}
	time_data.tm_wday  = 0;
	time_data.tm_yday  = 0;
	time_data.tm_isdst = 0;
	/* BSD extension stuff */
	/*
	time_data.tm_gmtoff = 0;
	time_data.tm_zone = strdup(timezone_string);
	*/
	/* create time in UTC */
	time_in_secs = mktime(&time_data);
	if(time_in_secs < 0)
	{
		fprintf(stderr,"Parse_Date:mktime failed.\n");
		return FALSE;
	}
	(*time_secs) = (int)time_in_secs;
	if(strcmp(timezone_string,"UTC") == 0)
	{
		/* do nothing */
		(*time_secs) = (*time_secs);
	}
	else if(strcmp(timezone_string,"GMT") == 0)
	{
		/* do nothing */
		(*time_secs) = (*time_secs);
	}
	else if(strcmp(timezone_string,"BST") == 0)
	{
		/* do nothing */
		(*time_secs) += TIMEZONE_OFFSET_BST;
	}
	else if(strcmp(timezone_string,"HST") == 0)
	{
		/* do nothing */
		(*time_secs) += TIMEZONE_OFFSET_HST;
	}
	else
	{
		fprintf(stderr,"Parse_Date: Unknown timezone '%s'.\n",timezone_string);
		return FALSE;
	}
	return TRUE;
}

/*
** $Log: date_parser.c,v $
** Revision 1.3  2010/01/22 14:33:07  cjm
** Various small changes.
**
** Revision 1.2  2010/01/20 12:06:45  cjm
** Fixed month parsing.
**
** Revision 1.1  2007/05/03 17:56:02  cjm
** Initial revision
**
*/
