//
//  img-color-map.h
//  HHP Interior Decor & Design
//
//  Created by Heidi Muth, Persis Finny, Halima Olapade on 3/3/14.
//  Copyright (c) 2014 ImKea. All rights reserved.
//

#ifndef HHP_Interior_Decor___Design_img_color_map_h
#define HHP_Interior_Decor___Design_img_color_map_h

/**
 * This function takes RGB value and maps it to a color
 * returns 0 if sucessful, -1 otherwise
 */
int mapToColor(int red, int green, int blue);

/**
 * This function gets the 3 most reccurent colors in analyzed images
 * returns a string appended with colors appended to search criteria
 * e.g. "green yellow white interior decor"
 */
char* dominantColors();


#endif
