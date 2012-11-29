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

@property BOOL isRelevant;
@property (strong) NSImage *original;

@end

@implementation WindowController

- (id)init
{
	return [super initWithWindowNibName:@"WindowController"];
}

- (void)dealloc
{
	[self.imageView removeObserver:self forKeyPath:@"image"];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Set self as image view's delegate to capture the resulting image of a drag operation.
	self.imageView.delegate = self;
	
	// Register as observer of the image view's image.
	[self.imageView addObserver:self forKeyPath:@"image" options:nil context:nil];
}

- (void)openDocument:(id)sender
{
	// Create a panel to open an image.
	NSOpenPanel *openPanel = [[NSOpenPanel alloc] init];
	openPanel.allowedFileTypes = [NSArray arrayWithObjects:@"png", @"tif", @"jpg", nil];
		
	if ([openPanel runModal]) {
		// Open selected image.
		NSImage *image = [[NSImage alloc] initByReferencingURL:[openPanel URL]];

		// If successful, display image and create a backup.
		if ([image isValid]) {
			self.imageView.image = image;
			self.original = image;
		}
	}
}

#pragma mark - ImageViewDelegate

- (void)imageFromDragOperation:(NSImage *)image
{
	self.original = image;
}

#pragma mark - Key-Value Observing

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if ([keyPath isEqualToString:@"image"]) {
		self.isAbstract = NO;
		self.isRelevant = NO;
	}
}

#pragma mark - Target-Action

- (IBAction)abstractImage:(id)sender
{	
	if (self.imageView.image) {		
		// Acquire underlying image reference and create a queue for asynchronous execution.
		CGImageRef imageRef = [self.imageView.image CGImageForProposedRect:nil context:nil hints:nil];
		dispatch_queue_t abstractionQueue = dispatch_queue_create("ca.uvic.leons.abstraction", nil);

		dispatch_async(abstractionQueue, ^{
			// The image to be abstracted is the current image displayed.
			self.isRelevant = YES;
			
			// Create image abstraction.
			Image image(imageRef);
			CGImageRef newImageRef = image.createAbstraction();
			
			// UI related code must be on the main thread.
			dispatch_async(dispatch_get_main_queue(), ^{
				// Only display abstraction if it corresponds to the current image.
				if (self.isRelevant) {
					self.imageView.image = [[NSImage alloc] initWithCGImage:newImageRef size:NSZeroSize];
					self.isAbstract = YES;
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
}

@end
