/* inttoint
** inttoint <string>
** prints int values to ints, including hex to int
*/
#include <stdio.h>

int main(int argc,char *argv[])
{
	int value,retval;

	if(argc != 2)
	{
		fprintf(stderr,"inttoint <integer>: No number specified.\n");
		exit(1);
	}
	retval = sscanf(argv[1],"%i",&value);
	if(retval != 1)
	{
		perror("inttoint");
		fprintf(stderr,"inttoint failed:%s was not a valid integer.\n",argv[1]);
		return 2;
	}
	fprintf(stdout,"%d\n",value);
	return 0;
}
/*
** $Log$
*/
