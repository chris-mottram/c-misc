/* i.c
** Image manipulation program, using libgd.
** Compile as follows:
** Static (recomended as i now uses gdImageSetThickness which is not present in default Redhat 9 version 1.8).
** cc -g i.c -o i -static -lgd -ljpeg -lfreetype -lpng -lz -lm
** Dynamic
** cc -g i.c -o i -lgd
** Example usage: i -create 64 64 -c 255 0 0 -l 0 0 32 32 -c 0 255 0 -l 32 32 0 64 -c 0 0 255 -l 32 32 64 0 -o i.png
** Example usage: i -i i.png -c 255 255 0 -l 32 32 64 32 -o j.png
** Example usage: i -create 64 64 -tr 
**                  -c 156 128 128 -circle 32 32 48 -f 32 32 -oval 7 28 6 12 -oval 57 28 6 12 -f 7 28 -f 57 28 
**                  -c 128 64 64 -oval 7 27 3 6 -c 228 156 156 -oval 57 27 3 6 
**                  -c 250 250 250 -oval 22 25 12 6 -oval 42 25 12 6 -f 22 25 -f 42 25 
**                  -c 228 156 156 -a 22 25 14 8 180 360 -a 42 25 14 8 180 360 
**                  -c 0 48 176 -circle 22 25 2 -circle 42 25 2 
**                  -c 0 0 0 -pt 22 25 -pt 42 25 
**                  -c 255 0 0 -a 32 40 24 8 0 180 -a 32 40 24 4 0 180 
**                  -c 128 0 0 -f 30 42 -f 34 42 
**                  -c 228 156 156 -fp 3 32 28 28 36 32 36 
**                  -c 0 0 0 -a 20 40 6 10 135 225 -a 44 40 6 10 315 45 
**                  -c 0 255 0 -t /usr/X11R6/lib/X11/fonts/TTF/luxirri.ttf 12 0 0 62 "The Face" 
**                  -c 0 0 0 -fr 4 15 60 20 -fr 17 2 47 15 
**                  -c 250 250 250 -fr 40 4 44 12 
**                  -o face.png
*/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "gd.h"

/* hash defines */
/**
 * True.
 */
#define TRUE               (1)
/**
 * False.
 */
#define FALSE              (0)
/**
 * Maximum  recursion level for parsing arguments.
 * i.e. You can only nest command files this deep.
 */
#define MAX_RECURION_LEVEL (10)
/**
 * Length of buffer (specifically, line buffer in parse command file).
 */
#define BUFFER_LEN         (256)

/* enum */
/**
 * Enum describing image manipulation types.
 */
enum Image_Manipulation_Type
{
	TYPE_CREATE,TYPE_LOAD,TYPE_COLOUR,TYPE_LINE_THICKNESS,TYPE_POINT,TYPE_LINE,TYPE_RECTANGLE,TYPE_RECTANGLE_FILL,
	TYPE_POLYGON,TYPE_POLYGON_FILL,
	TYPE_ARC,TYPE_CIRCLE,TYPE_FILL,TYPE_TEXT,TYPE_SAVE
};

/* structures */
/**
 * Used to specify parameters to gdImageCreate.
 */
struct Create_Image_Manipulation_Struct
{
	int Width;
	int Height;
};

/**
 * Used to specify parameters to gdImageCreateFromJpeg,gdImageCreateFromPng.
 */
struct Load_Image_Manipulation_Struct
{
	char *Filename;
};

/**
 * Used to specify parameters to gdImageColorResolve.
 */
struct Colour_Image_Manipulation_Struct 
{
	int Red;
	int Green;
	int Blue;
};

/**
 * Used to specify parameters to gdImageSetThickness.
 */
struct Line_Thickness_Image_Manipulation_Struct 
{
	int Thickness;
};

/**
 * Used to specify parameters to gdImageSetPixel.
 */
struct Point_Image_Manipulation_Struct 
{
	int X;
	int Y;
};

/**
 * Used to specify parameters to gdImageLine.
 */
struct Line_Image_Manipulation_Struct 
{
	int Start_X;
	int Start_Y;
	int End_X;
	int End_Y;
};

/**
 * Used to specify parameters to gdImageRectangle,gdImageFilledRectangle.
 */
struct Rectangle_Image_Manipulation_Struct 
{
	int Start_X;
	int Start_Y;
	int End_X;
	int End_Y;
};

/**
 * Used to specify parameters to gdImagePolygon,gdImageFilledPolygon.
 */
struct Polygon_Image_Manipulation_Struct 
{
	int Point_Count;
	gdPoint *Point_List;
};

/**
 * Used to specify parameters to gdImageFill.
 */
struct Fill_Image_Manipulation_Struct 
{
	int X;
	int Y;
};

/**
 * Used to specify parameters to gdImageArc.
 */
struct Arc_Image_Manipulation_Struct 
{
	int Centre_X;
	int Centre_Y;
	int Width;
	int Height;
	int Start_Angle;
	int End_Angle;
};

/**
 * Used to specify parameters to gdImageStringFT.
 */
struct Text_Image_Manipulation_Struct 
{
	int Start_X;
	int Start_Y;
	char *Fontname;
	double Point_Size;
	int Angle;
        char *Text;
};

/**
 * Used to specify parameters to gdImageJpeg,gdImagePng.
 */
struct Save_Image_Manipulation_Struct
{
	char *Filename;
	int Quality; /* JPEG only */
};

