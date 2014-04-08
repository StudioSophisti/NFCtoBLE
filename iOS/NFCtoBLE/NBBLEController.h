//
//  NBBLEController.h
//  NFCtoBLE
//
//  Created by Ruben van Rooij on 04/04/14.
//  Copyright (c) 2014 Studio Sophisti. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "NBBLEControllerDelegate.h"

typedef enum {
    kConnectionStateIdle = 0,
    kConnectionStateScanning,
    kConnectionStateConnecting,
    kConnectionStateConnected
} ConnectionState;

@class NBPeripheral;

@interface NBBLEController : NSObject <NBBLEControllerDelegate, CBCentralManagerDelegate, UIActionSheetDelegate>
{
    ConnectionState connectionState;
    NBPeripheral connectedPeripheral;
    NSTimer *scanTimer;
    UIActionSheet *deviceSheet;
    
    //UIActionSheet *deviceSheet;
}

@property (nonatomic, assign) id<NBBLEControllerDelegate> delegate;
@property (nonatomic, readonly) CBCentralManager *manager;
@property (nonatomic,readonly) NSMutableArray *devices;


+ (NBBLEController*)instance;

@end
