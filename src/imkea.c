/*
 * imkea.c
 * HHP Interior Decor & Design
 *
 *  Created by Heidi Muth, Persis Finny, Halima Olapade
 *  Copyright (c) 2014 ImKea. All rights reserved.
 */

// Images from http://press.liacs.nl/mirflickr/#sec_download
#include "imkea-img.h"
#include "img-color-map.h"

#define VIS_SIZE 10

int main(int argc, char ** argv) {
	int numberOfImages, // the number of images loaded to make the mosaic
			numColumns, // the number of horizontal images in the result
			numRows, // the number of vertical images in the result
			i; // just an iterator
	IplImage **timages, // the small images loaded from the disk
            **subImages; // small images created from the source image
	CvScalar *scolors; // the average color of the sub images of the source image
	pixelColor *pColor; // the pixel colors possible
    int pSize = 0;
    char* res; //resulting string to search

	// number of rows and columns for subimages
	numColumns = 30;
	numRows = 30;

    // The number of images passed in
    numberOfImages = argc - 1;

	// Load the thumbnail images
	printf("Loading images\n");
	if ((timages = loadImages(numberOfImages, argv + 1)) == NULL ) {
		fprintf(stderr, "Could not load images!");
		return EXIT_FAILURE;
	}
	
	pColor = NULL;
    pColor = malloc(sizeof(pixelColor) * 36); //create space for all colors

	for(i = 0; i < numberOfImages; ++i){
		// create the sub images
		subImages = getSubImages(timages[i], numColumns, numRows);

		// compute the average colors of the sub images
		//printf("Computing sub image colors\n");
		scolors = getAvgColors(subImages, numColumns * numRows);
		
		//printf("Finding closest images\n");
        
		if(assignColor(scolors, &pColor, &pSize, numColumns * numRows) != 0) // map RGB to color name{
			perror("Error mapping RGB to general color");
		}
    
    printf("Colors in the images are:\n");
    for(i = 0; i < pSize; ++i){
        printf("- %s : %d\n", pColor[i].name, pColor[i].numOccur);
    }
    
    res = findClosest(pColor, pSize);
    printf("Final String: %s", res);
    
	return EXIT_SUCCESS;
}
