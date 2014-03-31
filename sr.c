/* --------------------------------------------------------
**	sr.c
**	By Chris Mottram
**	Version 1.0
**	sr - search replace across multiple files
**	sr [-help] <search string> <replace string> <file list>...
** -------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH	4096
#define MOVE_COMMAND	"mv"

#ifndef TRUE
#define TRUE	1
#define FALSE 	0
#endif

char *Search_String = NULL;
char *Replace_String = NULL;
char Global_Buff[1024];

static int SR_Bak(char *filename);
static int SR_File(char *filename);
static void SR_Search_Replace(char *in_buff,char *out_buff);

int main(int argc,char *argv[])
{
	int i;

	if (argc < 4)
	{
		fprintf(stderr,"sr [-help] <search string> <replace string> <file list>...\n");
		return 1;
	}
	if (!strcmp(argv[1],"-help"))
	{
		fprintf(stderr,"sr [-help] <search string> <replace string> <file list>...\n");
		return 1;
	}
	Search_String = argv[1];
	Replace_String = argv[2];
	for(i=3;i<argc;i++)
	{
		if(SR_Bak(argv[i]))
			SR_File(argv[i]);
		else
			fprintf(stderr,"sr:warning:SR_Bak failed for '%s'\n",argv[i]);
	}
	return 0;
}

static int SR_Bak(char *filename)
{
	sprintf(Global_Buff,"%s %s %s.bak",MOVE_COMMAND,filename,filename);
	system(Global_Buff);
	return TRUE;
}

static int SR_File(char *filename)
{
	FILE *in_fp = NULL,*out_fp = NULL;
	char in_buff[MAX_LINE_LENGTH],out_buff[MAX_LINE_LENGTH];

/* open input file */
	sprintf(Global_Buff,"%s.bak",filename);
	in_fp = fopen(Global_Buff,"r");
	if(in_fp == NULL)
	{
		fprintf(stderr,"sr:SR_File:File Open(read) failed for '%s'\n",Global_Buff);
		return FALSE;
	}
/* open output file */
	out_fp = fopen(filename,"w");
	if(out_fp == NULL)
	{
		fclose(in_fp);
		fprintf(stderr,"sr:SR_File:File Open(write) failed for '%s'\n",filename);
		return FALSE;
	}
	while(fgets(in_buff,MAX_LINE_LENGTH,in_fp)!= NULL)
	{
		SR_Search_Replace(in_buff,out_buff);
		fprintf(out_fp,"%s",out_buff);
	}
	fclose(out_fp);
	fclose(in_fp);
	return TRUE;
}

static void SR_Search_Replace(char *in_buff,char *out_buff)
/* currenly replaces all occurence of Search_String with Replace_String */
{
	char *in_buff_ptr = NULL;
	char *ch = NULL;

	strcpy(out_buff,"");
	if(!strlen(in_buff))
		return;
	in_buff_ptr = in_buff;
	while((ch = strstr(in_buff_ptr,Search_String))!= NULL)
	{
		strncat(out_buff,in_buff_ptr,ch-in_buff_ptr);
		strcat(out_buff,Replace_String);
		in_buff_ptr = ch+strlen(Search_String);
		if(!strlen(in_buff_ptr))
			break;
	}
	strcat(out_buff,in_buff_ptr);
	return;
}

