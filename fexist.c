/* fexist
** fexist <filename>
** Return 0 if file exists, else return 1.
** cc -o fexist fexist.c
** $Header: /home/cjm/cjm/c/misc/RCS/fexist.c,v 1.1 2010/02/08 16:48:52 cjm Exp $
*/
#include <stdio.h>

#define TRUE (1)

#define FALSE (0)

static int fexist(char *filename);

static char rcsid[] = "$Id: fexist.c,v 1.1 2010/02/08 16:48:52 cjm Exp $";

/**
 * Main program. 
 */
int main(int argc,char *argv[])
{
	char *filename = NULL;
	int retval;

	/* check and parse arguments */
	if(argc != 2)
	{
		fprintf(stderr,"fexist <filename>\n");
		fprintf(stderr,"Return 0 if the filename exists, else return 1.\n");
		return 2;
	}
	filename = argv[1];
	if(fexist(filename))
		return 0;
	else
		return 1;
}

/**
 * Return whether the specified filename exists or not.
 * @param filename A string representing the filename to test.
 * @return The routine returns TRUE if the filename exists, and FALSE if it does not exist. 
 */
static int fexist(char *filename)
{
	FILE *fptr = NULL;

	fptr = fopen(filename,"r");
	if(fptr == NULL )
		return FALSE;
	fclose(fptr);
	return TRUE;
}
/*
** $Log: fexist.c,v $
** Revision 1.1  2010/02/08 16:48:52  cjm
** Initial revision
**
*/
