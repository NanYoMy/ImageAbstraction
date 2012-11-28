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
@property (strong) NSImage *original;
@end

@implementation WindowController

- (id)init
{
	return [super initWithWindowNibName:@"WindowController"];
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
		}
	}
}

- (IBAction)abstractImage:(id)sender
{	
	if (self.imageView.image) {
		CGImageRef imageRef = [self.imageView.image CGImageForProposedRect:nil context:nil hints:nil];
		
		// Perform image abstraction.
		Image image(imageRef);
		CGImageRef newImageRef = image.createAbstraction();
	
		// Diplay new image.
		self.imageView.image = [[NSImage alloc] initWithCGImage:newImageRef size:NSZeroSize];
		
		// Free created image.
		CGImageRelease(newImageRef);
	}
}

- (IBAction)revertImage:(id)sender
{
	self.imageView.image = self.original;
}

@end
