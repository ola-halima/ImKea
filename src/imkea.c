//
//  ImKea.c
//  HHP Interior Decor & Design
//
//  Created by Heidi Muth, Persis Finny, Halima Olapade on 3/3/14.
//  Copyright (c) 2014 ImKea. All rights reserved.
//

#include <stdio.h>
#include "google-img.h"

int main(int argc, const char * argv[])
{
   //char** images;
   //int nDownloaded = get_images(&images, argv[1], atoi(argv[2]));

    int numberOfImages, // the number of images loaded to make the mosaic
			numColumns, // the number of horizontal images in the result
			numRows, // the number of vertical images in the result
			i; // just an iterator
	IplImage **timages, // the small images loaded from the disk
            **subImages; // small images created from the source image
	CvScalar *scolors; // the average color of the sub images of the source image
	//colorGroup *cGroup; // the resulting group of colors in images
	pixelColor *pColor; // the pixel colors possible
    int pSize = 0;
    char* res; //resulting string to search

	// get the number of rows and columns
	//numColumns = atoi(argv[2]);
	//numRows = atoi(argv[3]);
	numColumns = 30;
	numRows = 30;

	// make sure we have an even separation
	//assert(src->width%numColumns==0);
	//assert(src->height%numRows==0);

	// The number of images passed in (aka nDownloaded)
    numberOfImages = get_images(&images, argv[1], atoi(argv[2]));;

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


    return 0;
}

