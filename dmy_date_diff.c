/* dmy_date_diff.c
** Print the difference in milliseconds between 2 dates of the form "2007-05-03T07:38:48.099 UTC"
** cc dmy_date_diff.c -o dmy_date_diff
** $Header: /home/cjm/cjm/c/misc/RCS/dmy_date_diff.c,v 1.1 2010/01/28 14:49:43 cjm Exp $
*/

#include <stdio.h>
#include <string.h>
/**
 * Add more fields to struct tm (tm_tm_zone)
 */
/*#define _BSD_SOURCE*/
#include <time.h>
#include <math.h>

/**
 * Parse a date of the format "22-01-2010T07:38:48.099" and print out the result in milliseconds.
 * Note the imput format is different from parse_date.c.
 * @version $Revision: 1.1 $
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
/**
 * The number of nanoseconds in one second. A struct timespec has fields in nanoseconds.
 */
#define ONE_SECOND_NS	(1000000000)
/**
 * The number of nanoseconds in one millisecond. A struct timespec has fields in nanoseconds.
 */
#define ONE_MILLISECOND_NS	(1000000)
/**
 * The number of milliseconds in one second.
 */
#define ONE_SECOND_MS	(1000)

/* internal functions */
static void Help(void);
static int Parse_DMY_Date(char *time_string,time_t *time_secs,int *time_ns);

/**
 * Main program.
 * @see #Parse_DMY_Date
 */
int main(int argc, char *argv[])
{
	time_t time_secs;
	double diff_time,time_ms1,time_ms2,dt1,dt2;
	int time_ns;

	if(argc != 3)
	{
		Help();
		return 1;
	}
	/* first date */
	if(!Parse_DMY_Date(argv[1],&time_secs,&time_ns))
		return 1;
	dt1 = (((double)time_secs)*((double)ONE_SECOND_MS));
	dt2 = (((double)time_ns)/((double)ONE_MILLISECOND_NS));
	time_ms1 = dt1+dt2;
	/* second date */
	if(!Parse_DMY_Date(argv[2],&time_secs,&time_ns))
		return 2;
	dt1 = (((double)time_secs)*((double)ONE_SECOND_MS));
	dt2 = (((double)time_ns)/((double)ONE_MILLISECOND_NS));
	time_ms2 = dt1+dt2;
	/* difference */
	diff_time = time_ms2-time_ms1;
	fprintf(stdout,"%.0f\n",diff_time);
	return 0;
}

static void Help(void)
{
	fprintf(stderr,"dmy_date_diff <date> <date>\n");
	fprintf(stderr,"Parses dates of the form: 22-01-2010T07:38:48.099\n");
	fprintf(stderr,"Prints out difference in milliseconds since 1970.\n");
}

/**
 * Parse a date of the form "22-01-2010T07:38:48.099" into number of seconds and nanoseconds since 1970 (unix time)
 * @param time_string The string.
 * @param time_secs The address of an integer to store the number of seconds.
 * @param time_ns The address of an integer to store the number of nanoseconds.
 * @return The routine returns TRUE on success and FALSE on failure.
 * @see #TIMEZONE_OFFSET_BST
 * @see #TIMEZONE_OFFSET_HST
 * @see #ONE_SECOND_NS
 */
static int Parse_DMY_Date(char *time_string,time_t *time_secs,int *time_ns)
{
	struct tm time_data;
	int year,month,day,hours,minutes,retval;
	double seconds,integral_seconds,fractional_seconds;
	char timezone_string[16];
	time_t time_in_secs;

	/* check parameters */
	if(time_string == NULL)
	{
		fprintf(stderr,"Parse_DMY_Date:time_string was NULL.\n");
		return FALSE;
	}
	if(time_secs == NULL)
	{
		fprintf(stderr,"Parse_DMY_Date:time_secs was NULL.\n");
		return FALSE;
	}
	if(time_ns == NULL)
	{
		fprintf(stderr,"Parse_DMY_Date:time_ns was NULL.\n");
		return FALSE;
	}
	/* parse time_string into fields */
	strcpy(timezone_string,"UTC");
	retval = sscanf(time_string,"%d-%d-%d T %d:%d:%lf %15s",&day,&month,&year,
			&hours,&minutes,&seconds,timezone_string);
	if(retval < 6)
	{
		fprintf(stderr,"Parse_DMY_Date:Failed to parse '%s', only parsed %d fields: day=%d,month=%d,year=%d,"
			"hour=%d,minute=%d,second=%.2f,timezone_string=%s.\n",time_string,retval,year,month,day,
			hours,minutes,seconds,timezone_string);
		return FALSE;
	}
	/* construct tm */
	time_data.tm_year  = year-1900; /* years since 1900 */
	time_data.tm_mon = month-1; /* 0..11 */
	time_data.tm_mday  = day; /* 1..31 */
	time_data.tm_hour  = hours; /* 0..23 */
	time_data.tm_min   = minutes;
	time_data.tm_sec   = (int)seconds;
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
		fprintf(stderr,"Parse_DMY_Date:mktime failed.\n");
		return FALSE;
	}
	(*time_secs) = time_in_secs;
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
		fprintf(stderr,"Parse_DMY_Date: Unknown timezone '%s'.\n",timezone_string);
		return FALSE;
	}
	/* convert fractional part of secs into ns */
	fractional_seconds = modf(seconds,&integral_seconds);
	/*fprintf(stdout,"seconds=%.2f,integral_seconds=%.2f,fraction_seconds=%.3f.\n",seconds,integral_seconds,
		fractional_seconds);*/
	(*time_ns) = (int)(fractional_seconds*((double)ONE_SECOND_NS));
	/*fprintf(stdout,"time_ns=%d.\n",(*time_ns));*/
	return TRUE;
}

/*
** $Log: dmy_date_diff.c,v $
** Revision 1.1  2010/01/28 14:49:43  cjm
** Initial revision
**
** Revision 1.1  2010/01/27 12:08:27  cjm
** Initial revision
**
*/
