/* delete_file.c $Header$ */
/**
 * delete_file delete a file, including '-help' and other files that cause rm to barf.
 * <p>Compilation:
 * <pre>
 * cc delete_file.c -o delete_file
 * </pre>
 * <p>Usage:
 * <pre>
 * delete_file -help
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
	int i,retval;

	if(argc < 2)
	{
		fprintf(stderr,"delete_file: delete_file <filename> [<filename> ....]\n");
		exit(1);
	}
	for(i=1;i<argc;i++)
	{
		retval = remove(argv[i]);
		if(retval == 0)
		{
			fprintf(stdout,"%s deleted.\n",argv[i]);
		}
		else
		{
			fprintf(stdout,"failed to delete %s.\n",argv[i]);
		}
	}
	return retval;
}
/*
** $Log$
*/
