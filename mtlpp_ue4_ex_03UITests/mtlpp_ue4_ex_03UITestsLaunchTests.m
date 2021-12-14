//
//  mtlpp_ue4_ex_03UITestsLaunchTests.m
//  mtlpp_ue4_ex_03UITests
//
//  Created by Noppadol Anuroje on 14/12/2564 BE.
//

#import <XCTest/XCTest.h>

@interface mtlpp_ue4_ex_03UITestsLaunchTests : XCTestCase

@end

@implementation mtlpp_ue4_ex_03UITestsLaunchTests

+ (BOOL)runsForEachTargetApplicationUIConfiguration {
    return YES;
}

- (void)setUp {
    self.continueAfterFailure = NO;
}

- (void)testLaunch {
    XCUIApplication *app = [[XCUIApplication alloc] init];
    [app launch];

    // Insert steps here to perform after app launch but before taking a screenshot,
    // such as logging into a test account or navigating somewhere in the app

    XCTAttachment *attachment = [XCTAttachment attachmentWithScreenshot:XCUIScreen.mainScreen.screenshot];
    attachment.name = @"Launch Screen";
    attachment.lifetime = XCTAttachmentLifetimeKeepAlways;
    [self addAttachment:attachment];
}

@end
