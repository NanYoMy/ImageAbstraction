//
//  Image.h
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-27.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#ifndef __ImageAbstraction__Image__
#define __ImageAbstraction__Image__

#import <CoreGraphics/CoreGraphics.h>

class Image {
	CGImageRef _image;
	
public:
	Image(CGImageRef);
	~Image();
	CGImageRef abstraction();
};

#endif /* defined(__ImageAbstraction__Image__) */
