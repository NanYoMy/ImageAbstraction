//
//  Image.cpp
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-27.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#include "Image.h"
#include <set>

// Constant coefficients used for bilateral filtering.
#define RADIUS 6
#define VARIANCE 9.0f
#define PHOTOMETRIC -36.125

// Constant coefficient for difference of gaussian edges.
#define SIGMA 2.0f
#define TAU 0.98f

Image::Image(CGImageRef image) :
	_width(CGImageGetWidth(image)),
	_height(CGImageGetHeight(image)),
	_bitsPerComponent(CGImageGetBitsPerComponent(image)),
	_bytesPerRow(CGImageGetBytesPerRow(image)),
	_colorSpaceRef(CGImageGetColorSpace(image)),
	_bitmapInfo(CGImageGetBitmapInfo(image)),
	_pixels(new pixel3f[_width * _height]),
	_copy(new pixel3f[_width * _height])
{
	// Obtain mutable image data.
	CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(image));
	_data = CFDataCreateMutableCopy(NULL, CFDataGetLength(data), data);
	
	// Release data from image.
	CFRelease(data);
}

Image::~Image()
{
	CFRelease(_data);

	delete[] _pixels;
	delete[] _copy;
}

CGImageRef Image::createAbstraction(float stylization, uint quantization)
{
	pixel4b *rgbPixels = (pixel4b *) CFDataGetMutableBytePtr(_data);
	
	// Convert from RGB to Lab colorspace to perform operations on lightness channel.
	RGBtoLab(rgbPixels, _pixels);
	
	// Initial bilateral filter.
	bilateral();
	
	// Extract edges.
	pixel3f *edges = createEdges(stylization);
	
	// Additional bilateral filtering.
	bilateral();
	bilateral();
	
	// Quantize lightness channel.
	quantize(quantization);
	 
	// Overlay edges.
	overlayEdges(edges);
	 
	// Convert back to RGB colorspace.
	LabtoRGB(_pixels, rgbPixels);
	
	// Create an image from the modified data.
	CGContextRef context = CGBitmapContextCreate(
		rgbPixels,
		_width,
		_height,
		_bitsPerComponent,
		_bytesPerRow,
		_colorSpaceRef,
		_bitmapInfo
	);
	
	CGImageRef image = CGBitmapContextCreateImage(context);
	
	delete[] edges;
	
	return image;
}

void Image::bilateral()
{
	// Gaussian kernel used to model geometric similarity.
	float *kernel = createGaussianKernel(RADIUS + 1, VARIANCE);
	
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			pixel3f *center = pixelAt(_pixels, x, y);
			float numerator = 0;
			float denominator = 0;
			
			for (int i = -RADIUS; i <= RADIUS; i++) {
				pixel3f *neighbor = pixelAt(_pixels, x + i, y);
				
				// Compute euclidean distance between Lab colors to determine photometric similarity.
				float dL = center->L - neighbor->L;
				float da = center->a - center->a;
				float db = center->b - center->b;
				float distance = dL * dL + da * da + db * db;
				
				float photometric = expf(distance / PHOTOMETRIC);
				float similarity = photometric * kernel[abs(i)];
				
				// Denominator serves to normalize values.
				denominator += similarity;
				numerator += similarity * neighbor->L;
			}
			
			// Copy a and b data over as well to use for the y-filter pass.
			pixel3f *target = pixelAt(_copy, x, y);
			target->L = numerator / denominator;
			target->a = center->a;
			target->b = center->b;
		}
	}
	
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			pixel3f *center = pixelAt(_copy, x, y);
			float numerator = 0;
			float denominator = 0;
			
			for (int j = -RADIUS; j <= RADIUS; j++) {
				pixel3f *neighbor = pixelAt(_copy, x, y + j);
				
				float dL = center->L - neighbor->L;
				float da = center->a - center->a;
				float db = center->b - center->b;
				float distance = dL * dL + da * da + db * db;
				
				float photometric = expf(distance / PHOTOMETRIC);
				float similarity = photometric * kernel[abs(j)];
				
				denominator += similarity;
				numerator += similarity * neighbor->L;
			}
			
			pixelAt(_pixels, x, y)->L = numerator / denominator;
		}
	}
	
	delete[] kernel;
}

void Image::quantize(uint n)
{
	if (!n) {
		return;
	}
	
	std::set<float> bins;
	float binWidth = 100.0f / n;

	// Create bin values.
	for (size_t i = 0; i <= n; i++) {
		bins.insert(binWidth * i);
	}
	
	for (pixel3f *p = _pixels; p < _pixels + _width * _height; p++) {
		std::set<float>::iterator i = bins.upper_bound(p->L);
		float nearest;
		
		// Determine the nearest bin value.
		if (i == bins.end()) {
			nearest = *(i--);
		} else {
			float ceilling = *i--;
			float floor = *i;
			nearest = p->L - floor < ceilling - p->L ? floor : ceilling;
		}
		
		// Smooth quantization.
		p->L = nearest + (binWidth / 2.0f) * tanhf(p->L - nearest);
	}
}

