//
//  ImageView.h
//  ImageAbstraction
//
//  Created by Leon Senft on 2012-11-28.
//  Copyright (c) 2012 Leon Senft. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol ImageViewDelegate <NSObject>

- (void)imageFromDragOperation:(NSImage *)image;

@end

@interface ImageView : NSImageView

@property (weak) id<ImageViewDelegate> delegate;

@end
