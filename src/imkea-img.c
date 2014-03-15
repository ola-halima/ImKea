//
//  imkea-img.c
//  HHP Interior Decor & Design
//
//  Created by Heidi Muth, Persis Finny, Halima Olapade on 3/3/14.
//  Copyright (c) 2014 ImKea. All rights reserved.
//

#include "img-color-map.h"
#include <stdio.h>

/**
* Given a name, create a new pixelcolor element initialized to 1 occurrence
* @param name	The name of the color being initialized
* @param rv	A pointer to memory for the pixel color element
*/
void createPixelColor(char* name, pixelColor* rv){
	rv->name = name;
	rv->numOccur = 1;
}

/**
 * Loads an image (stored as an IplImage struct) for each
 * filename provided.
 * @param numImages		The number of images to load
 * @param filenames		A list of strings of the filenames to load
 */
IplImage** loadImages(int numImages, char ** fileNames) {
	IplImage** rv; // the return result
	int i; // used for looping

	// TODO: allocate an array of numImages IplImage* s
	rv = malloc(sizeof(IplImage*) * numImages);

	// TODO: iterate over each filename
	for(i = 0; i < numImages; ++i){
		// TODO: load the image via the cvLoadImage function
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
    // TODO: return results
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

	// return the result
	return rv;
}

/**
 * Finds the CvScalar in colors closest to t using the colorDistance function.
 * @param scolors	The colors to look through
 * @param pColor	Array to store new colors found
 * @param numColors	The length of scolors
 * @returns 		0 on success, -1 otherwise
 */
int assignColor(CvScalar* scolors, pixelColor** pColor, int* pSize, int numColors) {
	//int rv = 0, // return value
    int i,j; // used to iterate
	char* res; //stores the result
    
	// iterate over scolors
	for(i = 0; i < numColors; ++i){
		res = mapColorName(scolors[i]); //retrieve color name
        
        if(res == NULL){
            continue; //no matching color name found
        }
        
        //if color already exists in group
        int colorExists = 0;
        
		for(j = 0; j < *pSize; ++j){
            char* cName = (*pColor)[j].name;
			if(strcmp(res, cName) == 0){
				++((*pColor)[j].numOccur); //increment occurrence of particular color
                colorExists = 1;
				break;
			}
		}
        
        if(colorExists == 1)
            continue;
	
		pixelColor newColor;
		createPixelColor(res, &newColor);
        (*pColor)[(*pSize)++] = newColor;
        
    }
    return 0;
}

/**
 * Given a cvScalar, map its RGB values to a general color name
 * @param scolor	The cvScalar
 * @returns		The char name of the color that best matches RGB
 */
char* mapColorName(CvScalar scolor){
    // TODO: iterate over the file and load each line into a new string.
    FILE* f = fopen("/users/halimaolapade/Desktop/colors/lab/lab_db.txt", "r"); // file descriptor to read from
    int i=0;
    char tok[256];
    double min = -1;
    char* cName = NULL;
    
    while(fscanf(f, "%s", tok) > 0){ //read lines in the file
        char *tmp = strtok(tok, ",");
        char *arg[4];
        i = 0;
        
		while(tmp != NULL && i < 4){ //get RGB values and color name from line read from file
			arg[i] = strdup(tmp);
			++i;
            tmp = strtok(NULL, ",");
		}
        
        double L = atoi(arg[0]);
        double A = atoi(arg[1]);
        double B = atoi(arg[2]);
        char* tmpName = strdup(arg[3]);

        double d = deltaE(scolor, L, A, B);
        
        if(min == -1 && d < 15){
            min = d;
            cName = tmpName;
            continue;
        }
        
        if(d < min && d < 15){
            min = d; //reset current minimum
            cName = tmpName;
        }
    }
    fclose(f);
    return cName; //no matching color found in db
}
/*
 * Converts rgb values to l*ab
 *
 */
void RGB2LAB(double r, double g, double b, double* l1, double* a1, double* b1){
    //rgb to xyz
    r/=255.0;
    g/=255.0;
    b/=255.0;
    
    if (r > 0.04045)
        r = powf((r+0.055)/1.055,2.4);
    else
        r = r/12.92;
    
    if (g > 0.04045)
        g = powf((g+0.055)/1.055,2.4);
    else
        g = g/12.92;
    
    if (b > 0.04045)
         b = powf((b+0.055)/1.055,2.4);
    else
        b = b/12.92;
        
    r = r * 100.0;
    g = g * 100.0;
    b = b * 100.0;
    
    double x = (r * 0.4124) + (g * 0.3576) + (b * 0.1805);
    double y = (r * 0.2126) + (g * 0.7152) + (b * 0.0722);
    double z = (r * 0.0193) + (g * 0.1192) + (b * 0.9505);
    
    //xyz to lab
    x = x/95.047;
    y = y/100.000;
    z = z/108.883;
    
    if (x > 0.008856)
        x = powf(x,0.3333);
    else
        x = (7.787 * x) + 0.1379;
    
    if (y > 0.008856)
        y = powf(y,0.3333);
    else
        y = (7.787 * y) + 0.1379;
    
    if (z > 0.008856)
        z = powf(z,0.3333);
    else
        z = (7.787 * z) + 0.1379;
    
    *l1=(116.0*y)-16.0;
    *a1=500.0*(x-y);
    *b1=200.0*(y-z);
}

/*
 * Calculate Delta E of La*b* values
 *
 */
double deltaE(CvScalar scolor, double L2, double A2, double B2){
    //define unity and weighting factors
    int KL = 2;
    double K1 = 0.048;
    double K2 = 0.014;
    
    double dL; //deltaL*
    double C1;
    double C2;
    double dC;
    double dH;
    double dA;
    double dB;
    double L1;
    double A1;
    double B1;
    
    double r = scolor.val[2]; //red
    double g = scolor.val[1]; //green
    double b = scolor.val[0]; //blue
    
    RGB2LAB(r, g, b, &L1, &A1, &B1);
    
//    double L1 = (L * 100)/255;
//    double A1 = A - 128;
//    double B1 = B - 128;
    
    printf("L: %f\n", L1);
    printf("a: %f\n", A1);
    printf("b: %f\n", B1);
    
    dL = L1 - L2;
    C1 = sqrtf(pow(A1, 2) + pow(B1, 2));
    C2 = sqrtf(pow(A2, 2) + pow(B2, 2));
    dC = C1 - C2;
    dA = A1 - A2;
    dB = B1 - B2;
    dH = sqrtf(pow(dA, 2) + pow(dB, 2) - pow(dC, 2));
    
    if(isnan(dH)){
        dH = 0;
    }
    
    double P1 = pow(dL/KL, 2);
    double P2 = pow(dC/(1 + K1 * C1), 2);
    double P3 = pow(dH/(1 + K2 * C2), 2);
    
    double rv = sqrtf(P1 + P2 + P3);
    
    printf("deltaE: %f\n\n", rv);
    return rv;
}

/*
 * Finds the three most recurring colors in all pictures analyzed
 * @param pColor    Array containing all colors found
 * @returns         String containing names of most recurrent colors
 */
char* findClosest(pixelColor *pColor, int pSize){
    pixelColor first;
    pixelColor second;
    pixelColor third; // ranking of most recurrent colors
    int i; // iteration
    char* rv; //return value
    
    rv = malloc(sizeof(char*) * 2048);
    
    if(pSize < 3){
        printf("TODO: Not enough colors");
        //snprintf(rv, sizeof rv, "%s%s%s%s", str1, str2, str3, str4);
        return NULL;
    }
    
    //set initial largest values
    if(pColor[0].numOccur > pColor[1].numOccur){
        if (pColor[0].numOccur > pColor[2].numOccur) {
            if (pColor[2].numOccur > pColor[1].numOccur) {
                first = pColor[0];
                second = pColor[2];
                third = pColor[1];
            }
            else{
                first = pColor[0];
                second = pColor[1];
                third = pColor[2];
            }
        }
        else if(pColor[0].numOccur < pColor[2].numOccur){
            first = pColor[2];
            second = pColor[0];
            third = pColor[1];
        }
    }
    else if (pColor[1].numOccur > pColor[0].numOccur){
        if (pColor[1].numOccur > pColor[2].numOccur) {
            if (pColor[2].numOccur > pColor[0].numOccur) {
                first = pColor[1];
                second = pColor[2];
                third = pColor[0];
            }
            else{
                first = pColor[1];
                second = pColor[0];
                third = pColor[2];
            }
        }
        else if(pColor[1].numOccur < pColor[2].numOccur){
            first = pColor[2];
            second = pColor[1];
            third = pColor[0];
        }
    }
    
    for(i = 3; i < pSize; ++i){
        int occur = pColor[i].numOccur;
        if(occur > first.numOccur){
            pixelColor tmp = first;
            pixelColor tmp2 = second;
            
            //replace first, second and third
            first = pColor[i];
            second.name = strdup(tmp.name); second.numOccur = tmp.numOccur;
            third.name = strdup(tmp2.name); third.numOccur = tmp2.numOccur;
            
        } else if(occur > second.numOccur){
            pixelColor tmp = second;
            second = pColor[i];
            
            third.name = strdup(tmp.name); third.numOccur = tmp.numOccur;
            
        } else if(occur > third.numOccur){
            third = pColor[i];
        }
    }

    //create final string to search
    strcat(rv, first.name);
    strcat(rv, " ");
    strcat(rv, second.name);
    strcat(rv, " ");
    strcat(rv, third.name);
    strcat(rv, " ");
    strcat(rv, "Interior Decor\n");
    return rv;
}

/**
 * For each image provided, computes the average color vector
 * (represented as a CvScalar object).
 *
 * @param images	The images
 * @param numImages	The length of images
 * @returns 		An numImages length array of CvScalars were rv[i] is the average color in images[i]
 */
CvScalar* getAvgColors(IplImage** images, int numImages) {
	CvScalar* rv;
	int i;
	// TODO: create return vector
	rv = malloc(sizeof(CvScalar) * numImages);

	// TODO: iterate over images and compute average color
	for(i = 0; i < numImages; ++i){
		// TODO: for each image, compute the average color (hint: use cvAvg)
		CvScalar s = cvAvg(images[i], NULL);
		rv[i] = s;
        //printf("Retrieved image average pixel");
	}
	return rv;
