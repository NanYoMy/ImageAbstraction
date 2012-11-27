//
//  WindowController.h
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-25.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface WindowController : NSWindowController

@property (weak) IBOutlet NSImageView *imageView;

- (void)openDocument:(id)sender;

@end
