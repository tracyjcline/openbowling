/*    openbowling.c
 *
 *    Various funtions for analyzing pictures.
 *    Copyright 2010 by Matt Mead
 *    This software is distributed under the GNU public license version 3
 *
 */
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <glob.h>
#include <jpeglib.h>


#include "openbowling.h"

#define MAX_X  640
#define MAX_Y  480

#define PIPE_NAME    "/tmp/ob_pipe"
#define LINE_LENGTH  12
#define INIT_FILE    "openbowling.conf"
#define PINS         10 /* Num of pins (10) */
#define RAKE         2  /* Num of rake points */

struct config {
    int pin_r;  int pin_g;  int pin_b;  
    int rake_r; int rake_g; int rake_b;  
    int pinx[PINS];  int piny[PINS];
    int rakex[RAKE]; int rakey[RAKE];
};
 
struct config data = {
    pin_r:   -1, pin_g:   -1, pin_b:   -1,
    rake_r:  -1, rake_g:  -1, rake_b:  -1,
    pinx:  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    piny:  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    rakex: { -1, -1 },
    rakey: { -1, -1 }
};

enum states {
    unknown = 0,
    ready_for_ball,
    rake_down,
    sweeping,
    rake_up
};

unsigned char *raw_image = NULL; // For picture test (read jpg files)

int  init_complete = FALSE;

void analyze_frame( void );
int  is_rake_down( void );
int  pins_down( void );
int  convert2rgb( unsigned char *image, int width, int height );
void send_data( int data );
int  get_red  (int x, int y);
int  get_green(int x, int y);
int  get_blue (int x, int y);
int  initialized ( void );
int  valid_integer( char *arg, int hex );
int  readrgbfile(char *filename);


/* Buffer to hold the RGB data for pixel color mapping */
static unsigned int rgb_map[MAX_X][MAX_Y];

static int pipe_fd;

int  valid_integer( char *arg, int hex )
{
    int return_value = 0;
    char *end_ptr;

    errno = 0;
    if (!hex)
    {
        // Calculate decimal value
        return_value = (int)strtol(arg, &end_ptr, 10);
    }
    if ((errno != 0) || (end_ptr == arg) || hex) 
    {
        // Calculate Hex value
        errno = 0;
        return_value = (int)strtol(arg, &end_ptr, 16);
    }

    if ((errno != 0) || (end_ptr == arg)) return_value = -1;

    return return_value;
}

