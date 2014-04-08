//
//  NBPeripheral.h
//  NFCtoBLE
//
//  Created by Ruben van Rooij on 04/04/14.
//  Copyright (c) 2014 Studio Sophisti. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/Corebluetooth.h>
#import "NBBLEControllerDelegate.h"

@interface NBPeripheral : NSObject <CBPeripheralDelegate>
{
    uint8_t *buffer;
    
    CBCharacteristic *dataCharacteristic;
}

@property(nonatomic, retain) CBPeripheral *peripheral;
@property(nonatomic, assign) id<NBBLEControllerDelegate> delegate;

@end
