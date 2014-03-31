/* tmpnam.c $Header$ */
/**
 * tmpnam prints out an unique temporary filename.
 * <pre>
 * cc tmpnam.c -o tmpnam
 * </pre>
 * @author Chris Mottram
 * @version $Revision$
 */
#include <stdio.h>

/**
 * Revision Control System identifier.
 */
static char rcsid[] = "$Id$";

int main(int argc,char *argv[])
{
	char *ptr = NULL;
	int i;

	if(argc != 1)
	{
		fprintf(stderr,"tmpnam: tmpnam prints out an unique temporary filename.\n");
		exit(1);
	}
	ptr = tmpnam(NULL);
	fprintf(stdout,"%s",ptr);
	return 0;
}
/*
** $Log$
*/
