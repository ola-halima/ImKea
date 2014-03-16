/*
 *  img-color-map.c
 *  HHP Interior Decor & Design
 *
 *  Created by Heidi Muth, Persis Finny, Halima Olapade
 *  Copyright (c) 2014 ImKea. All rights reserved.
 */

#include "img-color-map.h"

#define COLOR_MAP_FILE "assets/lab_db.txt"

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
    FILE* f;
    int i=0;
    char tok[256];
    double min = -1;
    char* cName = NULL;

    if ((f = fopen(COLOR_MAP_FILE, "r")) == NULL) { // file descriptor to read from
        fprintf(stderr, "Could not find color map file: %s\n", COLOR_MAP_FILE);
    }
    
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
 * Convert RGB values to L*ab
 * @param r,g,b     Red, Green, Blue values to convert to L*ab
 * @param l1,a1,b1  L*ab calculated from rgb
 * @reference   http://www.ece.ualberta.ca/~elliott/ece492/appnotes/2012w/UAV_Imaging/deltaE.c
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
 * @param scolor    The cvScalar representing subimage average color
 * @param L2        L value read from lab_db.txt
 * @param A2        A value read from lab_db.txt
 * @param B2        B value read from lab_db.txt
 * @returns         double delta E distance between l*ab value of scolor and given params
 
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
    
//    printf("L: %f\n", L1);
//    printf("a: %f\n", A1);
//    printf("b: %f\n", B1);
    
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
    
//    printf("deltaE: %f\n\n", rv);
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
        printf("Images has less than 3 colors\n");
        for(i = 0; i < pSize; ++i){
            strcat(rv, pColor[i].name);
            strcat(rv, " ");
        }
        strcat(rv, "Interior Decor");
        return rv;
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
    strcat(rv, "Interior Decor");
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
    
	rv = malloc(sizeof(CvScalar) * numImages);
    
	for(i = 0; i < numImages; ++i){
		CvScalar s = cvAvg(images[i], NULL);
		rv[i] = s;
	}
	return rv;
}
