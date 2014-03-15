//
//  imKea-img.h
//  HHP Interior Decor & Design
//
//  Created by Heidi Muth, Persis Finny, Halima Olapade on 3/3/14.
//  Copyright (c) 2014 ImKea. All rights reserved.
//

#ifndef HHP_Interior_Decor___Design_ImKea_Img_h
#define HHP_Interior_Decor___Design_ImKea_Img_h

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"

/**
* This is a pixel color struct
* It contains two members:
* 	@name: The generalized name of the color based on rgb values
*	@numOccurrences: The number of times the color appears over all images analyzed
*	For example, given 0,0,0:
*		@name: "black"
*		@numOccure: 1 //assuming first occurrence
**/
typedef struct pixelColor pixelColor;
struct pixelColor{
	char* name;
	int numOccur;
};

/**
* Given a name, create a new pixelColor element initialized to 1 occurence
* @param name	The name of the color being initialized
* @param rv	A pointer to memory for the pixel color element
*/
void createPixelColor(char* name, pixelColor* rv);

/**
* A structure representing a group of pixelColors that appear in pictures analyzed
*/
typedef struct colorGroup colorGroup;
struct colorGroup {
	pixelColor *data; //array of pixel colors
	int size; // number of pixel colors in data
};

/**
 * Loads an image (stored as an IplImage struct) for each
 * filename provided.
 * @param numImages		The number of images to load
 * @param filenames		A list of strings of the filenames to load
 */
IplImage** loadImages(int numImages, char ** fileNames);

/**
 * Splits up an image into numColumns by numRows sub-images and returns the results.
 *
 * @param src	The source image to split
 * @param numColumns	The number of columns to split into
 * @param numRows 		The number of rows to split into
 * @returns				A numColumns x numRows array of IplImages
 */
IplImage ** getSubImages(IplImage* src, int numColumns, int numRows);

/**
 * Finds the CvScalar in colors closest to t using the colorDistance function.
 * @param scolors	The colors to look through
 * @param pColor	Array to store new colors found
 * @param numColors	The length of scolors
 * @returns		0 on success, -1 otherwise
 */
int assignColor(CvScalar* scolors, pixelColor** pColor, int* pSize, int numColors);

/*
 * Given a cvScalar, maps its RGB values to a general color name
 * @param scolor	The cvScalar
 * @returns		The char name of the color that best matches RGB
 */
char* mapColorName(CvScalar scolor);

/*
 * Calculate Delta E of La*b* values
 *
 */
double deltaE(CvScalar scolor, double L2, double A2, double B2);

/*
 * Finds the three most recurring colors in all pictures analyzed
 * @param pColor    Array containing all colors found
 * @returns         String containing names of most recurrent colors
 */
char* findClosest(pixelColor *pColor, int pSize);

/**
 * For each image provided, computes the average color vector
 * (represented as a CvScalar object).
 *
 * @param images	The images
 * @param numImages	The length of images
 * @returns 		An numImages length array of CvScalars were rv[i] is the average color in images[i]
 */
CvScalar* getAvgColors(IplImage** images, int numImages);
