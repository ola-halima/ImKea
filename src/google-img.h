//
//  google-img.h
//  HHP Interior Decor & Design
//
//  Created by Heidi Muth, Persis Finny, Halima Olapade on 3/3/14.
//  Copyright (c) 2014 ImKea. All rights reserved.
//

#ifndef HHP_Interior_Decor___Design_Google_img_h
#define HHP_Interior_Decor___Design_Google_img_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <opencv/cv.h>
#include <curl/curl.h>
#include <jansson.h>

/**
 * Makes a Google Custom Image Search query.
 * @param images		Array of full paths to images found
 * @param query			What to search for
 * @param nResults		Number of results to return
 * @return			Number of images successfully returned (size of images)
 */
int get_images(char*** images, char* query, int nResults);

/**
 * Opens an image search based on a query.
 * @param query			space delimited full query to search for
 */
void open_image_search(char* query);

#endif
