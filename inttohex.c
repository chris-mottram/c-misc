/* inttohex
** inttohex <string>
** prints int values to hexidecimal ints, including hex to int
*/
#include <stdio.h>

int main(int argc,char *argv[])
{
	int value,retval;

	if(argc != 2)
	{
		fprintf(stderr,"inttohex <integer>: No number specified.\n");
		exit(1);
	}
	retval = sscanf(argv[1],"%i",&value);
	if(retval != 1)
	{
		perror("inttohex");
		fprintf(stderr,"inttohex failed:%s was not a valid integer.\n",argv[1]);
		return 2;
	}
	fprintf(stdout,"%#x\n",value);
	return 0;
}
/*
** $Log$
*/
