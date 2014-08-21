#import "BTVenmoAppSwitchURL.h"
#import "BTURLUtils.h"

#define kXCallbackTemplate @"scheme://x-callback-url/path"
#define kVenmoScheme @"com.venmo.touch.v1"

@implementation BTVenmoAppSwitchURL

+ (BOOL)isAppSwitchAvailable {
    NSURL *url = [self appSwitchBaseURLComponents].URL;
    return [[UIApplication sharedApplication] canOpenURL:url];
}

+ (NSURL *)appSwitchURLForMerchantID:(NSString *)merchantID returnURLScheme:(NSString *)scheme {
    NSDictionary *appSwitchParameters = @{
                                          @"x-success": [self returnURLWithScheme:scheme result:@"success"],
                                          @"x-error": [self returnURLWithScheme:scheme result:@"error"],
                                          @"x-cancel": [self returnURLWithScheme:scheme result:@"cancel"],
                                          @"x-source": scheme,
                                          @"braintree_merchant_id": merchantID
                                          };

    NSURLComponents *components = [self appSwitchBaseURLComponents];
    components.percentEncodedQuery = [BTURLUtils queryStringWithDictionary:appSwitchParameters];
    return components.URL;
}

#pragma mark Internal Helpers

+ (NSURL *)returnURLWithScheme:(NSString *)scheme result:(NSString *)result {
    NSURLComponents *components = [NSURLComponents componentsWithString:kXCallbackTemplate];
    components.scheme = scheme;
    components.percentEncodedPath = [NSString stringWithFormat:@"/vzero/auth/venmo/%@", result];
    return components.URL;
}

+ (NSURLComponents *)appSwitchBaseURLComponents {
    NSURLComponents *components = [NSURLComponents componentsWithString:kXCallbackTemplate];
    components.scheme = kVenmoScheme;
    components.percentEncodedPath = @"/vzero/auth";
    return components;
}

@end