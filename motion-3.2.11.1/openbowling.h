/*
 *    openbowling.h
 *
 *      Copyright 2010 by Matt Mead
 *      This software is distributed under the GNU public license version 3
 *      
 */
#ifndef _OPENBOWLING_H_
#define _OPENBOWLING_H_

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

void ob_process_data(unsigned char *image, int width, int height);

int initialized ( void );

void ob_data_testing( void );

#endif /* _OPENBOWLING_H_ */
