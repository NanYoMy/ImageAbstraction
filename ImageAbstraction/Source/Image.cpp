//
//  Image.cpp
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-27.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#include "Image.h"

Image::Image(CGImageRef image) :
	_image(CGImageCreateCopy(image)),
	_width(CGImageGetWidth(image)),
	_height(CGImageGetHeight(image)),
	_size(_width * _height)
{
}

Image::~Image()
{
	CGImageRelease(_image);
}

CGImageRef Image::createAbstraction()
{	
	// Get mutable image data and create floating point array for colorspace conversions.
	CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(_image));
	CFMutableDataRef mutableData = CFDataCreateMutableCopy(NULL, CFDataGetLength(data), data);
	pixel4b *rgbPixels = (pixel4b *) CFDataGetMutableBytePtr(mutableData);
	pixel3f *labPixels = new pixel3f[_size];
	
	// Convert from RGB to Lab colorspace to perform operations on lightness channel.
	RGBtoLab(rgbPixels, labPixels);
	
	// Nom nom nom bytes!
	// Play with lightness channel...
	pixel3f *edges = createEdges(labPixels);

	// Convert back to RGB colorspace.
	LabtoRGB(edges, rgbPixels);
	
	// Create a new image from abstracted data.
	CGContextRef context = CGBitmapContextCreate(
		rgbPixels,
		CGImageGetWidth(_image),
		CGImageGetHeight(_image),
		CGImageGetBitsPerComponent(_image),
		CGImageGetBytesPerRow(_image),
		CGImageGetColorSpace(_image),
		CGImageGetBitmapInfo(_image)
	);
	
	CGImageRef image = CGBitmapContextCreateImage(context);
	
	// Release memory.
	CGContextRelease(context);
	CFRelease(mutableData);
	CFRelease(data);
	
	delete[] edges;
	delete[] labPixels;
	
	return image;
}

pixel3f *Image::pixelAt(pixel3f *pixels, int x, int y)
{
	return pixels + x + (y * _width);
}

pixel3f *Image::createEdges(pixel3f *source)
{
	float sigma = 2.0f;
	float tau = 0.98f;
	float phi = 2.0f;
	float dog;
	
	pixel3f *gaussianE = createGaussian(source, sigma);
	pixel3f *gaussianR = createGaussian(source, sqrtf(1.6) * sigma);
	
	for (int i = 0; i < _size; i++) {
		// Difference of gaussians mapped to a smoothed step function.
		dog = gaussianE[i].L - tau * gaussianR[i].L;
		gaussianE[i].L = dog > 0.0f ? 100.0f : 100.0f * (1.0f + tanhf(phi * dog));
	}

	delete[] gaussianR;
	
	return gaussianE;
}

#pragma mark - Filters

pixel3f *Image::createGaussian(pixel3f *source, float sigma)
{
	pixel3f *gaussian = new pixel3f[_size]();
	pixel3f *temp = new pixel3f[_size]();
	
	// Precompute constants.
	float variance = sigma * sigma;
	float denomiator = 2 * M_PI * variance;

	// Radius of filter.
	int r = 3.0f * sigma;
	float *kernel = createGaussianKernel(r + 1, variance);
	
	// Sum of filtered neighbouring pixels' lightness.
	float sum;
	
	// Seperable x component of Gaussian filter.
	for (int x = r; x < _width - r; x++) {
		for (int y = r; y < _height - r; y++) {
			sum = 0;
			
			for (int i = -r; i <= r; i++) {
				sum += pixelAt(source, x + i, y)->L * kernel[abs(i)];
			}
			
			// Do not divide by the regular denominator.
			pixelAt(temp, x, y)->L = sum;
		}
	}
	
	// Seperable y component of Gaussian filter.
	for (int x = r; x < _width - r; x++) {
		for (int y = r; y < _height - r; y++) {
			sum = 0;
			
			for (int j = -r; j <= r; j++) {
				sum += pixelAt(temp, x, y + j)->L * kernel[abs(j)];
			}
			
			// Divided by the denominator square only once rather than twice as is.
			pixelAt(gaussian, x, y)->L = sum / denomiator;
		}
	}
	
	// Free temporary data.
	delete[] kernel;
	delete[] temp;
	
	return gaussian;
}