/**
 * Image manipulation structure.
 * @see #Image_Manipulation_Type
 * @see #Create_Image_Manipulation_Struct
 * @see #Load_Image_Manipulation_Struct
 * @see #Colour_Image_Manipulation_Struct
 * @see #Line_Thickness_Image_Manipulation_Struct 
 * @see #Point_Image_Manipulation_Struct
 * @see #Line_Image_Manipulation_Struct
 * @see #Rectangle_Image_Manipulation_Struct
 * @see #Polygon_Image_Manipulation_Struct
 * @see #Fill_Image_Manipulation_Struct
 * @see #Arc_Image_Manipulation_Struct
 * @see #Text_Image_Manipulation_Struct
 * @see #Save_Image_Manipulation_Struct
 */
struct Image_Manipulation_Struct
{
	enum Image_Manipulation_Type Type;
	union
	{
		struct Create_Image_Manipulation_Struct Create;
		struct Load_Image_Manipulation_Struct Load;
		struct Colour_Image_Manipulation_Struct Colour;
		struct Line_Thickness_Image_Manipulation_Struct Line_Thickness;
		struct Point_Image_Manipulation_Struct Point;
		struct Line_Image_Manipulation_Struct Line;
		struct Rectangle_Image_Manipulation_Struct Rectangle;
		struct Polygon_Image_Manipulation_Struct Polygon;
		struct Fill_Image_Manipulation_Struct Fill;
		struct Arc_Image_Manipulation_Struct Arc;
		struct Text_Image_Manipulation_Struct Text;
		struct Save_Image_Manipulation_Struct Save;
	} Manipulation;
};


/* internal variables */
/**
 * Declare the image.
 */
static gdImagePtr Image = NULL;
/**
 * The current foreground colour.
 */
static int Foreground_Colour = -1;
/**
 * The background colour to use when creating a new image. Defaults to white.
 */
static struct Colour_Image_Manipulation_Struct Background_Colour = {255,255,255};
/**
 * Whether a new image should have the background colour made transparent.
 */
static int Transparent = FALSE;
/**
 * List of image manipulations to do.
 */
static struct Image_Manipulation_Struct *Image_Manipulation_List = NULL;
/**
 * Number of image manipulations.
 */
static int Image_Manipulation_Count = 0;

/* internal functions */
static int Image_Manipulation_List_Add(struct Image_Manipulation_Struct **list,int *count,
				       struct Image_Manipulation_Struct image_manipulation);
static int Process_Image(struct Image_Manipulation_Struct *list, int count);
static int Process_Image_Manipulation(struct Image_Manipulation_Struct im);
static int Create_Image(struct Create_Image_Manipulation_Struct im);
static int Load_Image(struct Load_Image_Manipulation_Struct im);
static int Colour_Image(struct Colour_Image_Manipulation_Struct im);
static int Line_Thickness_Image(struct Line_Thickness_Image_Manipulation_Struct im);
static int Point_Image(struct Point_Image_Manipulation_Struct im);
static int Line_Image(struct Line_Image_Manipulation_Struct im);
static int Rectangle_Image(struct Rectangle_Image_Manipulation_Struct im);
static int Filled_Rectangle_Image(struct Rectangle_Image_Manipulation_Struct im);
static int Polygon_Image(struct Polygon_Image_Manipulation_Struct im);
static int Filled_Polygon_Image(struct Polygon_Image_Manipulation_Struct im);
static int Fill_Image(struct Fill_Image_Manipulation_Struct im);
static int Arc_Image(struct Arc_Image_Manipulation_Struct im);
static int Text_Image(struct Text_Image_Manipulation_Struct im);
static int Save_Image(struct Save_Image_Manipulation_Struct im);
static int Parse_Arguments(int recursion_level,int argc, char *argv[]);
static int Parse_Command_Filename(int recursion_level,char *comamnd_filename);
static void Help(void);

/**
 * Main Program.
 * @see #Parse_Arguments
 * @see #Process_Image
 * @see #Image
 */
int main(int argc, char *argv[])
{
	if(!Parse_Arguments(0,argc,argv))
		return 1;
	if(!Process_Image(Image_Manipulation_List,Image_Manipulation_Count))
		return 2;
	if(Image != NULL)
		gdImageDestroy(Image);
	Image = NULL;
	if(Image_Manipulation_List != NULL)
		free(Image_Manipulation_List);
	Image_Manipulation_List = NULL;
	return 0;
}

/**
 * Routine to add a manipulation to a list.
 */
static int Image_Manipulation_List_Add(struct Image_Manipulation_Struct **list,int *count,
				       struct Image_Manipulation_Struct image_manipulation)
{
	if((*list) == NULL)
	{
		(*list) = (struct Image_Manipulation_Struct *)malloc(sizeof(struct Image_Manipulation_Struct));
	}
	else
	{
		(*list) = (struct Image_Manipulation_Struct *)realloc((*list),
							((*count)+1)*sizeof(struct Image_Manipulation_Struct));
	}
	if((*list) == NULL)
	{
		fprintf(stderr,"Image_Manipulation_List_Add:Failed to (re) allocate image manipulation list(%d).\n",
			(*count));
		return FALSE;
	}
	(*list)[(*count)++] = image_manipulation;
	return TRUE;
}

/**
 * Process the specified image manipulation list.
 * @see #Process_Image_Manipulation
 */
static int Process_Image(struct Image_Manipulation_Struct *list, int count)
{
	int i;

	for(i=0;i<count;i++)
	{
		if(!Process_Image_Manipulation(list[i]))
			return FALSE;
	}
	return TRUE;
}

