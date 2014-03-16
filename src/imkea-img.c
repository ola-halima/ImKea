/*
 * imkea-img.c
 * HHP Interior Decor & Design
 *
 *  Created by Heidi Muth, Persis Finny, Halima Olapade
 *  Copyright (c) 2014 ImKea. All rights reserved.
 */

#include "imkea-img.h"
#include "img-color-map.h"

/**
 * Loads an image (stored as an IplImage struct) for each
 * filename provided.
 * @param numImages		The number of images to load
 * @param filenames		A list of strings of the filenames to load
 */
IplImage** loadImages(int numImages, char ** fileNames) {
	IplImage** rv; // the return result
	int i; // used for looping

	rv = malloc(sizeof(IplImage*) * numImages);

	for(i = 0; i < numImages; ++i){
        IplImage* img = cvLoadImage(fileNames[i], CV_LOAD_IMAGE_COLOR);

        if(!img){
			perror("Error: Unable to load image\n");
			return NULL;
		}
        
        IplImage* finalIm = cvCreateImage(cvSize(600, 600), img->depth, img->nChannels);

        assert(finalIm->width%15==0);
        assert(finalIm->height%15==0);
        
        cvResize(img, finalIm, CV_LOAD_IMAGE_COLOR);
        
		// if it could not, print an error to stderr and return NULL
		rv[i] = finalIm;
	}
	return rv;
}

/**
 * Splits up an image into numColumns by numRows sub-images and returns the results.
 *
 * @param src	The source image to split
 * @param numColumns	The number of columns to split into
 * @param numRows 		The number of rows to split into
 * @returns				A numColumns x numRows array of IplImages
 */
IplImage ** getSubImages(IplImage* src, int numColumns, int numRows) {
	int cellWidth, cellHeight, y, x, i;
	IplImage ** rv;
	CvSize s = cvGetSize(src);

	// Compute the cell width and the cell height
	cellWidth = s.width / numColumns;
	cellHeight = s.height / numRows;

	// Allocate an array of IplImage* s
	rv = malloc(sizeof(IplImage*) * numColumns * numRows);
	if (rv == NULL) {
		fprintf(stderr, "Could not allocate rv array\n");
	}

	// Iterate over the cells
	i = 0;
	for (y = 0; y < s.height; y += cellHeight)
		for (x = 0; x < s.width; x += cellWidth) {
			// Create a new image of size cellWidth x cellHeight and
			// store it in rv[i]. The depth and number of channels should be the same as src.
            rv[i] = cvCreateImage(cvSize(cellWidth, cellHeight), src->depth,
					src->nChannels);
			if (rv[i] == NULL) {
				fprintf(stderr, "Could not allocate image %d\n", i);
			}
			
            // set the ROI of the src image
            cvSetImageROI(src, cvRect(x, y, cellWidth, cellHeight));
			
            // copy src to rv[i] using cvCopy, which obeys ROI
			cvCopy(src, rv[i], NULL);
            
            // reset the src image roi
			cvResetImageROI(src);

			// increment i
			i++;
		}
	return rv;
}