float* Image::createGaussianKernel(int size, float variance)
{
	float *kernel = new float[size];
	variance *= -2.0f;
	
	for (int i = 0; i < size; i++) {
		kernel[i] = expf(i * i / variance);
	}
	
	return kernel;
}

#pragma mark - Colorspace Conversions

void Image::RGBtoLab(pixel4b *source, pixel3f *destination)
{
	RGBtoXYZ(source, destination);
	XYZtoLab(destination);
}

void Image::LabtoRGB(pixel3f *source, pixel4b *destination)
{
	LabtoXYZ(source);
	XYZtoRGB(source, destination);
}

void Image::RGBtoXYZ(pixel4b *source, pixel3f *destination)
{
	pixel4b *s;
	pixel3f *d;
	float r, g, b;
	
	for (s = source, d = destination; s != source + _size; s++, d++) {
		r = s->r / 255.0f;
		g = s->g / 255.0f;
		b = s->b / 255.0f;
		
		r = r > 0.04045f ? powf((r + 0.055f) / 1.055f, 2.4f) : r / 12.92f;
		g = g > 0.04045f ? powf((g + 0.055f) / 1.055f, 2.4f) : g / 12.92f;
		b = b > 0.04045f ? powf((b + 0.055f) / 1.055f, 2.4f) : b / 12.92f;
		
		d->X = 41.24f * r + 35.76f * g + 18.05f * b;
		d->Y = 21.26f * r + 71.52f * g + 7.220f * b;
		d->Z = 1.930f * r + 11.92f * g + 95.05f * b;
	}
}

void Image::XYZtoRGB(pixel3f *source, pixel4b *destination)
{
	pixel3f *s;
	pixel4b *d;
	float r, g, b;
	
	for (s = source, d = destination; s != source + _size; s++, d++) {
		r =  0.032406f * s->X - 0.015372f * s->Y - 0.004986f * s->Z;
		g = -0.009689f * s->X + 0.018758f * s->Y + 0.0004150f * s->Z;
		b =  0.000557f * s->X - 0.002040f * s->Y + 0.01057f * s->Z;
		
		r = r > 0.0031308f ? 1.055f * powf(r, 0.4167f) - 0.055f : 12.92f * r;
		g = g > 0.0031308f ? 1.055f * powf(g, 0.4167f) - 0.055f : 12.92f * g;
		b = b > 0.0031308f ? 1.055f * powf(b, 0.4167f) - 0.055f : 12.92f * b;
		
		d->r = 255 * r;
		d->g = 255 * g;
		d->b = 255 * b;
	}
}

void Image::LabtoXYZ(pixel3f *pixels)
{
	float X, Y, Z;
	
	for (pixel3f *p = pixels; p != pixels + _size; p++) {
		Y = (p->L + 16.0f) / 116.0f;
		X = p->a / 500.0f + Y;
		Z = Y - p->b / 200.0f;
		
		X = X > 0.2069f ? powf(X, 3.0f) : (X - 0.13793103f) / 7.787f;
		Y = Y > 0.2069f ? powf(Y, 3.0f) : (Y - 0.13793103f) / 7.787f;
		Z = Z > 0.2069f ? powf(Z, 3.0f) : (Z - 0.13793103f) / 7.787f;
		
		p->X =  95.047f * X;
		p->Y = 100.000f * Y;
		p->Z = 108.883f * Z;
	}
}

void Image::XYZtoLab(pixel3f *pixels)
{
	float X, Y, Z;
	
	for (pixel3f *p = pixels; p != pixels + _size; p++) {
		X = p->X /  95.047f;
		Y = p->Y / 100.000f;
		Z = p->Z / 108.883f;
		
		X = X > 0.008856f ? powf(X, 0.3333f) : 7.787f * X + 0.13793103f;
		Y = Y > 0.008856f ? powf(Y, 0.3333f) : 7.787f * Y + 0.13793103f;
		Z = Z > 0.008856f ? powf(Z, 0.3333f) : 7.787f * Z + 0.13793103f;
		
		p->L = 116.0f * Y - 16.0f;
		p->a = 500.0f * (X - Y);
		p->b = 200.0f * (Y - Z);
	}
}
