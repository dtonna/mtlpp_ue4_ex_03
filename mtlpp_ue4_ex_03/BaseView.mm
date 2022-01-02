//
//  BaseView.m
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 15/12/2564 BE.
//

#import "BaseView.h"
#include <iostream>

@implementation BaseView

///////////////////////////////////////
#pragma mark - Initialization and Setup
///////////////////////////////////////

- (instancetype) initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if(self)
    {
        [self initCommon];
    }
    return self;
}

- (instancetype) initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self)
    {
        [self initCommon];
    }
    return self;
}

- (void)initCommon
{
    _metalLayer = (CAMetalLayer*) self.layer;
    
    self.layer.delegate = self;
}

//////////////////////////////////
#pragma mark - Render Loop Control
//////////////////////////////////

#if ANIMATION_RENDERING

- (void)stopRenderLoop
{
    // Stubbed out method.  Subclasses need to implement this method.
}

- (void)dealloc
{
    [self stopRenderLoop];
    [super dealloc];
}

#else // IF !ANIMATION_RENDERING

// Override methods needed to handle event-based rendering

- (void)displayLayer:(CALayer *)layer
{
    [self renderOnEvent];
}

- (void)drawLayer:(CALayer *)layer
        inContext:(CGContextRef)ctx
{
    [self renderOnEvent];
}

- (void)drawRect:(CGRect)rect
{
    [self renderOnEvent];
}

- (void)renderOnEvent
{
#if RENDER_ON_MAIN_THREAD
    [self render];
#else
    // Dispatch rendering on a concurrent queue
    dispatch_queue_t globalQueue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_async(globalQueue, ^(){
        [self render];
    });
#endif
}

#endif // END !ANIMAITON_RENDERING
///////////////////////
#pragma mark - Resizing
///////////////////////

#if AUTOMATICALLY_RESIZE

- (void)resizeDrawable:(CGFloat)scaleFactor
{
    CGSize newSize = self.bounds.size;
    newSize.width *= scaleFactor;
    newSize.height *= scaleFactor;
    
    if(newSize.width <= 0 || newSize.width <= 0)
    {
        return;
    }
    
#if RENDER_ON_MAIN_THREAD
    
    if(newSize.width == _metalLayer.drawableSize.width &&
       newSize.height == _metalLayer.drawableSize.height)
    {
        return;
    }
    
    _metalLayer.drawableSize = newSize;
    
    [_delegate drawableResize:newSize];
    
#else
    // All AppKit and UIKit calls which notify of a resize are called on the main thread.  Use
    // a synchronized block to ensure that resize notifications on the delegate are atomic
    @synchronized(_metalLayer)
    {
        if(newSize.width == _metalLayer.drawableSize.width &&
           newSize.height == _metalLayer.drawableSize.height)
        {
            return;
        }
        
        _metalLayer.drawableSize = newSize;
        
        [_delegate drawableResize:newSize];
    }
#endif
}

#endif

//////////////////////
#pragma mark - Drawing
//////////////////////

- (void)render:(CADisplayLink*)link
{
    float delta = link.targetTimestamp - _previousTargetTimestamp;
    _previousTargetTimestamp = link.targetTimestamp;
    double max = MAX(delta, link.duration);
    double min = MIN(delta, link.duration);
    if ((max - min) > 0.00001) {
        std::cout << " duration : " << link.duration << " delta : " << delta << std::endl;
        delta = link.duration;
    }
    std::cout << "render : " << link.targetTimestamp << " timestamp : " << link.timestamp << " prev : " << _previousTargetTimestamp << " delta : " << delta << " duration : " << link.duration << std::endl;
  
#if RENDER_ON_MAIN_THREAD
    [_delegate updateTime:delta];
    [_delegate render:_metalLayer];
//    [_delegate renderToMetalLayer:_metalLayer];
#else

    // Must synchronize if rendering on background thread to ensure resize operations from the
    // main thread are complete before rendering which depends on the size occurs.
    @synchronized(_metalLayer)
    {
        [_delegate updateTime:delta];
        [_delegate render:_metalLayer;
//        [_delegate renderToMetalLayer:_metalLayer];
    }
#endif
}


@end
