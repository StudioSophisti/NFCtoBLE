//
//  NBViewController.h
//  NFCtoBLE
//
//  Created by Ruben van Rooij on 04/04/14.
//  Copyright (c) 2014 Studio Sophisti. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NBBLEControllerDelegate.h"

@interface NBViewController : UIViewController <NBBLEControllerDelegate>
@property (weak, nonatomic) IBOutlet UILabel *connectedLabel;
@property (weak, nonatomic) IBOutlet UILabel *uuidLabel;

@end
