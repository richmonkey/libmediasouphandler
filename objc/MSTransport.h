#import <Foundation/Foundation.h>

@interface MSFingerprint : NSObject
@property(nonatomic, copy) NSString *algorithm;
@property(nonatomic, copy) NSString *fingerprint;
-(instancetype)initWithAlgorithm:(NSString*)algorithm fingerprint:(NSString*)fingerprint;
@end

@interface MSTransport : NSObject

@property(nonatomic) MSFingerprint *fingerprint;

@end