// Initialize routine - get startup data, etc
// Return: FALSE - init completed; TRUE - init not done
int initialized ( void )
{
    FILE *fp = NULL;
    char line[PATH_MAX], *cmd = NULL, *arg1 = NULL;
    char *beg = NULL, *end = NULL, *str = NULL; 
    int  value, hex, temp; 

printf("OB: Initialized started\n");

    if (init_complete) return FALSE;
    
    fp = fopen(INIT_FILE, "r");

    if (!fp) return TRUE;

    /* process each line from the config file */

    while (fgets(line, PATH_MAX-1, fp)) {
        if (!(line[0] == '#' || line[0] == ';' || strlen(line) < 2)) {/* skipcomment */
            
            arg1 = NULL;
            hex = FALSE;
            value = -1;
            temp = -1;

            /* trim white space and any CR or LF at the end of the line */
            end = line + strlen(line) - 1; /* Point to the last non-null character in the string */
            while (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')
                end--;

            *(end+1) = '\0';
            
            /* If line is only whitespace we continue to the next line */
            if (strlen(line) == 0)
                continue;

            /* trim leading whitespace from the line and find command */
            beg = line;
            
            while (*beg == ' ' || *beg == '\t')
                beg++;

            cmd = beg; /* command starts here */

            while (*beg != ' ' && *beg != '\t' && *beg != '=' && *beg != '\0')
                beg++;
            
            *beg = '\0'; /* command string terminates here */

            /* trim space between command and argument */
            beg++;

            if (strlen(beg) > 0) {
                while (*beg == ' ' || *beg == '\t' || *beg == '=' || *beg == '\n' || *beg == '\r')
                    beg++;
                

                /* If argument is in "" we will strip them off
                   It is important that we can use "" so that we can use
                   leading spaces in text_left and text_right */
                if ((beg[0]=='"' && beg[strlen(beg)-1]=='"') ||
                    (beg[0]=='\'' && beg[strlen(beg)-1]=='\'')) {
                    beg[strlen(beg)-1]='\0';
                    beg++;
                }
                
                arg1 = beg; /* Argument starts here */
            }
            /* else arg1 stays null pointer */

            if (strstr(beg, "0x") != NULL) 
            {
               beg = strstr(beg, "0x") + 2;
               hex = TRUE;
            }
            value = valid_integer(beg, hex);
            if (value >= 0) 
            {
                if (strncmp(cmd,"pin_r", strlen(cmd)) == 0)      data.pin_r = value;
                else if (strncmp(cmd,"pin_g", strlen(cmd)) == 0) data.pin_g = value;
                else if (strncmp(cmd,"pin_b", strlen(cmd)) == 0) data.pin_b = value;

                else if (strncmp(cmd,"rake_r", strlen(cmd)) == 0) data.rake_r = value;
                else if (strncmp(cmd,"rake_g", strlen(cmd)) == 0) data.rake_g = value;
                else if (strncmp(cmd,"rake_b", strlen(cmd)) == 0) data.rake_b = value;
                
                else if (strncmp(cmd,"rakex1", strlen(cmd)) == 0) data.rakex[0] = value;
                else if (strncmp(cmd,"rakex2", strlen(cmd)) == 0) data.rakex[1] = value;
                else if (strncmp(cmd,"rakey1", strlen(cmd)) == 0) data.rakey[0] = value;
                else if (strncmp(cmd,"rakey2", strlen(cmd)) == 0) data.rakey[1] = value;
                
                else if (strstr(cmd, "pinx") != NULL) 
                {
                    str = strstr(cmd, "pinx") + 4;
                    temp = valid_integer(str, FALSE);
                    if (temp > 0 && temp <= PINS)  data.pinx[temp - 1] = value;
                }
                else if (strstr(cmd, "piny") != NULL) 
                {
                    str = strstr(cmd, "piny") + 4;
                    temp = valid_integer(str, FALSE);
                    if (temp > 0 && temp <= PINS)  data.piny[temp - 1] = value;
                }
            }

//            cnt = conf_cmdparse(cnt, cmd, arg1);
        }
    }
    
    fclose(fp);
printf("OB: Initialized finished normally\n");
    return FALSE;
}

void send_data( int data )
{
    int error = 0;
    int i;
    char line[LINE_LENGTH];

    // Clean the output buffer for the pipe
    for (i = 0; i < LINE_LENGTH; i++) line[i] = '\0';
    
    // open the FIFO (named pipe)
    pipe_fd = open(PIPE_NAME,  O_WRONLY);
//    pipe_fd = open(PIPE_NAME,  O_WRONLY | O_NONBLOCK);
    error = errno;
    if (pipe_fd == -1) 
    {
        if (error == ENXIO) return;

        printf("OB: ERROR: pipe open failed: %d\n", error);

        switch (error)
        {
            case EACCES:
                printf("OB: ERROR: pipe failure: The  requested access to the file is not allowed\n"); 
                break;
             case EFBIG:
             case EOVERFLOW:
                printf("OB: ERROR: pipe failure: file that is too large to be opened\n"); 
                break;
            case EROFS:
                printf("OB: ERROR: pipe failure: read-only file system and write access was requested\n"); 
                break;
            case ETXTBSY:
                printf("OB: ERROR: pipe failure: executable image which is currently being executed\n"); 
                break;
            case ENOMEM:
                printf("OB: ERROR: pipe failure: Insufficient kernel memory was available\n"); 
                break;
            case ENFILE:
                printf("OB: ERROR: pipe failure: The system limit on the total number of open files has been reached\n"); 
                break;
            case EINTR:
                printf("OB: ERROR: pipe failure: While blocked waiting to complete an open of a slow device\n"); 
                break;
            default:
                printf("OB: ERROR: pipe failure: Unknown error\n");
                break;
        }
    }
    else
    {
        sprintf(line, "%X", data);
printf("SENDING to PIPE: %s\n", line);
        write(pipe_fd, line, strlen(line));
        

        // close the pipe
        close(pipe_fd);
     }
}

/* Convert the YUV420 format to RBG pixels (not complete RBT format)
 */
int  convert2rgb(unsigned char *image, int width, int height)
{
    int x, y, r, g, b;
    unsigned char *l = image;
    unsigned char *u = image + width * height;
    unsigned char *v = u + (width * height) / 4;
    
    if ((width * height) > (MAX_X * MAX_Y)) 
    {
        return FALSE;
    }

    /* Convert to RGB values */
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            r = 76283* (((int)*l) - 16) + 104595 * (((int)*u) - 128);
            g = 76283* (((int)*l) - 16) - 53281  * (((int)*u) - 128) - 25625*(((int)*v)-128);
            b = 76283* (((int)*l) - 16) + 132252 * (((int)*v) - 128);
            r = r>>16;
            g = g>>16;
            b = b>>16;
            if (r < 0)
                r = 0;
            else if (r > 255)
                r = 255;
            if (g < 0)
                g = 0;
            else if (g > 255)
                g = 255;
            if (b < 0)
                b = 0;
            else if (b > 255)
                b = 255;

            /* Store RGB in global data buffer */
            rgb_map[x][y] = (r << 16) + (g << 8) + b;

            l++;
            if (x & 1) {
                u++;
                v++;
            }
        }

        if (y & 1) {
            u -= width / 2;
            v -= width / 2;
        }
    }
    return TRUE;
}

