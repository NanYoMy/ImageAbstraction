//
//  AppDelegate.m
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-24.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import "AppDelegate.h"
#import "WindowController.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	self.windowController = [[WindowController alloc] init];
	[self.windowController.window makeKeyAndOrderFront:self];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

@end
