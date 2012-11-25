//
//  AppDelegate.m
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-25.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import "AppDelegate.h"
#import "WindowController.h"

@interface AppDelegate ()
@property (strong, nonatomic) NSMutableArray *windowControllers;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Intialize window controller array.
	self.windowControllers = [[NSMutableArray alloc] init];
	
	// Create a new window.
	[self newDocument:self];
}

- (void)newDocument:(id)sender
{
	// Create new window controller.
	WindowController *windowController = [[WindowController alloc] init];

	// Add to list of window controllers and make its window visible.
	[self.windowControllers addObject:windowController];
	[windowController.window makeKeyAndOrderFront:self];
}

@end
