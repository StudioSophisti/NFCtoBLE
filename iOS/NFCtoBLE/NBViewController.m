//
//  NBViewController.m
//  NFCtoBLE
//
//  Created by Ruben van Rooij on 04/04/14.
//  Copyright (c) 2014 Studio Sophisti. All rights reserved.
//

#import "NBViewController.h"
#import "NBBLEController.h"

@interface NBViewController ()

@end

@implementation NBViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [NBBLEController instance].delegate = self;
    
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning
{
    
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)peripheralDidConnect:(NBPeripheral*)peripheral
{
    self.connectedLabel.text = @"Connected: Yes";
    NSLog(@"Did connect");
    
}

- (void)peripheralDidDisconnect:(NBPeripheral*)peripheral
{
    self.connectedLabel.text = @"Connected: No";
    NSLog(@"Did disconnect");
}

- (void)peripheral:(NBPeripheral*)peripheral didReceiveCardUUID:(NSString*)cardUUID
{
    self.uuidLabel.text = cardUUID;
    NSLog(@"Did receive carduuid: %@", cardUUID);
}

@end
