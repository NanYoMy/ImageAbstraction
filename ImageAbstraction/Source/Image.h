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
#import <iostream>

typedef union pixel3f {
	struct { float L, a, b; };
	struct { float X, Y, Z; };
} pixel3f;

typedef struct pixel4i {
	UInt8 r, g, b, a;
} pixel4i;

class Image {
public:
	Image(CGImageRef);
	~Image();
	CGImageRef createAbstraction();

private:
	CGImageRef _image;
	pixel3f *_pixels;
	size_t _width;
	size_t _height;
	size_t _size;
	
	pixel3f *pixelAt(pixel3f *, int, int);
	pixel3f *createEdges(pixel3f *);
	pixel3f *gaussian(pixel3f *, float);
	void RGBtoLab(pixel4i *, pixel3f *);
	void LabtoRGB(pixel3f *, pixel4i *);
	void RGBtoXYZ(pixel4i *, pixel3f *);
	void XYZtoRGB(pixel3f *, pixel4i *);
	void LabtoXYZ(pixel3f *);
	void XYZtoLab(pixel3f *);
};

#endif /* defined(__ImageAbstraction__Image__) */