void Image::overlayEdges(pixel3f *edges)
{
	// Overlay edges over image by multiplying their values together.
	for (pixel3f *p = _pixels; p < _pixels + _width * _height; p++, edges++) {
		p->L *= (edges->L / 100.0f);
		p->a *= (edges->L / 100.0f);
		p->b *= (edges->L / 100.0f);
	}
}

pixel3f *Image::createEdges(float stylization)
{
	pixel3f *gaussianE = createGaussian(SIGMA);
	pixel3f *gaussianR = createGaussian(sqrtf(1.6) * SIGMA);
	
	for (int i = 0; i < _width * _height; i++) {
		// Difference of gaussians mapped to a smoothed step function.
		float dog = gaussianE[i].L - TAU * gaussianR[i].L;
		gaussianE[i].L = dog > 0.0f ? 100.0f : 100.0f * (1.0f + tanhf(stylization * dog));
	}

	delete[] gaussianR;
	
	return gaussianE;
}

#pragma mark - Filters

pixel3f *Image::createGaussian(float sigma)
{
	pixel3f *gaussian = new pixel3f[_width * _height]();
	
	// Precompute constants.
	float variance = sigma * sigma;
	float denomiator = 2 * M_PI * variance;

	// Radius of filter.
	int r = 2.0f * sigma;
	float *kernel = createGaussianKernel(r + 1, variance);
	
	// Seperable x component of Gaussian filter.
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			float sum = 0;
			
			for (int i = -r; i <= r; i++) {
				sum += pixelAt(_pixels, x + i, y)->L * kernel[abs(i)];
			}
			
			// Do not divide by the regular denominator.
			pixelAt(_copy, x, y)->L = sum;
		}
	}
	
	// Seperable y component of Gaussian filter.
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			float sum = 0;
			
			for (int j = -r; j <= r; j++) {
				sum += pixelAt(_copy, x, y + j)->L * kernel[abs(j)];
			}
			
			// Divided by the denominator squared only once rather than twice.
			pixelAt(gaussian, x, y)->L = sum / denomiator;
		}
	}
	
	// Free temporary data.
	delete[] kernel;
	
	return gaussian;
}

float* Image::createGaussianKernel(uint size, float variance)
{
	float *kernel = new float[size];
	variance *= -2.0f;
	
	for (uint i = 0; i < size; i++) {
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
	
	for (s = source, d = destination; s != source + _width * _height; s++, d++) {
		float r = s->r / 255.0f;
		float g = s->g / 255.0f;
		float b = s->b / 255.0f;
		
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
	
	for (s = source, d = destination; s != source + _width * _height; s++, d++) {
		float r =  0.032406f * s->X - 0.015372f * s->Y - 0.004986f * s->Z;
		float g = -0.009689f * s->X + 0.018758f * s->Y + 0.0004150f * s->Z;
		float b =  0.000557f * s->X - 0.002040f * s->Y + 0.01057f * s->Z;
		
		r = r > 0.0031308f ? 1.055f * powf(r, 0.4167f) - 0.055f : 12.92f * r;
		g = g > 0.0031308f ? 1.055f * powf(g, 0.4167f) - 0.055f : 12.92f * g;
		b = b > 0.0031308f ? 1.055f * powf(b, 0.4167f) - 0.055f : 12.92f * b;
		
		// Validate RGB values before scaling.
		d->r = 255 * (r < 0.0f ? 0.0f : r > 1.0f ? 1.0f : r);
		d->g = 255 * (g < 0.0f ? 0.0f : g > 1.0f ? 1.0f : g);
		d->b = 255 * (b < 0.0f ? 0.0f : b > 1.0f ? 1.0f : b);
	}
}

void Image::LabtoXYZ(pixel3f *pixels)
{	
	for (pixel3f *p = pixels; p != pixels + _width * _height; p++) {
		float Y = (p->L + 16.0f) / 116.0f;
		float X = p->a / 500.0f + Y;
		float Z = Y - p->b / 200.0f;
		
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
	for (pixel3f *p = pixels; p != pixels + _width * _height; p++) {
		float X = p->X /  95.047f;
		float Y = p->Y / 100.000f;
		float Z = p->Z / 108.883f;
		
		X = X > 0.008856f ? powf(X, 0.3333f) : 7.787f * X + 0.13793103f;
		Y = Y > 0.008856f ? powf(Y, 0.3333f) : 7.787f * Y + 0.13793103f;
		Z = Z > 0.008856f ? powf(Z, 0.3333f) : 7.787f * Z + 0.13793103f;
		
		p->L = 116.0f * Y - 16.0f;
		p->a = 500.0f * (X - Y);
		p->b = 200.0f * (Y - Z);
	}
}
