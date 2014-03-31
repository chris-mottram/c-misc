/* ifbitset
** ifbitset <number> <bit>
** Return 0 if bit <bit> is set in the number <number>, else return 1.
** cc -o ifbitset ifbitset.c
*/
#include <stdio.h>

/**
 * The number of bits in an integer.
 */
#define BIT_COUNT (32)

/**
 * Main program. 
 * @see #BIT_COUNT
 */
int main(int argc,char *argv[])
{
	int retval,value,bit,i;

	/* check and parse arguments */
	if(argc != 3)
	{
		fprintf(stderr,"ifbitset <number> <bit>\n");
		fprintf(stderr,"Return 0 if bit <bit> is set in the number <number>, else return 1.\n");
		fprintf(stderr,"Can also return 2/3/4/5 on parse errors.\n");
		return 2;
	}
	retval = sscanf(argv[1],"%i",&value);
	if(retval != 1)
	{
		perror("ifbitset");
		fprintf(stderr,"ifbitset failed:number %s was not a valid integer.\n",argv[1]);
		return 3;
	}
	retval = sscanf(argv[2],"%i",&bit);
	if(retval != 1)
	{
		perror("ifbitset");
		fprintf(stderr,"ifbitset failed:bit %s was not a valid integer.\n",argv[2]);
		return 4;
	}
	/* check bit in range */
	if((bit < 0) || (bit > BIT_COUNT))
	{
		fprintf(stderr,"ifbitset failed:bit %d was not in range (0..%d).\n",bit,BIT_COUNT);
		return 5;
	}
	/* if bit set return 0, else return 1 */
	if(((1<<bit)&value) > 0)
	{
		fprintf(stdout,"0\n");
		return 0;
	}
	else
	{
		fprintf(stdout,"1\n");
		return 1;
	}
}
/*
** $Log: ifbitset.c,v $
** Revision 1.1  2009/09/02 14:29:02  cjm
** Initial revision
**
*/
