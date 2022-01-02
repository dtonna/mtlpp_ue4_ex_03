//
//  BaseView.h
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 15/12/2564 BE.
//

#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <UIKit/UIKit.h>

#import "AppConfig.h"

NS_ASSUME_NONNULL_BEGIN

@protocol BaseViewDelegate <NSObject>

- (void)drawableResize:(CGSize)size;
- (void)render:(nonnull CAMetalLayer *)metalLayer;
- (void)updateTime:(double)delta;
//- (void)renderToMetalLayer:(nonnull CAMetalLayer *)metalLayer;

@end

@interface BaseView : UIView <CALayerDelegate>

@property (nonatomic, nonnull, readonly) CAMetalLayer *metalLayer;

@property (nonatomic, getter=isPaused) BOOL paused;

@property (nonatomic, nullable, retain) id<BaseViewDelegate> delegate;

@property (atomic) double previousTargetTimestamp;

- (void)initCommon;

#if AUTOMATICALLY_RESIZE
- (void)resizeDrawable:(CGFloat)scaleFactor;
#endif

#if ANIMATION_RENDERING
- (void)stopRenderLoop;
#endif

- (void)render:(CADisplayLink*)link;

@end

NS_ASSUME_NONNULL_END
