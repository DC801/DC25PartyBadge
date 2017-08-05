/**
 * @file bmp-convert.h
 * @data June 2017
 * @author hamster, others
 */

#include <stdint.h>

typedef struct __attribute__((__packed__)) tagBITMAPFILEHEADER{
	uint16_t 	bfType;  		/* should be two characters, 'BM" */
	uint32_t 	bfSize;			/* Size of file in bytes */
	uint16_t 	bfReserved1;	/* 0 */
	uint16_t 	bfReserved2;	/* 0 */
	uint32_t	bfOffBits;		/* Offset to the start of the image data */
} BITMAPFILEHEADER;


typedef struct __attribute__((__packed__)) tagRGBQUAD{
	/* rgbq */
    uint8_t    rgbBlue;
    uint8_t    rgbGreen;
    uint8_t    rgbRed;
    uint8_t    rgbReserved;
} RGBQUAD;


typedef struct __attribute__((__packed__)) tagBITMAPINFOHEADER
{ /* bmih */
	uint32_t  	biSize;			/* Specifies the number of bytes
								 * required by the structure. */
	uint32_t   	biWidth;		/* Specifies the width of the bitmap, in pixels. */
	uint32_t   	biHeight;		/* Specifies the height of the bitmap, in pixels. */
								/* If biHeight is positive, the bitmap is a
								 * bottom-up DIB and its origin is the lower left
								 * corner. If biHeight is negative, the bitmap is
                                 * a top-down DIB and its origin is the upper
                                 * left corner. */
	uint16_t   	biPlanes;       /* Specifies the number of planes for the target
   								 * device. This value must be set to 1. */
	uint16_t   	biBitCount;     /* Specifies the number of bits per pixel. This
   								 * value must be 1, 4, 8, 16, 24, or 32. */
	uint32_t  	biCompression;  /* Specifies the type of compression for a
   								 * compressed bottom-up bitmap (top-down DIBs
                                 * cannot be compressed). It can be one of the
                                 * following values:
   						         * Value			Description
								 * BI_RGB			An uncompressed format.
								 *
								 * BI_RLE8			A run-length encoded (RLE) format for
                                 *          		bitmaps with 8 bits per pixel. The compression
                                 *          		format is a two-byte format consisting of a count
                                 *          		byte followed by a byte containing a color index.
                                 *
                                 * BI_RLE4			An RLE format for bitmaps with 4 bits per
                                 *          		pixel. The compression format is a two-byte format
                                 *          		consisting of a count byte followed by two word-
                                 *          		length color indices.
								 *
								 * BI_BITFIELDS		Specifies that the bitmap is not
                              	 *					compressed and that the color table consists of
                              	 *					three doubleword color masks that specify the red,
                              	 *					green, and blue components, respectively, of each
                              	 *					pixel. This is valid when used with 16- and
                              	 *					32-bits-per-pixel bitmaps.*/
	uint32_t  	biSizeImage;   	/* Size of the image, in bytes. Can be 0 if the
   								 * bitmap is in the BI_RGB format. */

	uint32_t   	biXPelsPerMeter;/* Specifies the horizontal resolution, in pixels
                                 * per meter, of the target device for the bitmap.
                                 * Use this value to select a bitmap from a resource
                                 * group that best matches the characteristics of
                                 * the current device. */

	uint32_t   	biYPelsPerMeter;/* Specifies the vertical resolution, in pixels per
   								 * meter, of the target device. */
	uint32_t  	biClrUsed;      /* Specifies the number of color indexes in the color
   								 * table actually used by the bitmap. If this value
                                 * is 0, the bitmap uses the maximum number of colors
                                 * corresponding to the value of the biBitCount member.
								 * For 16-bit applications, if biClrUsed is nonzero,
                                 * it specifies the actual number of colors that the
                                 * graphics engine or device driver will access if
                                 * the biBitCount member is less than 24.
                                 * If biBitCount is set to 24, biClrUsed specifies
                                 * the size of the reference color table used to
                                 * optimize performance of color palettes.
								 * For 32-bit applications, if biClrUsed is nonzero
                                 * and the biBitCount member is less than 16, the
                                 * biClrUsed member specifies the actual number of
                                 * colors the graphics engine or device driver accesses.
                                 * If biBitCount is 16 or greater, then biClrUsed
                                 * member specifies the size of the color table used
                                 * to optimize performance of Windows color palettes.
                                 * For biBitCount equal to 16 or 32 the optimal
                                 * color palette starts immediately following
                                 * the 3 DWORD masks.
								 * If the bitmap is a packed bitmap (that is, a
                                 * bitmap in which the bitmap array immediately
                                 * follows the BITMAPINFO header and which is
                                 * referenced by a single pointer), the biClrUsed
                                 * member must be set to zero or to the actual
                                 * size of the color table. */

	uint32_t  	biClrImportant; /* Specifies the number of color indexes that are
   								 * considered important for displaying the bitmap.
                                 * If this value is zero, all colors are important.*/
} BITMAPINFOHEADER;


typedef struct tagBITMAPINFO{
	/* bmi */
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD          bmiColors[1];
} BITMAPINFO;

typedef union tagPixelData{
    struct{
    	uint16_t red:4;
    	uint16_t green:4;
    	uint16_t blue:4;
    	uint16_t unused:4;
    } color754xx;
    struct{
    	uint16_t red:5;
    	uint16_t green:5;
    	uint16_t blue:5;
    	uint16_t unused:1;
    } color555;
    struct{
    	uint16_t red:5;
    	uint16_t green:6;
    	uint16_t blue:5;
    } color565;
    struct{
    	uint32_t red:8;
    	uint32_t green:8;
    	uint32_t blue:8;
    	uint32_t alpha:8;
    } color888;
    uint32_t pixel32;
    uint16_t pixel16[2];
} PixelData;

