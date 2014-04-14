//
//  NBBLEControllerDelegate.h
//  NFCtoBLE
//
//  Created by Ruben van Rooij on 04/04/14.
//  Copyright (c) 2014 Studio Sophisti. All rights reserved.
//

#import <Foundation/Foundation.h>

@class NBPeripheral;

@protocol NBBLEControllerDelegate <NSObject>

- (void)peripheralDidConnect:(NBPeripheral*)peripheral;
- (void)peripheralDidDisconnect:(NBPeripheral*)peripheral;
- (void)peripheral:(NBPeripheral*)peripheral didReceiveCardUUID:(NSString*)cardUUID;

@end
