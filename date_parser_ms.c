/* date_parser_ms.c
** Parse a date of the format "2007-05-03T07:38:48.099 +0000"
** cc date_parser_ms.c -o date_parser_ms
** $Header: /home/cjm/cjm/c/misc/RCS/date_parser_ms.c,v 1.3 2010/01/22 14:33:07 cjm Exp $
*/

#include <stdio.h>
#include <string.h>
/**
 * Add more fields to struct tm (tm_tm_zone)
 */
/*#define _BSD_SOURCE*/
#include <time.h>

/**
 * Parse a date of the format "2007-05-03T07:38:48.099 +0000"
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
 * Millisecond to second conversion.
 */
#define SECONDS_TO_MILLISECONDS (1000)
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
static int Parse_Date(char *time_string,long *time_msecs);

/**
 * Main program
 */
int main(int argc, char *argv[])
{
	long time_msecs;

	if(argc != 2)
	{
		Help();
		return 1;
	}
	if(!Parse_Date(argv[1],&time_msecs))
		return 1;
	fprintf(stdout,"%ld\n",time_msecs);
	return 0;
}

static void Help(void)
{
	fprintf(stderr,"date_parser_ms <date>\n");
	fprintf(stderr,"Parses a date of the form: 2007-05-03T07:38:48.099 +0000\n");
	fprintf(stderr,"Prints out results in milliseconds since 1970.\n");
}

/**
 * Parse a date of the form "2007-05-03T07:38:48.099 +0000" into number of milliseconds since 1970
 * @param time_string The string.
 * @param time_msecs The address of an integer to store the number of milliseconds.
 * @return The routine returns TRUE on success and FALSE on failure.
 * @see #SECONDS_TO_MILLISECONDS
 * @see #TIMEZONE_OFFSET_BST
 * @see #TIMEZONE_OFFSET_HST
 */
static int Parse_Date(char *time_string,long *time_msecs)
{
	struct tm time_data;
	int year,month,day,hours,minutes,seconds,milliseconds,retval;
	char timezone_string[16];
	time_t time_in_secs;

	/* check parameters */
	if(time_string == NULL)
	{
		fprintf(stderr,"Parse_Date:time_string was NULL.\n");
		return FALSE;
	}
	if(time_msecs == NULL)
	{
		fprintf(stderr,"Parse_Date:time_msecs was NULL.\n");
		return FALSE;
	}
	/* parse time_string into fields */
	strcpy(timezone_string,"UTC");
	retval = sscanf(time_string,"%d-%d-%d T %d:%d:%d.%d %15s",&year,&month,&day,
			&hours,&minutes,&seconds,&milliseconds,timezone_string);
	if(retval < 7)
	{
		fprintf(stderr,"Parse_Date:Failed to parse '%s', only parsed %d fields: year=%d,month=%d,day=%d,"
			"hour=%d,minute=%d,seconds=%d,milliseconds=%d,timezone_string=%s.\n",time_string,retval,
			year,month,day,hours,minutes,seconds,milliseconds,timezone_string);
		return FALSE;
	}
	/* construct tm */
	time_data.tm_year  = year-1900; /* years since 1900 */
	time_data.tm_mon = month-1; /* 0..11 */
	time_data.tm_mday  = day; /* 1..31 */
	time_data.tm_hour  = hours; /* 0..23 */
	time_data.tm_min   = minutes;
	time_data.tm_sec   = seconds;
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
	(*time_msecs) = (long)((time_in_secs*SECONDS_TO_MILLISECONDS)+milliseconds);
	if(strcmp(timezone_string,"UTC") == 0)
	{
		/* do nothing */
		(*time_msecs) = (*time_msecs);
	}
	else if(strcmp(timezone_string,"GMT") == 0)
	{
		/* do nothing */
		(*time_msecs) = (*time_msecs);
	}
	else if(strcmp(timezone_string,"+0000") == 0)
	{
		/* do nothing */
		(*time_msecs) = (*time_msecs);
	}
	else if(strcmp(timezone_string,"BST") == 0)
	{
		/* do nothing */
		(*time_msecs) += TIMEZONE_OFFSET_BST*SECONDS_TO_MILLISECONDS;
	}
	else if(strcmp(timezone_string,"HST") == 0)
	{
		/* do nothing */
		(*time_msecs) += TIMEZONE_OFFSET_HST*SECONDS_TO_MILLISECONDS;
	}
	else
	{
		fprintf(stderr,"Parse_Date: Unknown timezone '%s'.\n",timezone_string);
		return FALSE;
	}
	return TRUE;
}

/*
** $Log$
*/
