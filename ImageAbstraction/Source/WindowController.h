//
//  WindowController.h
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-25.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ImageView.h"

@interface WindowController : NSWindowController <ImageViewDelegate, NSWindowDelegate>

@property BOOL isAbstract;
@property BOOL isAbstractable;

- (void)resumeActivity;
- (void)openDocument:(id)sender;

@end
