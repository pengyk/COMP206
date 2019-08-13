/* FILE: A3_solutions.c is where you will code your answers for Assignment 3.
 * 
 * Each of the functions below can be considered a start for you. They have 
 * the correct specification and are set up correctly with the header file to
 * be run by the tester programs.  
 *
 * You should leave all of the code as is, especially making sure not to change
 * any return types, function name, or argument lists, as this will break
 * the automated testing. 
 *
 * Your code should only go within the sections surrounded by
 * comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A3_solutions.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "A3_provided_functions.h"

unsigned char*
bmp_open( char* bmp_filename,        unsigned int *width, 
          unsigned int *height,      unsigned int *bits_per_pixel, 
          unsigned int *padding,     unsigned int *data_size, 
          unsigned int *data_offset                                  )
{
  unsigned char *img_data=NULL;
  // REPLACE EVERYTHING FROM HERE

  FILE *bmpfile=fopen( bmp_filename, "rb");
  if( bmpfile == NULL){
    printf("Unable to open file bmp_filename\n");
    return NULL;
  }

  char b, m;
  fread(&b, 1, 1, bmpfile);
  fread(&m, 1, 1, bmpfile);

  //checks if it is a .bmp file
  if( b!='B' || m!='M'){
    printf("This isn't a bmp file.\n");
    return img_data;
  }

  //gets the size of the bmp file
  unsigned int overallFileSize;
  fread( &overallFileSize, 1, sizeof(unsigned int), bmpfile);
  rewind(bmpfile);
 
  //assigns enough memory 
  img_data=(unsigned char*)malloc(overallFileSize);

  //stores in img_data the content of bmpfile
  fread(img_data, 1, overallFileSize, bmpfile);
  fclose(bmpfile);

  //gets the width and stores it inside of width
  //the same method is used for height, data size and data offset
  unsigned int* wp= (unsigned int*)(img_data+18);
  *width = *wp;

  unsigned int* hp = (unsigned int*)(img_data+22);
  *height = *hp;
  
  //the bits per pixel takes only account of the first two bits, so it takes in a short int
  unsigned int* bbp = (unsigned int*)(img_data+28);
  *bits_per_pixel = (unsigned short int) *bbp;

  //finds the padding by modulo 4
  *padding = *width%4;

  unsigned int* data_s = (unsigned int*)(img_data+34);
  *data_size = *data_s;

  unsigned int* data_o =(unsigned int*)(img_data+10);
  *data_offset = *data_o;
  // TO HERE!  
  return img_data;  
}

void 
bmp_close( unsigned char **img_data )
{
  // REPLACE EVERYTHING FROM HERE
  free( *img_data );
 *img_data=NULL;
  // TO HERE!  
}

unsigned char***  
bmp_scale( unsigned char*** pixel_array, unsigned char* header_data, unsigned int header_size,
           unsigned int* width, unsigned int* height, unsigned int num_colors,
           float scale )
{
  unsigned char*** new_pixel_array = NULL; 
  // REPLACE EVERYTHING FROM HERE

  //calculates the scaled width and height
  *width=(unsigned int)(*width*scale);
  *height=(unsigned int)(*height*scale);

  //finds the file size
  unsigned int* fs=(unsigned int*)(header_data+2);

  //changes the header's width and height
  unsigned int* hwidth=(unsigned int*)(header_data+18);
  *hwidth=*hwidth*scale;
  unsigned int* hheight=(unsigned int*)(header_data+22);
  *hheight = *hheight *scale;

  //changes the file size
  *fs=(*height)*(*width)*(num_colors)+header_size+(*width)%4*(*height);
  
  //allocates enough memory for the new scaled array
  new_pixel_array = (unsigned char***)malloc( sizeof(unsigned char**) * (*height));
  if( new_pixel_array == NULL ){
    printf( "Error: bmp_to_3D_array failed to allocate memory for image of height %d.\n", (*height) );
    return NULL;
  }
  
  for( int row=0; row<*height; row++ ){
    new_pixel_array[row] = (unsigned char**)malloc( sizeof(unsigned char*) * (*width) );
    for( int col=0; col<*width; col++ ){
      new_pixel_array[row][col] = (unsigned char*)malloc( sizeof(unsigned char) * (num_colors) );
    }
  }

  //assigns to new pixel array the content(image) of the old pixel array
  for( int row=0; row<*height; row++ )
    for( int col=0; col<*width; col++ )
      for( int color=0; color<num_colors; color++ )
        new_pixel_array[row][col][color] = pixel_array [(int)(row/scale)][(int)(col/scale)][color];
  // TO HERE! 
  return new_pixel_array;
}         

int 
bmp_collage( char* background_image_filename,     char* foreground_image_filename, 
             char* output_collage_image_filename, int row_offset,                  
             int col_offset,                      float scale )
{
// REPLACE EVERYTHING FROM HERE

//initalizes all variables of the background
unsigned char*   back_header_data;
unsigned int     back_header_size, back_width, back_height, back_num_colors;
unsigned char*** back_pixel_array = NULL;

//gives out the 3d array of the background
back_pixel_array = bmp_to_3D_array( background_image_filename,       &back_header_data, 
                                    &back_header_size,  &back_width, 
                                    &back_height,       &back_num_colors   );

if(back_pixel_array==NULL){ 
    printf("background array is empty\n"); 
    return -1;
  }

//initalizes all variabels of the foreground
unsigned char*   fore_header_data;
unsigned int     fore_header_size, fore_width, fore_height, fore_num_colors;
unsigned char*** fore_pixel_array = NULL;
  
//gives te 3d array of the foreground
fore_pixel_array = bmp_to_3D_array(foreground_image_filename,       &fore_header_data, 
                                   &fore_header_size,  &fore_width, 
                                   &fore_height,       &fore_num_colors   );
if(fore_pixel_array==NULL){ 
    printf("foreground array is empty\n");
    return -1;
  }

//gives a scaled version of the foreground array
unsigned char*** fore_scaled_pixel_array = bmp_scale( fore_pixel_array, fore_header_data, fore_header_size,
                                                   &fore_width, &fore_height, fore_num_colors,  scale   );  
if(fore_scaled_pixel_array==NULL) {
    printf("scaled foreground is empty\n");
    return -1;
  }

//assigns to the foregorund array the scaled version
fore_pixel_array=fore_scaled_pixel_array;

//loops through all the zone covered by the foreground on the background
//if the colour value of the foreground is 0, it means that it is transparent-than the background stays the same
//if it isn't 0, then change the colour of the background at a certain row and colum to those of the foreground
for(int row=row_offset; row<fore_height+row_offset; row++){
    for(int col=col_offset; col<fore_width+col_offset; col++){
      for(int colour=0; colour<fore_num_colors; colour++){
        if(fore_pixel_array[row-row_offset][col-col_offset][colour]==0){
          break;
          }
        else{    
           back_pixel_array[row][col][colour]=fore_pixel_array[row-row_offset][col-col_offset][colour];
           }
      }
    }
  }

//gives to output collage iamge filename the bmp format of the 3d array back pixel array
bmp_from_3D_array(output_collage_image_filename, back_header_data,
                 back_header_size, back_pixel_array, 
                 back_width, back_height, back_num_colors);

  return 0;
}              

