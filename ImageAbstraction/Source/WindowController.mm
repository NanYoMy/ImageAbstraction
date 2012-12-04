//
//  WindowController.m
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-25.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import "WindowController.h"
#import "Image.h"

@interface WindowController ()

@property NSInteger imageID;
@property (strong) NSImage *original;
@property (weak) IBOutlet ImageView *imageView;
@property (weak) IBOutlet NSSlider *stylizationSlider;
@property (weak) IBOutlet NSSlider *quantizationSlider;

@end

@implementation WindowController

- (id)init
{
	return [super initWithWindowNibName:@"WindowController"];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Set self as image view's delegate to capture the resulting image of a drag operation.
	self.imageView.delegate = self;
}

#pragma mark - Responder Chain

- (void)openDocument:(id)sender
{
	// Create a panel to open an image.
	NSOpenPanel *openPanel = [[NSOpenPanel alloc] init];
	openPanel.allowedFileTypes = [NSArray arrayWithObjects:@"png", @"tif", @"tiff", @"jpg", @"jpeg", nil];
		
	if ([openPanel runModal]) {
		// Open selected image.
		NSImage *image = [[NSImage alloc] initByReferencingURL:[openPanel URL]];

		// If successful, display image and create a backup.
		if ([image isValid]) {
			self.imageView.image = image;			
			[self didOpenNewImage];
		}
	}
}

#pragma mark - ImageViewDelegate

- (void)didOpenNewImage
{
	self.original = self.imageView.image;
	self.isAbstract = NO;
	self.isAbstractable = YES;
}

#pragma mark - Target-Action

- (IBAction)abstractImage:(id)sender
{
	if (self.imageView.image) {
		// The image to be abstracted is the current image displayed.
		self.imageID = [self.imageView.image hash];
		self.isAbstractable = NO;
		
		// Acquire underlying image reference and create a queue for asynchronous abstraction.
		CGImageRef imageRef = [self.imageView.image CGImageForProposedRect:nil context:nil hints:nil];
		dispatch_queue_t abstractionQueue = dispatch_queue_create("ca.uvic.leons.abstraction", nil);
		
		dispatch_async(abstractionQueue, ^{
			// Create image abstraction.
			Image image(imageRef);
			
			// As the number of bins approaches inifity, quantization has no effect.
			// Pass 0 to disable quantization for max quantization value.
			uint quantization = self.quantizationSlider.intValue == self.quantizationSlider.maxValue ? 0 : self.quantizationSlider.intValue;
			CGImageRef newImageRef = image.createAbstraction(self.stylizationSlider.floatValue, quantization);
			
			// UI related code must be on the main thread.
			dispatch_async(dispatch_get_main_queue(), ^{
				// Only display abstraction if it corresponds to the current image.
				if (self.imageID == [self.imageView.image hash]) {
					self.imageView.image = [[NSImage alloc] initWithCGImage:newImageRef size:NSZeroSize];
					self.isAbstract = YES;
					self.isAbstractable = YES;
				}
				
				// Free created image.
				CGImageRelease(newImageRef);
			});
		});
	}
}

- (IBAction)revertImage:(id)sender
{
	self.imageView.image = self.original;
	self.isAbstract = NO;
	self.isAbstractable = YES;
}

@end
