#include "bitmap.h"
#include "smiley.h"
#include "i8042.h"
#include "i8254.h"
#include "stdio.h"
#include <minix/drivers.h>

#include "video_gr.h"
#include "vbe.h"

Bitmap* loadBitmap(const char* filename) {
    // allocating necessary size
    Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

    // open filename in read binary mode
    FILE *filePtr;
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read the bitmap file header
    BitmapFileHeader bitmapFileHeader;
    fread(&bitmapFileHeader, 2, 1, filePtr);

    // verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.type != 0x4D42) {
        fclose(filePtr);
        return NULL;
    }

    int rd;
    do {
        if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
            break;
    } while (0);

    if (rd = !1) {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    unsigned char* bitmapImage = (unsigned char*) malloc(
            bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;

    return bmp;
}

void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment) {
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > 640 || y + height < 0 || y > 480)
        return;


    int xCorrection = 0;
    if (x < 0) {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > 640)
            drawWidth = 480;
    } else if (x + drawWidth >= 640) {
        drawWidth = 640 - x;
    }

    unsigned short* bufferStartPos;
    unsigned short* imgStartPos;

    int i,j;
    for (i = 0; i < height; i++) {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= 480)
            continue;

        bufferStartPos = (unsigned short*) get_video_mem();
        bufferStartPos += x  + pos * 640 ;

        imgStartPos = (unsigned short*) bmp->bitmapData + xCorrection  + i * width; //* 2;

        for (j = 0; j < width; j++)
        {
            if (*imgStartPos != 0x07E0)
            {
                *bufferStartPos = *imgStartPos;
            }
            bufferStartPos++;
            imgStartPos++;
        }

    }
}

void drawBitmap1(Bitmap* bmp, int x, int y, Alignment alignment) {
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > 640 || y + height < 0 || y > 480)
        return;


    int xCorrection = 0;
    if (x < 0) {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > 640)
            drawWidth = 480;
    } else if (x + drawWidth >= 640) {
        drawWidth = 640 - x;
    }

    unsigned short* bufferStartPos;
    unsigned short* imgStartPos;

    int i,j;
    for (i = 0; i < height; i++) {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= 480)
            continue;

        bufferStartPos = (unsigned short*) get_double_buffer();
        bufferStartPos += x  + pos * 640 ;

        imgStartPos = (unsigned short*) bmp->bitmapData + xCorrection  + i * width; //* 2;

        for (j = 0; j < width; j++)
        {
            if (*imgStartPos != 0x07E0)
            {
                *bufferStartPos = *imgStartPos;
            }
            bufferStartPos++;
            imgStartPos++;
        }

    }
}




void deleteBitmap(Bitmap* bmp) {
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}
