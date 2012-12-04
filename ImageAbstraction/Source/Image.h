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

typedef union pixel3f {
	struct { float L, a, b; };
	struct { float X, Y, Z; };
} pixel3f;

typedef struct pixel4b {
	UInt8 r, g, b, a;
} pixel4b;

class Image {
public:
	Image(CGImageRef);
	~Image();
	CGImageRef createAbstraction(float, uint);

private:
	CFMutableDataRef _data;
	size_t _width;
	size_t _height;
	size_t _bitsPerComponent;
	size_t _bytesPerRow;
	CGColorSpaceRef _colorSpaceRef;
	CGBitmapInfo _bitmapInfo;
	pixel3f *_pixels;
	pixel3f *_copy;
	
	void bilateral();
	void quantize(uint);
	void overlayEdges(pixel3f *);
	pixel3f *createEdges(float);
	pixel3f *createGaussian(float);
	float *createGaussianKernel(uint, float);
	
	void RGBtoLab(pixel4b *, pixel3f *);
	void LabtoRGB(pixel3f *, pixel4b *);
	void RGBtoXYZ(pixel4b *, pixel3f *);
	void XYZtoRGB(pixel3f *, pixel4b *);
	void LabtoXYZ(pixel3f *);
	void XYZtoLab(pixel3f *);
	
	inline pixel3f *pixelAt(pixel3f *pixels, int x, int y)
	{
		if (x < 0) x = 0;
		else if (x >= _width) x = (int) _width - 1;
		
		if (y < 0) y = 0;
		else if (y >= _height) y = (int) _height - 1;
		
		return pixels + x + y * _width;
	}
};

#endif /* defined(__ImageAbstraction__Image__) */
