//
//  google-img.h
//  HHP Interior Decor & Design
//
//  Created by Heidi Muth, Persis Finny, Halima Olapade on 3/3/14.
//  Copyright (c) 2014 ImKea. All rights reserved.
//

#ifndef HHP_Interior_Decor___Design_Google_img_h
#define HHP_Interior_Decor___Design_Google_img_h

/**
 * Makes a Google Custom Image Search query.
 * @param images		Array of full paths to images found
 * @param query			What to search for
 * @param nResults		Number of results to return
 * @return			Number of images successfully returned (size of images)
 */
int get_images(char*** images, char* query, int nResults);

/**
 * Formats a query for interior design based on provided colors and opens that webpage in the user's default browser.
 * @param colorStrings                  Strings of colors to include in query
 * @param nColors                       Length of colorStrings
*/
void find_designs(char** colorStrings, int nColors);

#endif
