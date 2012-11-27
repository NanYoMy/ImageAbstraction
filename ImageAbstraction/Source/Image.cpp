//
//  Image.cpp
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-27.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#include "Image.h"

Image::Image(CGImageRef image)
{
	_image = CGImageCreateCopy(image);
}

Image::~Image()
{
	CGImageRelease(_image);
}

CGImageRef Image::abstraction()
{
	// Get mutable image data.
	CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(_image));
	CFMutableDataRef mutableData = CFDataCreateMutableCopy(NULL, CFDataGetLength(data), data);
	UInt8 *bytes = CFDataGetMutableBytePtr(mutableData);

	// Do something here...
	
	// Reconstruct image from edited data.
	CGContextRef context = CGBitmapContextCreate(bytes,
												 CGImageGetWidth(_image),
												 CGImageGetHeight(_image),
												 CGImageGetBitsPerComponent(_image),
												 CGImageGetBytesPerRow(_image),
												 CGImageGetColorSpace(_image),
												 CGImageGetBitmapInfo(_image));
	
	// New image.
	CGImageRef image = CGBitmapContextCreateImage(context);

	// Free used memory.
	CGContextRelease(context);
	CFRelease(data);
	CFRelease(mutableData);
	
	return image;
}