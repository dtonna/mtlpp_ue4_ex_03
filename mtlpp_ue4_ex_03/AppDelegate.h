//
//  AppDelegate.h
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 14/12/2564 BE.
//

#import <UIKit/UIKit.h>
#import "GameViewController.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (nonatomic, retain) GameViewController* viewController;

@end

