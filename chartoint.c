/* chartoint -*- mode: Fundamental;-*-
**  chartoint <string>
** list int values of each char in string
*/
#include <stdio.h>

int main(int argc,char *argv[])
{
	int i;

	if(argc != 2)
	{
		fprintf(stderr,"chartoint <string>: No String specified\n");
		exit(1);
	}
	for(i=0;i<strlen(argv[1]);i++)
	{
		fprintf(stdout,"%d",(int)argv[1][i]);
	}
	fprintf(stdout,"\n");
	return 0;
}
