/* glob_expand.c
** $Header: /home/cjm/cjm/c/misc/RCS/glob_expand.c,v 1.1 2008/03/12 16:36:33 cjm Exp $
*/
/**
 * glob_expand expands a set of glob expressions into a list of filenames that exist.
 * It tries to get round the /bin/ls error where the shell globbing is greater than the argument space.
 * To compile:
 * <pre>
 * cc glob_expand.c -o glob_expand
 * </pre>
 * To Run:
 * <pre>
 * glob_expand '/icc/tmp/q_e_*.fits' '/icc/tmp/q_e_*.fits.gz'
 * </pre>
 * You need to single quote the glob expressions to stop the shells globber expanding them.
 * Pipe the results into a file to bypass any shell argument length problems.
 * @author Chris Mottram
 * @version $Revision: 1.1 $
 */
#include <stdio.h>
#include <glob.h>

/**
 * Revision Control System identifier.
 */
static char rcsid[] = "$Id: glob_expand.c,v 1.1 2008/03/12 16:36:33 cjm Exp $";

/**
 * Main routine.
 */
int main(int argc,char *argv[])
{
	glob_t glob_list;
	int i,path_index,glob_expand_retval,glob_retval;

	if(argc < 2)
	{
		fprintf(stderr,"glob_expand: glob_expand exapnds filename globs.\n");
		fprintf(stderr,"glob_expand 'glob' ['glob']....\n");
		fprintf(stderr,"Single quote your globs to prevent shell expansion.\n");
		fprintf(stderr,"e.g. glob_expand '*.fits'.\n");
		exit(1);
	}
	glob_expand_retval=0;
	for(i=1;i<argc;i++)
	{
		fprintf(stderr,"glob_expand : Expanding glob %s.\n",argv[i]);
		glob_retval = glob(argv[i],GLOB_BRACE|GLOB_TILDE,NULL,&glob_list);
		if(glob_retval == 0)
		{
			for(path_index = 0;path_index < glob_list.gl_pathc; path_index++)
			{
				fprintf(stdout,"%s\n",glob_list.gl_pathv[path_index]);
			}
		}
		else if(glob_retval == GLOB_NOSPACE)
		{
			fprintf(stderr,"glob_expand : Expanding glob %s ran out of space.\n",argv[i]);
			glob_expand_retval = 1; 
		}
		else if(glob_retval == GLOB_ABORTED)
		{
			fprintf(stderr,"glob_expand : Expanding glob %s had a read error.\n",argv[i]);
			glob_expand_retval = 2; 
		}
		else if(glob_retval == GLOB_NOMATCH)
		{
			fprintf(stderr,"glob_expand : Expanding glob %s returned no matches.\n",argv[i]);
			/* not a real error */
		}
		else
		{
			fprintf(stderr,"glob_expand : Expanding glob %s returned unknown error %d.\n",argv[i],
				glob_retval);
		}
		globfree(&glob_list);
	}/* end for on argc */
	return glob_expand_retval;
}

/*
** $Log: glob_expand.c,v $
** Revision 1.1  2008/03/12 16:36:33  cjm
** Initial revision
**
*/
