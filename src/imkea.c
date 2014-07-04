/*
 * imkea.c
 * HHP Interior Decor & Design
 *
 *  Created by Heidi Muth, Persis Finny, Halima Olapade
 *  Copyright (c) 2014 ImKea. All rights reserved.
 */

#include "google-img.h"
#include "imkea-img.h"
#include "img-color-map.h"

#define VIS_SIZE 10

int main(int argc, char ** argv) {
    char *searchTerm; // user input, what to search for that will return images to analyze
    int numberOfImages, // the number of images loaded to analyze colors
            numColumns, // the number of horizontal subimages per image to analyze
            numRows, // the number of vertical subimages per image to analyze
            i;
    char **images; // filenames of images downloaded to analyze
    IplImage **timages, // Loaded IplImages from char** images
            **subImages; // small sub images created from the source image
    CvScalar *scolors; // the average colors of the sub images of the source image
    pixelColor *pColor; // the pixel colors possible
    int pSize = 0;      //total number of pixel colors in all images analyzed
    char* design_query; //resulting query to pass to search engine to find designs

    searchTerm = argv[1];

    // Do image search for searchTerm, download images to analyze
    printf("Downloading images...");
    numberOfImages = download_images(&images, searchTerm, 5);
    if (numberOfImages == 0) {
        fprintf(stderr, "No images downloaded.\n");
        return EXIT_FAILURE;
    }

    // Load downloaded images
    printf("Loading images into analyzer...\n");
    if ((timages = loadImages(&numberOfImages, images)) == NULL ) {
        fprintf(stderr, "Could not load images.");
        return EXIT_FAILURE;
    }

    // number of rows and columns for subimages
    numColumns = 30;
    numRows = 30;
    if((pColor = malloc(sizeof(pixelColor)*36)) == NULL) { //create space for all possible colors
        fprintf(stderr, "Error allocating memory for possible color list.\n");
        return EXIT_FAILURE;
    }

    for(i = 0; i < numberOfImages; ++i){
        // create the sub images
        printf("Getting subimages of %s\n", images[i]);
        subImages = getSubImages(timages[i], numColumns, numRows);

        // compute the average colors of the sub images
        printf("Getting avg colors of %s\n", images[i]);
        scolors = getAvgColors(subImages, numColumns * numRows);
        
        // assign found dominant colors to image
        printf("Assigning colors for %s\n\n", images[i]);
        if(assignColor(scolors, &pColor, &pSize, numColumns * numRows) != 0) { // map RGB to color name
            perror("Error mapping RGB to general color");
        }
    }
    
    // find most dominant, generate query string
    design_query = findClosest(pColor, pSize);
    printf("Colors in the images are:\n");
    for(i = 0; i < pSize; ++i){
        printf("- %s : %d\n", pColor[i].name, pColor[i].numOccur);
    }
    printf("final query string: %s\n\n", design_query);

    printf("You searched for: %s\n", searchTerm);

    // search for designs according to query string 
    open_image_search(design_query);
    printf("\n");
    
    return EXIT_SUCCESS;
}
