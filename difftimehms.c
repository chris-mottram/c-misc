/* difftimehms.c
** Calculate the difference in time between two "dates" i.e.
** difftimehms 20:50:54.871  20:52:47.763 
** cc difftimehms.c -o difftimehms
** $Header: /home/cjm/cjm/c/misc/RCS/difftimehms.c,v 1.1 2007/10/08 18:03:30 cjm Exp $
*/

#include <stdio.h>
#include <string.h>

/**
 * Calculate the difference in time between two "dates" i.e.
 * difftimehms 20:50:54.871  20:52:47.763 
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

/* internal functions */
static void Help(void);
static int Parse_Time(char *time_string,double *time_secs);

/**
 * Main program
 */
int main(int argc, char *argv[])
{
	double start_time_secs;
	double end_time_secs;
	double diff_time_secs;

	if(argc != 3)
	{
		Help();
		return 1;
	}
	if(!Parse_Time(argv[1],&start_time_secs))
		return 1;
	fprintf(stderr,"Start Time secs = %.2f.\n",start_time_secs);
	if(!Parse_Time(argv[2],&end_time_secs))
		return 1;
	fprintf(stderr,"End Time secs = %.2f.\n",end_time_secs);
	if(start_time_secs > end_time_secs)
	{
		fprintf(stderr,"Start Time secs > End Time secs, perhaps a day overlap, add a day to end timesecs.\n");
		end_time_secs += 86400;
	}
	diff_time_secs = end_time_secs-start_time_secs;
	fprintf(stdout,"%.2f\n",diff_time_secs);
	return 0;
}

static void Help(void)
{
	fprintf(stderr,"difftimehms <HH:MM:SS.sss> <HH:MM:SS.sss> \n");
	fprintf(stderr,"Prints out difference between start and end date in decimal seconds.\n");
	fprintf(stderr,"If start date > end date, assumes end date if for day after and adds a days worth of seconds.\n");
}

/**
 * Parse a time of the form "07:38:48.099 UTC" into number of seconds.
 * @param time_string The string.
 * @param time_secs The address of an double to store the number of seconds.
 * @return The routine returns TRUE on success and FALSE on failure.
 * @see #TIMEZONE_OFFSET_BST
 * @see #TIMEZONE_OFFSET_HST
 */
static int Parse_Time(char *time_string,double *time_secs)
{
	int hours,minutes,retval;
	double seconds;

	/* check parameters */
	if(time_string == NULL)
	{
		fprintf(stderr,"Parse_Time:time_string was NULL.\n");
		return FALSE;
	}
	if(time_secs == NULL)
	{
		fprintf(stderr,"Parse_Time:time_secs was NULL.\n");
		return FALSE;
	}
	/* parse time_string into fields */
	retval = sscanf(time_string,"%d:%d:%lf",&hours,&minutes,&seconds);
	if(retval != 3)
	{
		fprintf(stderr,"Parse_Time:Failed to parse '%s', only parsed %d fields: "
			"hour=%d,minute=%d,seconds=%.2f.\n",time_string,retval,hours,minutes,seconds);
		return FALSE;
	}
	(*time_secs) = (((double)hours)*3600.0)+(((double)minutes)*60.0)+seconds;
	return TRUE;
}

/*
** $Log: difftimehms.c,v $
** Revision 1.1  2007/10/08 18:03:30  cjm
** Initial revision
**
*/