/**
 * Process a specified Image manipulation.
 * @see #Create_Image
 * @see #Load_Image
 * @see #Colour_Image
 * @see #Line_Thickness_Image
 * @see #Point_Image
 * @see #Line_Image
 * @see #Rectangle_Image
 * @see #Filled_Rectangle_Image
 * @see #Polygon_Image
 * @see #Filled_Polygon_Image
 * @see #Fill_Image
 * @see #Arc_Image
 * @see #Text_Image
 * @see #Save_Image
 */
static int Process_Image_Manipulation(struct Image_Manipulation_Struct im)
{
	switch(im.Type)
	{
		case TYPE_CREATE:
			Create_Image(im.Manipulation.Create);
			break;
		case TYPE_LOAD:
			Load_Image(im.Manipulation.Load);
			break;
		case TYPE_COLOUR:
			Colour_Image(im.Manipulation.Colour);
			break;
		case TYPE_LINE_THICKNESS:
			Line_Thickness_Image(im.Manipulation.Line_Thickness);
			break;
		case TYPE_POINT:
			Point_Image(im.Manipulation.Point);
			break;
		case TYPE_LINE:
			Line_Image(im.Manipulation.Line);
			break;
		case TYPE_RECTANGLE:
			Rectangle_Image(im.Manipulation.Rectangle);
			break;
		case TYPE_RECTANGLE_FILL:
			Filled_Rectangle_Image(im.Manipulation.Rectangle);
			break;
		case TYPE_POLYGON:
			Polygon_Image(im.Manipulation.Polygon);
			break;
		case TYPE_POLYGON_FILL:
			Filled_Polygon_Image(im.Manipulation.Polygon);
			break;
		case TYPE_FILL:
			Fill_Image(im.Manipulation.Fill);
			break;
		case TYPE_ARC:
			Arc_Image(im.Manipulation.Arc);
			break;
		case TYPE_TEXT:
			Text_Image(im.Manipulation.Text);
			break;
		case TYPE_SAVE:
			Save_Image(im.Manipulation.Save);
			break;
		default:
			fprintf(stderr,"Process_Image_Manipulation:Failed to process Type %d.\n",im.Type);
			return FALSE;
	}
	return TRUE;
}

/**
 * Routine to create an image.
 * @see #Image
 * @see #Create_Image_Manipulation_Struct
 * @see #Background_Colour
 * @see #Transparent
 */
static int Create_Image(struct Create_Image_Manipulation_Struct im)
{
	int background_colour;

	if(Image != NULL)
	{
		fprintf(stderr,"Create_Image:Image already exists.\n");
		return FALSE;
	}
	Image = gdImageCreate(im.Width,im.Height);
	if(Image == NULL)
	{
		fprintf(stderr,"Create_Image:Failed to Create Image (%d,%d).\n",im.Width,im.Height);
		return FALSE;
	}
        /* Allocate the background color. 
	** Since this is the first color in a new image, it will be the background color. */
        background_colour = gdImageColorAllocate(Image,Background_Colour.Red,Background_Colour.Green,
						 Background_Colour.Blue);
	/* and make the background transparent. */
	if(Transparent)
		gdImageColorTransparent(Image,background_colour);
        /* Allocate the color black , as the current foreground colour. */
        Foreground_Colour = gdImageColorAllocate(Image, 0, 0, 0);
	return TRUE;
}

static int Load_Image(struct Load_Image_Manipulation_Struct im)
{
	FILE *fp = NULL;

	if(Image != NULL)
	{
		fprintf(stderr,"Load_Image:Image already exists.\n");
		return FALSE;
	}
	if(im.Filename == NULL)
	{
		fprintf(stderr,"Load_Image:Filename was NULL.\n");
		return FALSE;
	}
	if(strstr(im.Filename,".jpg")!=NULL)
	{
		fp = fopen(im.Filename,"rb");
		if(fp == NULL)
		{
			fprintf(stderr,"Load_Image:Failed to Open Filename (%s).\n",im.Filename);
			return FALSE;
		}
		Image = gdImageCreateFromJpeg(fp);
		fclose(fp);
		if(Image == NULL)
		{
			fprintf(stderr,"Load_Image:Failed to Load Image (%s).\n",im.Filename);
			return FALSE;
		}
		free(im.Filename);
	}
	else if(strstr(im.Filename,".png")!=NULL)
	{
		fp = fopen(im.Filename,"rb");
		if(fp == NULL)
		{
			fprintf(stderr,"Load_Image:Failed to Open Filename (%s).\n",im.Filename);
			return FALSE;
		}
		Image = gdImageCreateFromPng(fp);
		fclose(fp);
		if(Image == NULL)
		{
			fprintf(stderr,"Load_Image:Failed to Load Image (%s).\n",im.Filename);
			return FALSE;
		}
		free(im.Filename);
	}
	else
	{
		fprintf(stderr,"Load_Image:Filename %s was not jpg/png.\n",im.Filename);
		return FALSE;
	}
	return TRUE;
}

static int Colour_Image(struct Colour_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Colour_Image:Image does not exist.\n");
		return FALSE;
	}
	Foreground_Colour = gdImageColorResolve(Image,im.Red,im.Green,im.Blue);
	return TRUE;
}

static int Line_Thickness_Image(struct Line_Thickness_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Line_Thickness_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImageSetThickness(Image,im.Thickness);
	return TRUE;
}

static int Point_Image(struct Point_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Point_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImageSetPixel(Image,im.X,im.Y,Foreground_Colour);
	return TRUE;
}

