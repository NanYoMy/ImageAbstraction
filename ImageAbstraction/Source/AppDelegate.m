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
	[self newDocument:nil];
}

#pragma mark - Responder Chain

- (void)newDocument:(id)sender
{
	// Create new window controller and set delegate.
	WindowController *windowController = [[WindowController alloc] init];
	windowController.window.delegate = self;
	
	// Add to list of window controllers and make its window visible.
	[self.windowControllers addObject:windowController];
	[windowController showWindow:nil];
}

#pragma mark - NSWindow Delegate

- (void)windowWillClose:(NSNotification *)notification
{	
	// Remove window's controller when closed.
	[self.windowControllers removeObject:[[notification object] windowController]];
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification
{
	[[[notification object] windowController] resumeActivity];
}

@end
