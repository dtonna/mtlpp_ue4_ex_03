//
//  GameViewController.m
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 14/12/2564 BE.
//

#import <QuartzCore/CAMetalLayer.h>
#import "GameViewController.h"
#import "Renderer.hpp"
#import "MetalUIView.h"

//extern mtlpp::Device g_device;

@implementation GameViewController
{
    MetalUIView *_view;
    renderer *_renderer;
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    CGRect contentSize = self.view.bounds;
    _view = [[MetalUIView alloc] initWithFrame:contentSize];
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    _view.metalLayer.device = device;
//    g_device = device;
    _view.delegate = self;
    
    _view.metalLayer.presentsWithTransaction = NO;
    _view.metalLayer.drawsAsynchronously = YES;
    _view.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    //self.metalLayer.framebufferOnly = true; // Note: setting this will dissallow sampling and reading from texture.
    _view.metalLayer.framebufferOnly = NO;
    
    [self.view addSubview:_view];

    _renderer = new renderer();
}

-(void)dealloc
{
    delete _renderer;
    [_view dealloc];
    [super dealloc];
}

-(BOOL)prefersStatusBarHidden
{
    return YES;
}

-(BOOL)prefersHomeIndicatorAutoHidden
{
    return YES;
}

- (void)drawableResize:(CGSize)size
{
    _renderer->drawable_resize(size);
}

//- (void)renderToMetalLayer:(nonnull CAMetalLayer *)layer
//{
//    id<CAMetalDrawable> drawable = layer.nextDrawable;
//    if (drawable) {
//        _renderer->update_current_drawable(drawable);
//        _renderer->update_pass_descriptor(drawable.texture);
//        _renderer->render_to_metal_layer(layer);
//    }
//}

- (void)render:(nonnull CAMetalLayer *)layer
{
    id<CAMetalDrawable> drawable = layer.nextDrawable;
    if (drawable) {
        _renderer->update_current_drawable(drawable);
        _renderer->update_pass_descriptor(drawable.texture);
//        _renderer->update_game_state(delta);
        _renderer->render();
    }
}
- (void)updateTime:(double)delta
{
    _renderer->update_game_state(delta);
}

@end
