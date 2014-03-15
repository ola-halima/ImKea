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
   char** images;
   int nDownloaded = get_images(&images, argv[1], atoi(argv[2]));

    // insert code here...

    return 0;
}

