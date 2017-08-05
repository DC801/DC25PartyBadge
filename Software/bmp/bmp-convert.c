/**
 *
 * @file bmp-convert.c
 * @data June 2017
 * @author hamster, others
 *
 * This is a utility for converting a bmp to a rgb565 image
 * It has bounds checking to fit our screen
 *
 * The code was based upon software found at
 * https://www.lpcware.com/content/nxpfile/bmp-c-image-conversion-utility
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include "bmp-convert.h"

#define MAX_WIDTH 128
#define MAX_HEIGHT 128

/**
 * Convert a bmp to a 16 bit RGB565 header
 */
int main(int argc, char * argv[]){

	FILE *infile, *outfile;
	char *outname;
	BITMAPFILEHEADER bf;
	BITMAPINFO bmi;
	uint32_t bytes_read = 0, bmpsize;
	PixelData *databuffer;

	// Bail if we don't have an input and output
	if(argc < 3){
		printf("Usage: %s <src.bmp> <out.h>\n", argv[0]);
		exit(-1);
	}

    infile = fopen(argv[1], "rb");
    if (!infile){
		printf("Error opening input file %s\n", argv[1]);
		exit(-1);
	}


    // Figure out the string we're going to use in the output
    // Replace spaces or dots with an underscore
    outname = basename(argv[1]);
    int i = 0;
    while(outname[i] != '\0'){
    	if(outname[i] == ' ' || outname[i] == '.'){
    		outname[i] = '_';
    	}
    	i++;
    }

    printf(" %s: %s\n", argv[1], outname);

    // Parse the header
	if (fread(&bf,sizeof(bf),1,infile) == 1){
        bytes_read += sizeof(bf);
        printf("  %d bytes\n", bf.bfSize);
    }
	else{
		printf("Error reading BMP info\n");
		fclose(infile);
		exit(-1);
	}

    if (fread(&bmi,sizeof(bmi.bmiHeader),1,infile) == 1){
        bytes_read += sizeof(bmi.bmiHeader);
        printf("  %dx%d size\n", bmi.bmiHeader.biHeight, bmi.bmiHeader.biWidth);
    }
	else{
		printf("Error reading BMP header\n");
		fclose(infile);
		exit(-1);
	}

    if(bmi.bmiHeader.biHeight > MAX_HEIGHT || bmi.bmiHeader.biWidth > MAX_WIDTH){
    	printf("Error image is too large for the 128x128 screen\n");
    	exit(-1);
    }

    printf("  %d bits per pixel\n", bmi.bmiHeader.biBitCount);

	if (bmi.bmiHeader.biBitCount != 24){
		printf("Only 24bpp BMP files are supported\n");
		fclose(infile);
		exit(-1);
	}

	// Allocate buffer storage
	bmpsize = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight;
	databuffer = (PixelData *) malloc(sizeof(PixelData) * bmpsize);
	if (databuffer == NULL)
	{
		printf("Error allocating temporary data buffer, is image too big?\n");
		fclose(infile);
		exit(-1);
	}

    // Skip to the start of the image data
	fseek(infile, bf.bfOffBits, SEEK_SET);

	// Store the BMP data to an array, in RGB565 format
	for(int i = 0; i < bmpsize; i++){
		uint8_t rgb[3];

		if (fread(&rgb, sizeof(rgb), 1, infile) != 1){
			printf("error reading BMP data\n");
 		    fclose(infile);
			free(databuffer);
	        exit(-1);
   	    }

		databuffer[i].color565.blue = rgb[0] >> 3;
		databuffer[i].color565.red = rgb[1] >> 3;
		databuffer[i].color565.green = rgb[2] >> 2;

	}

	fclose(infile);

	// Write it out to a file
	outfile = fopen(argv[2], "wb");
	if (outfile == NULL){
		printf("error opening out file %s\n", argv[2]);
		free(databuffer);
	    exit(-1);
	}

    fprintf(outfile, "\n\n");
    fprintf(outfile, "/**\n");
    fprintf(outfile, " * Original file: %s\n", argv[1]);
    fprintf(outfile, " * Bytes: %d\n", bf.bfSize);
    fprintf(outfile, " * Size: %dx%d\n", bmi.bmiHeader.biHeight, bmi.bmiHeader.biWidth);
    fprintf(outfile, " */\n\n");

	// Image data as 2 byte RGB565
	fprintf(outfile, "static const uint16_t %s[] = {\n    ", outname);

	int fillindex = 0;
	for(int i = 0; i < bmpsize; i++){
		if (fillindex >= 72){
			fprintf(outfile, ",\n    ");
			fillindex = 0;
		}
		else if (i != 0){
			fprintf(outfile, ", ");
		}

		fprintf (outfile, "0x%04x", databuffer[i].pixel16[0]);
		fillindex += 9;
	}

	fprintf (outfile, "\n};");
	free(databuffer);
	fclose(outfile);

	printf("  written to %s\n", argv[2]);

    return 0;

}
