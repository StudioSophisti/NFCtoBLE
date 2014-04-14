//
//  NBPeripheral.m
//  NFCtoBLE
//
//  Created by Ruben van Rooij on 04/04/14.
//  Copyright (c) 2014 Studio Sophisti. All rights reserved.
//

#import "NBPeripheral.h"
#import "NBMacros.h"

@implementation NBPeripheral

- (id)init {
    if ((self = [super init])) {
        
        buffer = malloc(sizeof(uint8_t) * 1024);
    }
    return self;
}

- (void)dealloc {
    
    free(buffer);
    buffer = NULL;
}

- (void)stopPolling {
    
}

- (void)discoverServices {
    // discover our hid and battery service
    
    NSMutableArray *services = [NSMutableArray arrayWithObjects:NFC_SERVICE_UUID, nil];
    
    
    [_peripheral discoverServices:services];
    
}

#pragma mark - CBPeripheralDelegate

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error {
    
    for (CBService *service in peripheral.services) {
        
        if ([service.UUID isEqual:NFC_SERVICE_UUID]) {
            
            [peripheral discoverCharacteristics:[NSArray arrayWithObjects:NFC_CHAR_UUID, nil]
                                     forService:service];
        }
        
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error {
    
    for (CBCharacteristic *characteristic in service.characteristics) {
        
        if ([characteristic.UUID isEqual:NFC_CHAR_UUID]) {
            
            dataCharacteristic = characteristic;
            [peripheral setNotifyValue:YES forCharacteristic:dataCharacteristic];
            
        }
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    
    // Not needed right now.
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    
    if(dataCharacteristic == characteristic)
    {
        NSString *raw = [NSString stringWithFormat:@"0x%@", characteristic.value];
        raw = [raw stringByReplacingOccurrencesOfString:@"<" withString:@""];
        raw = [raw stringByReplacingOccurrencesOfString:@">" withString:@""];
        
        if (_delegate && [_delegate respondsToSelector:@selector(peripheral:didReceiveCardUUID:)]) {
            [_delegate peripheral:self didReceiveCardUUID:raw];
        }
    }
}




@end
