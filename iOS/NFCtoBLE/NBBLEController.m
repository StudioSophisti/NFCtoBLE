//
//  NBBLEController.m
//  NFCtoBLE
//
//  Created by Ruben van Rooij on 04/04/14.
//  Copyright (c) 2014 Studio Sophisti. All rights reserved.
//

#import "NBBLEController.h"
#import "NBPeripheral.h"
#import "NBMacros.h"

static NBBLEController *__instance = nil;

@implementation NBBLEController

+ (NBBLEController*)instance {
    if (!__instance) {
        __instance = [[NBBLEController alloc] init];
    }
    
    return __instance;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (id)init {
    if ((self = [super init])) {
        
        _devices = [[NSMutableArray alloc] initWithCapacity:10];
        _manager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(connect)
                                                     name:UIApplicationDidBecomeActiveNotification object:nil];
    
    }
    
    return self;
}

- (void)connect {
    
    if (_manager.state == CBCentralManagerStatePoweredOn) {
        
        //check if other apps are connected
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f) {
            NSArray *peripherals = [_manager retrieveConnectedPeripheralsWithServices:
                                    [NSArray arrayWithObject:NFC_SERVICE_UUID]];
            
            for (CBPeripheral *peripheral in peripherals) {
                [self connectPeripheral:peripheral];
                return;
            }
            
            [self scanPeripherals];
            
        } else {
            
            [_manager retrieveConnectedPeripherals];
        }
    }
}

- (void)connectPeripheral:(CBPeripheral*)peripheral {
    
    if (connectionState > kConnectionStateScanning) return;
    
    if (scanTimer) {
        [scanTimer invalidate];
        scanTimer = nil;
    }
    
    connectionState = kConnectionStateConnecting;
    
    [_manager stopScan];
    
    NSLog(@"Connecting periphiral %@", peripheral.name);
    
    connectedPeripheral = [[NBPeripheral alloc] init];
    connectedPeripheral.delegate = self;
    connectedPeripheral.peripheral = peripheral;
    connectedPeripheral.peripheral.delegate = connectedPeripheral;
    
    [_manager connectPeripheral:peripheral options:nil];
}

- (void)scanPeripherals {
    
    connectionState = kConnectionStateScanning;
    
    NSLog(@"Scanning...");
    
    [_manager stopScan];
    
    [_devices removeAllObjects];
    
    NSArray	*uuidArray = [NSArray arrayWithObject:NFC_SERVICE_UUID];
	NSDictionary *options = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:NO] forKey:CBCentralManagerScanOptionAllowDuplicatesKey];
    [_manager scanForPeripheralsWithServices:uuidArray options:options];
    
    if (scanTimer) {
        [scanTimer invalidate];
        scanTimer = nil;
    }
    
    scanTimer = [NSTimer scheduledTimerWithTimeInterval:3 target:self selector:@selector(scanTimeout) userInfo:nil repeats:NO];
    
    if (deviceSheet) {
        [deviceSheet dismissWithClickedButtonIndex:-1 animated:NO];
    }
}

- (void)scanTimeout {
    
    scanTimer = nil;
    
    if ([_devices count] == 1) {
        [self connectPeripheral:[_devices objectAtIndex:0]];
    } else if ([_devices count] > 1) {
        
        [_manager stopScan];
        
        //show selection
        
        deviceSheet = [[UIActionSheet alloc] initWithTitle:@"Choose board to connect"
                                                  delegate:self
                                         cancelButtonTitle:nil
                                    destructiveButtonTitle:nil
                                         otherButtonTitles:nil];
        
        
        for (CBPeripheral* peripheral in _devices) {
            [deviceSheet addButtonWithTitle:peripheral.name];
        }
        
        [deviceSheet addButtonWithTitle:@"Search again..."];
        [deviceSheet showInView:[[[UIApplication sharedApplication] delegate] window]];
        
    } else {
        
        scanTimer = [NSTimer scheduledTimerWithTimeInterval:3 target:self selector:@selector(scanTimeout) userInfo:nil repeats:NO];
    }
}


#pragma mark - NBBLEControllerDelegate

- (void)peripheralDidConnect:(NBPeripheral*)peripheral
{
    if(_delegate && _delegate != self)
        [_delegate peripheralDidConnect:peripheral];
}

- (void)peripheralDidDisconnect:(NBPeripheral*)peripheral
{
    if(_delegate && _delegate != self)
        [_delegate peripheralDidDisconnect:peripheral];
}

- (void)peripheral:(NBPeripheral*)peripheral didReceiveData:(NSData*)data
{
    if(_delegate && _delegate != self)
        [_delegate peripheral:peripheral didReceiveData:data];
}

#pragma mark - CBCentralManagerDelegate

- (void)centralManagerDidUpdateState:(CBCentralManager *)central {
    NSLog(@"Central manager changed state: %d", central.state);
    
    if (central.state == CBCentralManagerStatePoweredOn) {
        [self connect];
        
    }
}

- (void)centralManager:(CBCentralManager *)central didRetrievePeripherals:(NSArray *)peripherals {
    NSLog(@"%d known periphirals retrieved", [peripherals count]);
}


- (void)centralManager:(CBCentralManager *)central didRetrieveConnectedPeripherals:(NSArray *)peripherals {
    NSLog(@"%d connected periphirals retrieved", [peripherals count]);
    
    for (CBPeripheral *peripheral in peripherals) {
        //TODO: check if it has the IKAWA service!
        [self connectPeripheral:peripheral];
        break;
    }
    
    
    if (![peripherals count]) {
        [self scanPeripherals];
    }
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI {
    NSLog(@"Periphiral discovered: %@, signal strength: %d", peripheral.name, RSSI.intValue);
    
    for (CBPeripheral *device in _devices) {
        if ([device.name isEqualToString:peripheral.name]) {
            return;
        }
    }
    
    [_devices addObject:peripheral];
    
    if ([peripheral.name isEqualToString:[[NSUserDefaults standardUserDefaults] objectForKey:@"last_connected_roaster"]]) {
        [self connectPeripheral:peripheral];
    }
}


- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    NSLog(@"Periphiral connected: %@", peripheral.name);
    
    [connectedPeripheral discoverServices];
    
    if (_delegate && [_delegate respondsToSelector:@selector(peripheralDidConnect:)]) {
        [_delegate peripheralDidConnect:connectedPeripheral];
    }
    
    connectionState = kConnectionStateConnected;
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
    NSLog(@"Periphiral disconnected: %@", peripheral.name);
    
    
    if (_delegate && [_delegate respondsToSelector:@selector(peripheralDidDisconnect:)]) {
        [_delegate peripheralDidDisconnect:connectedPeripheral];
    }
    
    [connectedPeripheral stopPolling];
    connectedPeripheral = nil;
    
    [self scanPeripherals];
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
    NSLog(@"Periphiral failed to connect: %@", peripheral.name);
    
    connectedPeripheral = nil;
    
    [self scanPeripherals];
}
@end
