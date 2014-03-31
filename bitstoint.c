/* bitstoint
** bitstoint <bit> [<bit> ...]
** Print integer with the specified bits set (starting at bit 0).
** cc -o bitstoint bitstoint.c
*/
#include <stdio.h>

/**
 * The number of bits in an integer.
 */
#define BIT_COUNT (32)

/**
 * Main program. Go through command line arguments, setting the specified bits. Print out resulting integer.
 * @see #BIT_COUNT
 */
int main(int argc,char *argv[])
{
	int bit_number,i;
	unsigned int value;

	if(argc < 2)
	{
		fprintf(stderr,"bitstoint <bit> [<bit> ...] : No bit specified.\n");
		exit(1);
	}
	value = 0;
	for(i=1;i<argc;i++)
	{
		bit_number = atoi(argv[i]);
		if(bit_number < 0)
			fprintf(stderr,"Argument %d out of range:%d not a valid bit.\n",i,bit_number);
		else if(bit_number >= BIT_COUNT)
			fprintf(stderr,"Argument %d out of range:%d not a valid bit.\n",i,bit_number);
		else
			value += (1<<bit_number);
	}
	fprintf(stdout,"value = %u.\n",value);
	fprintf(stdout,"value = %#x.\n",value);
	return 0;
}
/*
** $Log$
*/