static int Line_Image(struct Line_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Line_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImageLine(Image,im.Start_X,im.Start_Y,im.End_X,im.End_Y,Foreground_Colour);
	return TRUE;
}

static int Rectangle_Image(struct Rectangle_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Rectangle_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImageRectangle(Image,im.Start_X,im.Start_Y,im.End_X,im.End_Y,Foreground_Colour);
	return TRUE;
}

static int Filled_Rectangle_Image(struct Rectangle_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Filled_Rectangle_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImageFilledRectangle(Image,im.Start_X,im.Start_Y,im.End_X,im.End_Y,Foreground_Colour);
	return TRUE;
}

static int Polygon_Image(struct Polygon_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Polygon_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImagePolygon(Image,im.Point_List,im.Point_Count,Foreground_Colour);
	return TRUE;
}

static int Filled_Polygon_Image(struct Polygon_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Filled_Polygon_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImageFilledPolygon(Image,im.Point_List,im.Point_Count,Foreground_Colour);
	return TRUE;
}

static int Fill_Image(struct Fill_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Fill_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImageFill(Image,im.X,im.Y,Foreground_Colour);
	return TRUE;
}

static int Arc_Image(struct Arc_Image_Manipulation_Struct im)
{
	if(Image == NULL)
	{
		fprintf(stderr,"Arc_Image:Image does not exist.\n");
		return FALSE;
	}
	gdImageArc(Image,im.Centre_X,im.Centre_Y,im.Width,im.Height,im.Start_Angle,im.End_Angle,Foreground_Colour);
	return TRUE;
}

static int Text_Image(struct Text_Image_Manipulation_Struct im)
{
	double angle_radians;
	char *return_str = NULL;
	int bounding_rectangle[8];

	if(Image == NULL)
	{
		fprintf(stderr,"Text_Image:Image does not exist.\n");
		return FALSE;
	}
	angle_radians = (((double)(im.Angle))*M_PI)/180.0;
	return_str = gdImageStringFT(Image,&(bounding_rectangle[0]),Foreground_Colour,im.Fontname,im.Point_Size,
				     angle_radians,im.Start_X,im.Start_Y,im.Text);
	free(im.Fontname);
	im.Fontname = NULL;
	free(im.Text);
	im.Text = NULL;
	if(return_str != NULL)
	{
		fprintf(stderr,"Text_Image:Failed to render text:%s.\n",return_str);
		return FALSE;
	}
	return TRUE;
}

static int Save_Image(struct Save_Image_Manipulation_Struct im)
{
	FILE *fp = NULL;

	if(Image == NULL)
	{
		fprintf(stderr,"Save_Image:Image does not exist.\n");
		return FALSE;
	}
	if(im.Filename == NULL)
	{
		fprintf(stderr,"Save_Image:Filename was NULL.\n");
		return FALSE;
	}
	if(strstr(im.Filename,".jpg")!=NULL)
	{
		fp = fopen(im.Filename,"wb");
		if(fp == NULL)
		{
			fprintf(stderr,"Save_Image:Failed to Open Filename (%s).\n",im.Filename);
			return FALSE;
		}
		gdImageJpeg(Image,fp,im.Quality);
		fclose(fp);
		free(im.Filename);
	}
	else if(strstr(im.Filename,".png")!=NULL)
	{
		fp = fopen(im.Filename,"wb");
		if(fp == NULL)
		{
			fprintf(stderr,"Save_Image:Failed to Open Filename (%s).\n",im.Filename);
			return FALSE;
		}
		gdImagePng(Image,fp);
		fclose(fp);
		free(im.Filename);
	}
	else
	{
		fprintf(stderr,"Load_Image:Filename %s was not jpg/png.\n",im.Filename);
		return FALSE;
	}
	return TRUE;
}

/**
 * Parse arguments.
 * @param recursion_level What level of recursion we are dealing with. 0 is command line, 1 is
 *        command file, 2 is command file in command file, etc.
 * @see #Image_Manipulation_List_Add
 * @see #Image_Manipulation_List
 * @see #Image_Manipulation_Count
 * @see #Help
 * @see #MAX_RECURION_LEVEL
 */
