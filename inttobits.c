/* inttobits
** inttobits <number>
** Print which bits are set in an int
** cc -o inttobits inttobits.c
*/
#include <stdio.h>

/**
 * The number of bits in an integer.
 */
#define BIT_COUNT (32)

/**
 * Main program. Parse argv[1] into an integer, and go through a loop through all the bits,
 * seeing which ones are set.
 * @see #BIT_COUNT
 */
int main(int argc,char *argv[])
{
	int retval,value,i;

	if(argc != 2)
	{
		fprintf(stderr,"inttobits <number>: No Number specified\n");
		exit(1);
	}
	/* value = atoi(argv[1]);*/
	retval = sscanf(argv[1],"%i",&value);
	if(retval != 1)
	{
		perror("inttobits");
		fprintf(stderr,"inttobits failed:%s was not a valid integer.\n",argv[1]);
		return 2;
	}
	for(i=0;i<BIT_COUNT;i++)
	{
		if(((1<<i)&value) > 0)
			fprintf(stdout,"Bit %d set ((1<<%d) = %d).\n",i,i,(1<<i));
	}
	return 0;
}
/*
** $Log: inttobits.c,v $
** Revision 1.2  2005/02/17 17:34:44  cjm
** Use sscanf for parsing. Now parses hex input values.
**
** Revision 1.1  2005/02/17 17:32:00  cjm
** Initial revision
**
*/
