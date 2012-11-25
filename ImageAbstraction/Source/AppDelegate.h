//
//  AppDelegate.h
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-24.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class WindowController;

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) WindowController *windowController;
@end
