/* 
** Error_Flag
*/
/**
 * Error flag finds error codes in a C source file, and orders them, noting
 * free numbers and duplicates. It is run as follows:
 * <pre>
 * error_flag [-name <error flag name>] <file spec>
 * </pre>
 * It can be compiled as follows:
 * <pre>
 * cc -g error_flag.c -o error_flag CJMLib/cjm_lib.a -lMrm -lXm -lXt -lX11
 * </pre>
 * @author Chris Mottram
 * @version $Revision: 1.1 $
 */
#include <stdio.h>
#include "CJMLib/cjm_utils.h"

/**
 * Revision Control System identifier.
 */
static char rcsid[] = "$Id: error_flag.c,v 1.1 2005/12/01 18:47:03 cjm Exp $";
/**
 * The error flag we are searching for. Currently limited to 80 bytes length.
 */
static char Error_Flag_Name[80] = "Global_Error_Flag";
/**
 * A dynamically allocated list of error numbers retrieved from the specified source files.
 * @see #Count
 */
static int *List = NULL;
/**
 * The number of error numbers in List
 * @see #List
 */
static int Count = 0;

static int Add_Int_To_List(int number);

/**
 * Main program.
 * <pre>
 * error_flag [-name <error flag name>] <file spec>
 * </pre>
 * @param argc The number of parameters. There should be at least two.
 * @param argv The list of parameter strings.
 * @return The program returns zero for success and a positive number for failure.
 * @see #List
 * @see #Count
 */
int main(int argc,char *argv[])
{
	FILE *fp;
	char *command_buff = NULL;
	char temporary_filename[L_tmpnam];
	char scanf_buff[128];
	char buff[256];
	int i,error_number;

	if(argc < 2)
	{
		fprintf(stderr,"Error_Flag [-name error flag name] <file spec>.\n");
		return 1;
	}
	for(i=1;i<argc;i++)
	{
		if((strcmp(argv[i],"-name")==0)&&((i+1)<argc))
		{
			strcpy(Error_Flag_Name,argv[i+1]);
		}
	}
	CJM_Add_Text_To_Ptr(&command_buff,"grep -n ");
	CJM_Add_Text_To_Ptr(&command_buff,Error_Flag_Name);
	CJM_Add_Text_To_Ptr(&command_buff," ");
	for(i=1;i<argc;i++)
	{
		if(strcmp(argv[i],"-name") ==0)
			i++;
		else
		{
			CJM_Add_Text_To_Ptr(&command_buff,argv[i]);
			CJM_Add_Text_To_Ptr(&command_buff," ");
		}
	}
	CJM_Add_Text_To_Ptr(&command_buff," > ");
	tmpnam(temporary_filename);
	CJM_Add_Text_To_Ptr(&command_buff,temporary_filename);
	i = system(command_buff);
	if(i)
	{
		fprintf(stderr,"Error_Flag failed: system(%s) returned %d.\n",command_buff,i);
		return 1;
	}
	sprintf(scanf_buff,"%%*s %s = %%d;",Error_Flag_Name);
	fp = fopen(temporary_filename,"r");
	if(fp == NULL)
	{
		fprintf(stderr,"Error_Flag failed: Failed to open temporary file %s.\n",temporary_filename);
		return 1;
	}
	while(fgets(buff,255,fp)!= NULL)
	{
		error_number = 0;
		i = sscanf(buff,scanf_buff,&error_number);
		if(i)
		{
			if(!Add_Int_To_List(error_number))
			{
				fprintf(stderr,"Error_Flag failed: Add_Int_To_List(%d) failed.\n",error_number);
				fclose(fp);
				remove(temporary_filename);
				return 1;
			}/* end if add_to list */
		}/* end if sscanf returned ok */
	}/* end while */
	fclose(fp);
	qsort(List,Count,sizeof(int),CJM_Int_List_Sort);
	fprintf(stdout,"Error Flags(%s):\n",Error_Flag_Name);
	for(i=0;i<Count;i++)
	{
		if(i&&(List[i]>0)&&(List[i-1]>0)&&((List[i]-List[i-1])>1))
			fprintf(stdout,"GAP between %d and %d of %d.\n",List[i-1]+1,List[i]-1,(List[i]-List[i-1])-1);
		if(i&&(List[i]>0)&&(List[i-1]>0)&&(List[i]==List[i-1]))
			fprintf(stdout,"DUPLICATE:%d.\n",List[i]);
		if(List[i]>0)
			fprintf(stdout,"%d.\n",List[i]);
	}
	if(remove(temporary_filename))
	{
		fprintf(stderr,"Error_Flag failed: Failed to remove temporary file %s.\n",temporary_filename);
		return 1;
	}
	return 0;
}

/**
 * Routine to add a number to List.
 * @param number The number to add.
 * @see #List
 * @see #Count
 */
static int Add_Int_To_List(int number)
{
	if(List==NULL)
		List = (int*)malloc(sizeof(int));
	else
		List = (int*)realloc(List,(Count+1)*sizeof(int));
	if(List == NULL)
		return FALSE;
	List[Count++] = number;
	return TRUE;
}
/*
** $Log: error_flag.c,v $
** Revision 1.1  2005/12/01 18:47:03  cjm
** Initial revision
**
*/