int  get_red  (int x, int y)
{
    return ((rgb_map[x][y] & 0x00FF0000) >> 16);
}

int  get_green(int x, int y)
{
    return ((rgb_map[x][y] & 0x0000FF00) >> 8);
}

int  get_blue (int x, int y)
{
    return (rgb_map[x][y] & 0x000000FF);
}

void analyze_frame( void )
{
    static enum states state = unknown;
    static int frame = 0;

    frame++;
    
// pins_down(), is_rake_down()    
    switch (state)
    {
        // Starting state - see how to start things
        case unknown:
        {
            if (is_rake_down() == TRUE) state = rake_down;
            else state = ready_for_ball;
            frame = 0;
            break;
        }
        // Wait for the rake to drop
        case ready_for_ball:
        {
            if (is_rake_down() == TRUE) 
            {   
                send_data ( pins_down() ); // send data to other process
                state = rake_down;
                frame = 0;
            }
            break;
        }
        // Wait for the sweep to begin
        case rake_down:
        {
            // FIXME: remove magic number - use something in config file / default
            if ((is_rake_down() == FALSE) && (frame > 6)) 
            {
                state = sweeping;
                frame = 0;
            }
            break;
        }
        // Wait for the rake to return
        case sweeping:
        {
            // FIXME: remove magic number - use something in config file / default
            if ((is_rake_down() == TRUE) && (frame > 6)) 
            {
                state = rake_up;
                frame = 0;
            }
            break;
        }
        // Cycle is complete, get ready for the next ball
        case rake_up:
        {
            if (is_rake_down() == FALSE) state = ready_for_ball;
            break;
        }
        
        default:
            state = unknown;
            break;
    }
printf ("%2d, Pins: %4.4X, rake: %4s, state: %d\n", frame, pins_down(), 
is_rake_down() == TRUE ? "DOWN" : "UP", state);
 
//    send_data ( get_red(320, 240) );
   
}

int  pins_down( void )
{
    int value = 0;
    int counter = 0;
    int i = 0;

    // Count each pin RGB point values
    for (i = 0; i < PINS; i++)
    {
        counter = 0;
        
        // Actual RGB value is brighter (pin up) than the cut-off value
        if (data.pin_r <= get_red  (data.pinx[i], data.piny[i])) counter++;
        if (data.pin_g <= get_green(data.pinx[i], data.piny[i])) counter++;
        if (data.pin_b <= get_blue (data.pinx[i], data.piny[i])) counter++;

        // If pin is down add to the bit position, bits are in decending order:
        //   Examples: pin 1 up = 1000000000 or 0x0200 or  512 decimal
        //     pins 1, 5, 10 up = 1000100001 or 0x0221 or  545 decimal
        //          all pins up = 1111111111 or 0x03FF or 1023 decimal
        if (counter >= 2) 
        {
            value = value + (0x01 << (PINS - i - 1));
        }
// FIXME: test red head pins below
//printf("PINS: %2.2d, value: 0x%3.3X, %s, RBG: %6.6X\n", i+1, value, 
//counter >= 2 ? "UP" : "DN", rgb_map[data.pinx[i]][data.piny[i]]);
    }

    return value;
}

int  is_rake_down( void )
{
    int value = FALSE;
    int counter = 0;
    int i = 0;

    // Count all rake RGB point values
    for (i = 0; i < RAKE; i++)
    {
        // Actual RGB value is darker (rake down) than the cut-off value
        if (data.rake_r >= get_red  (data.rakex[i], data.rakey[i])) counter++;
        if (data.rake_g >= get_green(data.rakex[i], data.rakey[i])) counter++;
        if (data.rake_b >= get_blue (data.rakex[i], data.rakey[i])) counter++;
    }
    
    // Compare all rake point values (throw one out - why not...)
    if (counter >= (RAKE * 3) - 1) 
    {
        value = TRUE;
    }
        
    return value;
}

