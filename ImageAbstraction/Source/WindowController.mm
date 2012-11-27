//
//  WindowController.m
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-25.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import "WindowController.h"
#import "Image.h"

@implementation WindowController

- (id)init
{
	if (self = [super initWithWindowNibName:@"WindowController"]) {
		self.isOriginal = YES;
	}
	
	return self;
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
			self.isOriginal = YES;
			self.imageView.image = image;
			self.original = image;
		}
	}
}

- (IBAction)abstractImage:(id)sender
{
	if (self.imageView.image) {
		CGImageRef imageRef = [self.imageView.image CGImageForProposedRect:nil context:nil hints:nil];
		
		// Perform image abstraction.
		Image image(imageRef);
		CGImageRef newImageRef = image.abstraction();
	
		// Diplay new image.
		self.imageView.image = [[NSImage alloc] initWithCGImage:newImageRef size:NSZeroSize];
		self.isOriginal = NO;
	}
}

- (IBAction)revertImage:(id)sender
{
	self.isOriginal = YES;
	self.imageView.image = self.original;
}

@end
