/*
 *  img-color-map.h
 *  HHP Interior Decor & Design
 *
 *  Created by Heidi Muth, Persis Finny, Halima Olapade
 *  Copyright (c) 2014 ImKea. All rights reserved.
 */

#ifndef _img_color_map_h
#define _img_color_map_h

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
 * Finds the CvScalar in colors closest to t using the colorDistance function.
 * @param scolors	The colors to look through
 * @param pColor	Array to store new colors found
 * @param numColors	The length of scolors
 * @returns		0 on success, -1 otherwise
 */
int assignColor(CvScalar* scolors, pixelColor** pColor, int* pSize, int numColors);

/*
 * Given a cvScalar, maps its RGB values to a general color name
 * @param scolor	The cvScalar representing subimage average color
 * @returns		The char name of the color that best matches RGB
 */
char* mapColorName(CvScalar scolor);

/*
 * Convert RGB values to L*ab
 * @param r,g,b     Red, Green, Blue values to convert to L*ab
 * @param l1,a1,b1  L*ab calculated from rgb
 * @reference   http://www.ece.ualberta.ca/~elliott/ece492/appnotes/2012w/UAV_Imaging/deltaE.c
 */
void RGB2LAB(double r, double g, double b, double* l1, double* a1, double* b1);

/*
 * Calculate Delta E of La*b* values
 * @param scolor    The cvScalar representing subimage average color
 * @param L2        L value read from lab_db.txt
 * @param A2        A value read from lab_db.txt
 * @param B2        B value read from lab_db.txt
 * @returns         double delta E distance between l*ab value of scolor and given params
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

#endif
