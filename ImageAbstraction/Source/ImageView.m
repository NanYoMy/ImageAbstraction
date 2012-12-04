//
//  ImageView.m
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-28.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import "ImageView.h"

@implementation ImageView

- (void)concludeDragOperation:(id<NSDraggingInfo>)sender
{
	// Notify delegate that a new image has been set by a drag operation.
	[super concludeDragOperation:sender];
	[self.delegate didOpenNewImage];
}

@end
