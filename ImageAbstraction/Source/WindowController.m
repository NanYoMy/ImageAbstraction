//
//  WindowController.m
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-25.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import "WindowController.h"

@interface WindowController ()

@end

@implementation WindowController

- (id)init
{
	if (self = [super initWithWindowNibName:@"WindowController"]) {
		// Enable full screen.
		[self.window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
	}
	
	return self;
}

- (void)openDocument:(id)sender
{
	NSLog(@"Load new image...");
}

@end