static int Parse_Arguments(int recursion_level,int argc, char *argv[])
{
	struct Image_Manipulation_Struct Image_Manipulation;
	int i,j,retval;

	if(recursion_level > MAX_RECURION_LEVEL)
	{
		fprintf(stderr,"Parse_Arguments:Recursion Level %d too high.\n");
		return FALSE;
	}
	for(i=1;i<argc;i++)
	{
		if((strcmp(argv[i],"-arc")==0)||(strcmp(argv[i],"-a")==0))
		{
			if((i+6)<argc)
			{
				Image_Manipulation.Type = TYPE_ARC;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Arc.Centre_X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Arc:Parsing Centre_X %s failed.\n",argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Arc.Centre_Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Arc:Parsing Centre_Y %s failed.\n",argv[i+2]);
					return FALSE;
				}
				retval = sscanf(argv[i+3],"%d",&(Image_Manipulation.Manipulation.Arc.Width));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Arc:Parsing Width %s failed.\n",argv[i+3]);
					return FALSE;
				}
				retval = sscanf(argv[i+4],"%d",&(Image_Manipulation.Manipulation.Arc.Height));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Arc:Parsing Height %s failed.\n",argv[i+4]);
					return FALSE;
				}
				retval = sscanf(argv[i+5],"%d",&(Image_Manipulation.Manipulation.Arc.Start_Angle));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Arc:Parsing Start_Angle %s failed.\n",
						argv[i+5]);
					return FALSE;
				}
				retval = sscanf(argv[i+6],"%d",&(Image_Manipulation.Manipulation.Arc.End_Angle));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Arc:Parsing End_Angle %s failed.\n",
						argv[i+6]);
					return FALSE;
				}
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 6;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Arc requires <centre x> <centre y> <width> <height> "
					"<start_angle> <end angle>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-background_colour")==0)||(strcmp(argv[i],"-bc")==0))
		{
			if((i+3)<argc)
			{
				retval = sscanf(argv[i+1],"%d",&(Background_Colour.Red));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Background Colour:Parsing Red %s failed.\n",
						argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Background_Colour.Green));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Background Colour:Parsing Green %s failed.\n",
						argv[i+2]);
					return FALSE;
				}
				retval = sscanf(argv[i+3],"%d",&(Background_Colour.Blue));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Background Colour:Parsing Blue %s failed.\n",
						argv[i+3]);
					return FALSE;
				}
				i+= 3;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Background Colour requires <red> <green> <blue>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-circle")==0))
		{
			if((i+3)<argc)
			{
				Image_Manipulation.Type = TYPE_ARC;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Arc.Centre_X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Circle:Parsing Centre_X %s failed.\n",
						argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Arc.Centre_Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Circle:Parsing Centre_Y %s failed.\n",
						argv[i+2]);
					return FALSE;
				}
				retval = sscanf(argv[i+3],"%d",&(Image_Manipulation.Manipulation.Arc.Width));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Circle:Parsing Width %s failed.\n",argv[i+3]);
					return FALSE;
				}
				Image_Manipulation.Manipulation.Arc.Height = Image_Manipulation.Manipulation.Arc.Width;
				Image_Manipulation.Manipulation.Arc.Start_Angle = 0;
				Image_Manipulation.Manipulation.Arc.End_Angle = 360;
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 3;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Circle requires <centre x> <centre y> <diameter>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-colour")==0)||(strcmp(argv[i],"-c")==0))
		{
			if((i+3)<argc)
			{
				Image_Manipulation.Type = TYPE_COLOUR;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Colour.Red));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Colour:Parsing Red %s failed.\n",argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Colour.Green));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Colour:Parsing Green %s failed.\n",argv[i+2]);
					return FALSE;
				}
				retval = sscanf(argv[i+3],"%d",&(Image_Manipulation.Manipulation.Colour.Blue));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Colour:Parsing Blue %s failed.\n",argv[i+3]);
					return FALSE;
				}
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 3;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Colour requires <red> <green> <blue>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-command_file")==0)||(strcmp(argv[i],"-cf")==0))
		{
			if((i+1)<argc)
			{
				if(!Parse_Command_Filename(recursion_level,argv[i+1]))
					return FALSE;
				i++;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Command filename requires a filename.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-create")==0))
		{
			if((i+2)<argc)
			{
				Image_Manipulation.Type = TYPE_CREATE;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Create.Width));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Create:Parsing %s failed.\n",argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Create.Height));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Create:Parsing %s failed.\n",argv[i+2]);
					return FALSE;
				}
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 2;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Create requires <width> <height>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-fill")==0)||(strcmp(argv[i],"-f")==0))
		{
			if((i+2)<argc)
			{
				Image_Manipulation.Type = TYPE_FILL;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Fill.X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Fill:Parsing X %s failed.\n",argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Fill.Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Fill:Parsing Y %s failed.\n",argv[i+2]);
					return FALSE;
				}
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 2;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Fill requires <x> and <y>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-help")==0)||(strcmp(argv[i],"-h")==0))
		{
			Help();
			exit(0);
		}
		else if((strcmp(argv[i],"-input_filename")==0)||(strcmp(argv[i],"-i")==0))
		{
			if((i+1)<argc)
			{
				Image_Manipulation.Type = TYPE_LOAD;
				Image_Manipulation.Manipulation.Load.Filename = strdup(argv[i+1]);
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i++;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Input filename requires a filename.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-line")==0)||(strcmp(argv[i],"-l")==0))
		{
			if((i+4)<argc)
			{
				Image_Manipulation.Type = TYPE_LINE;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Line.Start_X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Line:Parsing Start_X %s failed.\n",argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Line.Start_Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Line:Parsing Start_Y %s failed.\n",argv[i+2]);
					return FALSE;
				}
				retval = sscanf(argv[i+3],"%d",&(Image_Manipulation.Manipulation.Line.End_X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Line:Parsing End_X %s failed.\n",argv[i+3]);
					return FALSE;
				}
				retval = sscanf(argv[i+4],"%d",&(Image_Manipulation.Manipulation.Line.End_Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Line:Parsing End_Y %s failed.\n",argv[i+4]);
					return FALSE;
				}
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 4;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Line requires 4 arguments.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-line_thickness")==0)||(strcmp(argv[i],"-lt")==0))
		{
			if((i+1)<argc)
			{
				Image_Manipulation.Type = TYPE_LINE_THICKNESS;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Line_Thickness.Thickness));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Line Thickness:Parsing Thickness %s failed.\n",argv[i+1]);
					return FALSE;
				}
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 1;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Line Thickness requires an integer value.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-oval")==0))
		{
			if((i+4)<argc)
			{
				Image_Manipulation.Type = TYPE_ARC;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Arc.Centre_X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Oval:Parsing Centre_X %s failed.\n",argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Arc.Centre_Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Oval:Parsing Centre_Y %s failed.\n",argv[i+2]);
					return FALSE;
				}
				retval = sscanf(argv[i+3],"%d",&(Image_Manipulation.Manipulation.Arc.Width));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Oval:Parsing Width %s failed.\n",argv[i+3]);
					return FALSE;
				}
				retval = sscanf(argv[i+4],"%d",&(Image_Manipulation.Manipulation.Arc.Height));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Oval:Parsing Height %s failed.\n",argv[i+4]);
					return FALSE;
				}
				Image_Manipulation.Manipulation.Arc.Start_Angle = 0;
				Image_Manipulation.Manipulation.Arc.End_Angle = 360;
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 4;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Circle requires <centre x> <centre y> <diameter>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-output_filename")==0)||(strcmp(argv[i],"-o")==0))
		{
			if((i+1)<argc)
			{
				Image_Manipulation.Type = TYPE_SAVE;
				Image_Manipulation.Manipulation.Save.Filename = strdup(argv[i+1]);
				Image_Manipulation.Manipulation.Save.Quality = -1;
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i++;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Output filename requires a filename.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-point")==0)||(strcmp(argv[i],"-pt")==0))
		{
			if((i+2)<argc)
			{
				Image_Manipulation.Type = TYPE_POINT;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Point.X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Point:Parsing X %s failed.\n",argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Point.Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Point:Parsing Y %s failed.\n",argv[i+2]);
					return FALSE;
				}
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 2;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Point requires <x> and <y>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-polygon")==0)||(strcmp(argv[i],"-p")==0)||
			(strcmp(argv[i],"-filled_polygon")==0)||(strcmp(argv[i],"-fp")==0))
		{
			if((i+1)<argc)
			{
				if((strcmp(argv[i],"-polygon")==0)||(strcmp(argv[i],"-p")==0))
					Image_Manipulation.Type = TYPE_POLYGON;
				else
					Image_Manipulation.Type = TYPE_POLYGON_FILL;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Polygon.Point_Count));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Polygon:Parsing Point_Count %s failed.\n",
						argv[i+1]);
					return FALSE;
				}
				i++;
			        if((i+(Image_Manipulation.Manipulation.Polygon.Point_Count*2))< argc)
				{
					Image_Manipulation.Manipulation.Polygon.Point_List = (gdPoint*)malloc(
					       Image_Manipulation.Manipulation.Polygon.Point_Count*sizeof(gdPoint));
					if(Image_Manipulation.Manipulation.Polygon.Point_List == NULL)
					{
						fprintf(stderr,"Parse_Arguments:Polygon:"
							"Failed to allocate %d points.\n",
							Image_Manipulation.Manipulation.Polygon.Point_Count);
						return FALSE;
					}
					for(j=0;j<Image_Manipulation.Manipulation.Polygon.Point_Count;j++)
					{
						retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.
										 Polygon.Point_List[j].x));
						if(retval != 1)
						{
							fprintf(stderr,"Parse_Arguments:Polygon:"
								"Parsing X[%d] %s failed.\n",j,argv[i+i]);
							return FALSE;
						}
						retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.
										 Polygon.Point_List[j].y));
						if(retval != 1)
						{
							fprintf(stderr,"Parse_Arguments:Polygon:"
								"Parsing X[%d] %s failed.\n",j,argv[i+2]);
							return FALSE;
						}
						i += 2;
					}
					if(!Image_Manipulation_List_Add(&Image_Manipulation_List,
							       &Image_Manipulation_Count,Image_Manipulation))
						return FALSE;
				}
				else
				{
					fprintf(stderr,"Parse_Arguments:"
						"Polygon requires <no of points> [<x> <y>...] (%d,%d,%d).\n",
						i,Image_Manipulation.Manipulation.Polygon.Point_Count,argc);
					return FALSE;
				}
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Polygon requires <no of points> [<x> <y>...].\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-rectangle")==0)||(strcmp(argv[i],"-r")==0)||
			(strcmp(argv[i],"-filled_rectangle")==0)||(strcmp(argv[i],"-fr")==0))
		{
			if((i+4)<argc)
			{
				if((strcmp(argv[i],"-rectangle")==0)||(strcmp(argv[i],"-r")==0))
					Image_Manipulation.Type = TYPE_RECTANGLE;
				else
					Image_Manipulation.Type = TYPE_RECTANGLE_FILL;
				retval = sscanf(argv[i+1],"%d",&(Image_Manipulation.Manipulation.Rectangle.Start_X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Rectangle:Parsing Start_X %s failed.\n",
						argv[i+1]);
					return FALSE;
				}
				retval = sscanf(argv[i+2],"%d",&(Image_Manipulation.Manipulation.Rectangle.Start_Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Rectangle:Parsing Start_Y %s failed.\n",
						argv[i+2]);
					return FALSE;
				}
				retval = sscanf(argv[i+3],"%d",&(Image_Manipulation.Manipulation.Rectangle.End_X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Line:Parsing End_X %s failed.\n",argv[i+3]);
					return FALSE;
				}
				retval = sscanf(argv[i+4],"%d",&(Image_Manipulation.Manipulation.Rectangle.End_Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Line:Parsing End_Y %s failed.\n",argv[i+4]);
					return FALSE;
				}
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 4;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Rectangle requires <sx> <sy> <ex> <ey>.\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-text")==0)||(strcmp(argv[i],"-t")==0))
		{
			if((i+6)<argc)
			{
				Image_Manipulation.Type = TYPE_TEXT;
				Image_Manipulation.Manipulation.Text.Fontname = strdup(argv[i+1]);
				retval = sscanf(argv[i+2],"%lf",&(Image_Manipulation.Manipulation.Text.Point_Size));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Point:Parsing Point Size %s failed.\n",
						argv[i+2]);
					return FALSE;
				}
				retval = sscanf(argv[i+3],"%d",&(Image_Manipulation.Manipulation.Text.Angle));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Point:Parsing Angle %s failed.\n",argv[i+3]);
					return FALSE;
				}
				retval = sscanf(argv[i+4],"%d",&(Image_Manipulation.Manipulation.Text.Start_X));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Point:Parsing Start_X %s failed.\n",argv[i+4]);
					return FALSE;
				}
				retval = sscanf(argv[i+5],"%d",&(Image_Manipulation.Manipulation.Text.Start_Y));
				if(retval != 1)
				{
					fprintf(stderr,"Parse_Arguments:Point:Parsing Start_Y %s failed.\n",argv[i+5]);
					return FALSE;
				}
				Image_Manipulation.Manipulation.Text.Text = strdup(argv[i+6]);
				if(!Image_Manipulation_List_Add(&Image_Manipulation_List,&Image_Manipulation_Count,
								Image_Manipulation))
					return FALSE;
				i+= 6;
			}
			else
			{
				fprintf(stderr,"Parse_Arguments:Text requires <fontname> <point size> <angle> "
					"<x> <y> \"<text>\".\n");
				return FALSE;
			}
		}
		else if((strcmp(argv[i],"-transparent")==0)||(strcmp(argv[i],"-tr")==0))
		{
			Transparent = TRUE;
		}
		else
		{
			fprintf(stderr,"Parse_Arguments:unknown option '%s'\n",argv[i]);
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * Routine to load command line arguments from the specified filename and pass then to Parse_Arguments
 * for processing
 * @param recursion_level The recursion level, used to stop infinite loops/ too deep nesting when
 *        a command file includes itself.
 * @param comamnd_filename The filename of a '.i' file to load, containing valid command line arguments.
 * @see #Parse_Arguments
 * @see #BUFFER_LEN
 */
static int Parse_Command_Filename(int recursion_level,char *comamnd_filename)
{
	char buff[BUFFER_LEN];
	FILE *fp = NULL;
	char **argument_list = NULL;
	char *argument = NULL;
	char *ch_ptr;
	int argument_count = 0;
	int done,i,retval,in_double_quote,in_single_quote;

	fp = fopen(comamnd_filename,"r");
	if(fp == NULL)
	{
		fprintf(stderr,"Parse_Command_Filename:Filename '%s' could not be opened.\n",comamnd_filename);
		return FALSE;
	}
	argument_list = NULL;
	argument_count = 0;
	done = FALSE;
	/* allocate first argument to be command name 'i' */
	argument_list = (char **)malloc(sizeof(char *));
	if(argument_list == NULL)
	{
		fprintf(stderr,"Parse_Command_Filename:Failed to allocate argument list.\n");
		return FALSE;
	}
	argument_list[argument_count] = strdup("i");
	if(argument_list[argument_count] == NULL)
	{
		free(argument_list);
		fprintf(stderr,"Parse_Command_Filename:Failed to allocate argument list 0.\n");
		return FALSE;
	}
	argument_count++;
	while(done == FALSE)
	{
		ch_ptr = fgets(buff,BUFFER_LEN,fp);
		if(ch_ptr == NULL)
			done = TRUE;
		if(strlen(buff) == (BUFFER_LEN-1))
		{
			fprintf(stderr,"Parse_Command_Filename:Line '%s' too long.\n",buff);
			return FALSE;
		}
		/* if we've not finished and this line is not a comment line */
		if((done == FALSE)&&(buff[0] != '#'))
		{
			in_double_quote = FALSE;
			in_single_quote = FALSE;
			argument = NULL;
			ch_ptr = strtok(buff," \t\n");
			while(ch_ptr != NULL)
			{
				/* check ch_ptr for quotes, undate variables and delete any quotes found from ch_ptr */
				if(ch_ptr[0] == '"')
				{
					in_double_quote = TRUE;
					ch_ptr++;
				}
				if(ch_ptr[0] == '\'')
				{
					in_single_quote = TRUE;
					ch_ptr++;
				}
				if(ch_ptr[strlen(ch_ptr)-1] == '"')
				{
					in_double_quote = FALSE;
					ch_ptr[strlen(ch_ptr)-1] = '\0';
				}
				if(ch_ptr[strlen(ch_ptr)-1] == '\'')
				{
					in_single_quote = FALSE;
					ch_ptr[strlen(ch_ptr)-1] = '\0';
				}
				/* add ch_ptr to argument */
				if(argument == NULL)
				{
					argument = strdup(ch_ptr);
					if(argument == NULL)
					{
						fprintf(stderr,"Parse_Command_Filename:"
							"Failed to allocate argument from %s.\n",ch_ptr);
						return FALSE;
					}
				}
				else
				{
					argument = (char *)realloc(argument,
								   (strlen(argument)+strlen(ch_ptr)+2)*sizeof(char));
					if(argument == NULL)
					{
						fprintf(stderr,"Parse_Command_Filename:"
							"Failed to re-allocate argument from %s.\n",ch_ptr);
						return FALSE;
					}
					strcat(argument," ");
					strcat(argument,ch_ptr);
				}
				if((in_double_quote == FALSE)&&(in_single_quote == FALSE))
				{
					/* add new argument to list */
					argument_list = (char **)realloc(argument_list,
									 (argument_count+1)*sizeof(char *));
					if(argument_list == NULL)
					{
						fprintf(stderr,"Parse_Command_Filename:"
							"Failed to allocate argument list %d from %s.\n",
							argument_count,buff);
						return FALSE;
					}
					argument_list[argument_count] = argument;
					if(argument_list[argument_count] == NULL)
					{
						fprintf(stderr,"Parse_Command_Filename:"
							"Failed to allocate argument %d (%s) from %s.\n",argument_count,
							argument,buff);
						return FALSE;
					}
					argument_count++;
					argument = NULL;
				}/* end if not in single/double quote */
				/* get next argument or end of arguments! */
				ch_ptr = strtok(NULL," \t\n");
			}/* end while on tokens in line */
			if((in_double_quote == TRUE)||(in_single_quote == TRUE))
			{
				fprintf(stderr,"Parse_Command_Filename:"
						"Line (%s) has mis-matched quotes.\n",buff);
				return FALSE;
			}
		}/* end if process this line */
	}/* end while not end of file */
	fclose(fp);
	/* call parse arguments */
	retval = Parse_Arguments(recursion_level+1,argument_count,argument_list);
	/* free argument list */
	for(i=0; i < argument_count;i++)
	{
		free(argument_list[i]);
	}
	free(argument_list);
	return retval;
}

/**
 * Help routine.
 */
static void Help(void)
{
	fprintf(stdout,"I :Help.\n");
	fprintf(stdout,"This program allows the user to create and add basic \n");
	fprintf(stdout,"\tdrawing elements to a PNG/JPEG.\n");
	fprintf(stdout,"i\t[-a[rc] <centre x> <centre y> <width> <height> <start_angle> <end angle>]\n");
	fprintf(stdout,"\t[-[background_olour|bc] <red> <green> <blue>]\n");
	fprintf(stdout,"\t[-circle <centre x> <centre y> <diameter>]\n");
	fprintf(stdout,"\t[-c[olour] <red> <green> <blue>]\n");
	fprintf(stdout,"\t[-[command_file|cf] <filename>]\n");
	fprintf(stdout,"\t[-create <width> <height>]\n");
	fprintf(stdout,"\t[-f[ill] <start x> <start y>]\n");
	fprintf(stdout,"\t[-[filled_polygon|fp] <no of points> [<x> <y> ...]]\n");
	fprintf(stdout,"\t[-[filled_ectangle|fr] <start x> <start y> <end x> <end y>]\n");
	fprintf(stdout,"\t[-i[nput_filename] <filename>]\n");
	fprintf(stdout,"\t[-h[elp]]\n");
	fprintf(stdout,"\t[-l[ine] <start x> <start y> <end x> <end y>]\n");
	fprintf(stdout,"\t[-line_thickness|-lt <thickness>]\n");
	fprintf(stdout,"\t[-[p[oin]t] <x> <y>]\n");
	fprintf(stdout,"\t[-p[olygon] <no of points> [<x> <y> ...]]\n");
	fprintf(stdout,"\t[-oval <centre x> <centre y> <width> <height>]\n");
	fprintf(stdout,"\t[-o[utput_filename] <filename>]\n");
	fprintf(stdout,"\t[-r[ectangle] <start x> <start y> <end x> <end y>]\n");
	fprintf(stdout,"\t[-t[ext] <fontname> <point size> <angle> <x> <y> \"<text>\"]\n");
	fprintf(stdout,"\t[-[transparent|tr]\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-arc draws an arc in the current foreground colour.\n");
	fprintf(stdout,"-background_colour changes the background colour used when creating a new image.\n");
	fprintf(stdout,"-circle draws a circle in the current foreground colour.\n");
	fprintf(stdout,"-colour changes the current foreground colour.\n");
	fprintf(stdout,"-command_file loads arguments from a '.i' file and processes them.\n");
	fprintf(stdout,"-create specifies an image of the specified dimensions.\n");
	fprintf(stdout,"-fill fills in the current foreground colour from the set point.\n");
	fprintf(stdout,"-filled_polygon draws a filled polygon in the current foreground colour.\n");
	fprintf(stdout,"-filled_rectangle draws a filled rectangle in the current foreground colour.\n");
	fprintf(stdout,"-help prints out this message and stops the program.\n");
	fprintf(stdout,"-input_filename loads a previously created image.\n");
	fprintf(stdout,"-line draws a line in the current foreground colour.\n");
	fprintf(stdout,"-line_thickness sets the width of the lines drawn for lines, ellipses, rectangles, polygons etc....\n");
	fprintf(stdout,"-point sets the specified x,y location to the current foreground colour.\n");
	fprintf(stdout,"-polygon draws a polygon in the current foreground colour.\n");
	fprintf(stdout,"-oval draws an oval in the current foreground colour.\n");
	fprintf(stdout,"-output_filename save the image to the specified filename.\n");
	fprintf(stdout,"-rectangle draws a rectangle in the current foreground colour.\n");
	fprintf(stdout,"-text prints the text in the current foreground colour.\n");
	fprintf(stdout,"-transparent makes the background colour transparent when creating a new PNG image.\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"Both PNG and JPEG filenames are supported.\n");
}

/*
** $Log: i.c,v $
** Revision 1.2  2005/01/20 17:58:27  cjm
** Added static/dynamic linking comment.
**
** Revision 1.1  2005/01/20 17:54:00  cjm
** Initial revision
**
*/