/* Convert the YUV420 format to RBG pixels (not complete RBT format)
 */
void ob_process_data(unsigned char *image, int width, int height)
{
    if ( initialized() ) return;

    // convert the image 
    if (convert2rgb( image, width, height ) == FALSE) return;

    analyze_frame();
}

void ob_data_testing( void )
{
    glob_t data;

    if ( initialized() ) return;
    
    switch( glob("./test3/*.jpg", 0, NULL, &data ) )
    {
        case 0:
            break;
        case GLOB_NOSPACE:
            printf( "ERROR: Out of memory\n" );
            break;
        case GLOB_ABORTED:
            printf( "ERROR: Reading error\n" );
            break;
        case GLOB_NOMATCH:
            printf( "ERROR: No files found\n" );
            break;
        default:
            break;
    }
    
    int i;
    for(i=0; i<data.gl_pathc; i++)
    {
//        printf( "%s:\n", data.gl_pathv[i] );
        if (!readrgbfile(data.gl_pathv[i]))
        {
//        printf("%s:  Pin3: %X\n", data.gl_pathv[i], raw_image[365 * 154 * 3]);
//            printf("%s: Pin 3: R %X, G %X, B %X\n", data.gl_pathv[i],
//                get_red(365, 154), get_green(365, 154), get_blue(365, 154));
        }
//        sleep(1); // do one file each second
        
//        printf("%s: Pins: 0x%3.3X, Rake: %4s\n", data.gl_pathv[i],
//            pins_down(), is_rake_down() == TRUE ? "DOWN" : "UP"); 
        analyze_frame();

    }
    
    globfree( &data );
}

/* Convert the YUV420 format to RBG pixels (not complete RBT format)
 */
int readrgbfile(char *filename)
{
	/* these are standard libjpeg structures for reading(decompression) */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	JSAMPROW row_pointer[1];
	
	FILE *infile = fopen( filename, "rb" );
	int i = 0, y = 0;
	
	if ( !infile )
	{
		printf("Error opening jpeg file %s\n!", filename );
		return TRUE;
	}
	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error( &jerr );
	/* setup decompression process and source, then read JPEG header */
	jpeg_create_decompress( &cinfo );
	/* this makes the library read from infile */
	jpeg_stdio_src( &cinfo, infile );
	/* reading the image header which contains image information */
	jpeg_read_header( &cinfo, TRUE );
	/* Uncomment the following to output image information, if needed. */
	/*--
	printf( "JPEG File Information: \n" );
	printf( "Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height );
	printf( "Color components per pixel: %d.\n", cinfo.num_components );
	printf( "Color space: %d.\n", cinfo.jpeg_color_space );
	--*/
	/* Start decompression jpeg here */
	jpeg_start_decompress( &cinfo );

	/* allocate memory to hold the uncompressed image */
//	raw_image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
    if (cinfo.output_width > MAX_X || cinfo.image_height > MAX_Y)
	{
		printf("ERROR: jpg too large: X: %d, Y: %d\n", cinfo.output_width, cinfo.image_height);
		return TRUE;
	}

	/* now actually read the jpeg into the raw buffer */
	row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );

	/* read one scan line at a time */
	y = 0;
	while( cinfo.output_scanline < cinfo.image_height )
	{
        jpeg_read_scanlines( &cinfo, row_pointer, 1 );
		for( i=0; i<cinfo.image_width; i++) 
        {
            rgb_map[i][y] = ((row_pointer[0][i*3]) << 16) + 
                ((row_pointer[0][i*3+1]) << 8) + 
                 row_pointer[0][i*3+2];
//printf("%3d %3d %3d\n", row_pointer[0][i], row_pointer[0][i], row_pointer[0][i]);
//printf("%3d: %X = %3d %3d %3d\n", i, rgb_map[x][i], 
//get_red(x, i), get_green(x, i), get_blue(x, i));
//			raw_image[location++] = row_pointer[0][i];
        }
        y++;
//printf("\n");
//return FALSE;
	}

	/* wrap up decompression, destroy objects, free pointers and close open files */
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	free( row_pointer[0] );
	fclose( infile );
	/* yup, we succeeded! */
	return FALSE;
}



