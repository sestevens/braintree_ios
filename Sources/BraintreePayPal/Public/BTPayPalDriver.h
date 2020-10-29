#import <BraintreeCore/BraintreeCore.h>
@class BTPayPalAccountNonce;
@class BTPayPalRequest;
@class BTPayPalApprovalRequest;

NS_ASSUME_NONNULL_BEGIN

/**
 Domain for PayPal errors.
 */
extern NSString *const BTPayPalDriverErrorDomain;

/**
 Error codes associated with PayPal.
 */
typedef NS_ENUM(NSInteger, BTPayPalDriverErrorType) {
    /// Unknown error
    BTPayPalDriverErrorTypeUnknown = 0,

    /// PayPal is disabled in configuration
    BTPayPalDriverErrorTypeDisabled,

    /// Browser switch is not configured appropriately. You must specify a
    /// valid returnURLScheme via BTAppSwitch before attempting a browser switch
    BTPayPalDriverErrorTypeIntegrationReturnURLScheme,

    /// UIApplication failed to switch despite it being available.
    /// `[UIApplication openURL:]` returned `NO` when `YES` was expected
    BTPayPalDriverErrorTypeAppSwitchFailed,

    /// Invalid configuration, e.g. bad CFBundleDisplayName
    BTPayPalDriverErrorTypeInvalidConfiguration,

    /// Invalid request, e.g. missing PayPal request
    BTPayPalDriverErrorTypeInvalidRequest,
    
    /// Braintree SDK is integrated incorrectly
    BTPayPalDriverErrorTypeIntegration,
};

/**
 Protocol to handle custom PayPal Approval via BTPayPalApprovalHandler
*/
@protocol BTPayPalApprovalDelegate
/**
 Use when custom approval has completed with success or error
*/
- (void)onApprovalComplete:(NSURL *) url;

/**
 Use when custom approval was canceled
*/
- (void)onApprovalCancel;

@end

/**
 Protocol for custom authentication and authorization of PayPal.
*/
@protocol BTPayPalApprovalHandler

/**
 Handle approval request for PayPal and carry out custom authentication and authorization.

 Use the delegate to handle success/error/cancel flows.
 On completion or error use BTPayPalApprovalDelegate:onApprovalComplete
 On cancel use BTPayPalApprovalDelegate:onApprovalCancel

 @param request BTPayPalApprovalRequest request object.
 @param delegate The BTPayPalApprovalDelegate to handle response.
*/
- (void)handleApproval:(BTPayPalApprovalRequest *)request paypalApprovalDelegate:(id<BTPayPalApprovalDelegate>)delegate;

@end

/** 
 BTPayPalDriver enables you to obtain permission to charge your customers' PayPal accounts by presenting the PayPal website.

 @note To make PayPal available, you must ensure that PayPal is enabled in your Braintree control panel.
 See our [online documentation](https://developers.braintreepayments.com/ios+ruby/guides/paypal) for
 details.

 This class supports two basic use-cases: Vault and Checkout. Each of these involves variations on the
 user experience as well as variations on the capabilities granted to you by this authorization.

 The *Vault* option uses PayPal's future payments authorization, which allows your merchant account to
 charge this customer arbitrary amounts for a long period of time into the future (unless the user
 manually revokes this permission in their PayPal control panel.) This authorization flow includes
 a screen with legal language that directs the user to agree to the terms of Future Payments.
 Unfortunately, it is not currently possible to collect shipping information in the Vault flow.

 The *Checkout* option creates a one-time use PayPal payment on your behalf. As a result, you must
 specify the checkout details up-front, so that they can be shown to the user during the PayPal flow.
 With this flow, you must specify the estimated transaction amount, and you can collect shipping
 details. This flow omits the Future Payments agreement, and the resulting payment method cannot be
 stored in the vault. It is only possible to create one Braintree transaction with this form of user
 approval.

 The user experience takes full advantage of One Touch. This
 means that users may bypass the username/password entry screen when they are already logged in.

 Upon successful completion, you will receive a `BTPayPalAccountNonce`, which includes user-facing
 details and a payment method nonce, which you must pass to your server in order to create a transaction
 or save the authorization in the Braintree vault (not possible with Checkout).

 ## User Experience Details

 To keep your UI in sync during authentication, you may set a delegate, which will be notified
 as the PayPal driver progresses through the various steps necessary for user
 authentication.

*/
@interface BTPayPalDriver : NSObject <BTAppSwitchHandler, BTPayPalApprovalDelegate>

/**
 Initialize a new PayPal driver instance.

 @param apiClient The API client
*/
- (instancetype)initWithAPIClient:(BTAPIClient *)apiClient;

/**
 Base initializer - do not use.
 */
- (instancetype)init __attribute__((unavailable("Please use initWithAPIClient:")));

/**
 Check out with PayPal to create a single-use PayPal payment method nonce.

 @note You can use this as the final step in your order/checkout flow. If you want, you may create a transaction from your
 server when this method completes without any additional user interaction.

 On success, you will receive an instance of `BTPayPalAccountNonce`; on failure, an error; on user cancellation,
 you will receive `nil` for both parameters.

 @param request A PayPal request
 @param completionBlock This completion will be invoked exactly once when checkout is complete or an error occurs.
 */
- (void)requestOneTimePayment:(BTPayPalRequest *)request
                   completion:(void (^)(BTPayPalAccountNonce * _Nullable tokenizedPayPalAccount, NSError * _Nullable error))completionBlock;


/**
 Check out with PayPal to create a single-use PayPal payment method nonce.

 @note You can use this as the final step in your order/checkout flow. If you want, you may create a transaction from your
 server when this method completes without any additional user interaction.

 On success, you will receive an instance of `BTPayPalAccountNonce`; on failure, an error; on user cancellation,
 you will receive `nil` for both parameters.

 @note This method is mutually exclusive with `authorizeAccountWithCompletion:`. In both cases, you need to create a
 Braintree transaction from your server in order to actually move money!

 @param request A PayPal request
 @param handler A BTPayPalApprovalHandler for custom authorization and approval
 @param completionBlock This completion will be invoked exactly once when checkout is complete or an error occurs.
 */
- (void)requestOneTimePayment:(BTPayPalRequest *)request
                      handler:(id<BTPayPalApprovalHandler>)handler
                   completion:(void (^)(BTPayPalAccountNonce * _Nullable tokenizedPayPalAccount, NSError * _Nullable error))completionBlock;

/**
 Create a PayPal Billing Agreement for repeat purchases.

 @note You can use this as the final step in your order/checkout flow. If you want, you may create a transaction from your
 server when this method completes without any additional user interaction.
 
 On success, you will receive an instance of `BTPayPalAccountNonce`; on failure, an error; on user cancellation,
 you will receive `nil` for both parameters.

 @param request A PayPal request
 @param completionBlock This completion will be invoked exactly once when checkout is complete or an error occurs.
*/
- (void)requestBillingAgreement:(BTPayPalRequest *)request
                     completion:(void (^)(BTPayPalAccountNonce * _Nullable tokenizedPayPalAccount, NSError * _Nullable error))completionBlock;

/**
 Create a PayPal Billing Agreement for repeat purchases.

 @note You can use this as the final step in your order/checkout flow. If you want, you may create a transaction from your
 server when this method completes without any additional user interaction.

 On success, you will receive an instance of `BTPayPalAccountNonce`; on failure, an error; on user cancellation,
 you will receive `nil` for both parameters.

 @note This method is mutually exclusive with `authorizeAccountWithCompletion:`. In both cases, you need to create a
 Braintree transaction from your server in order to actually move money!

 @param request A PayPal request
 @param handler A BTPayPalApprovalHandler for custom authorization and approval
 @param completionBlock This completion will be invoked exactly once when checkout is complete or an error occurs.
 */
- (void)requestBillingAgreement:(BTPayPalRequest *)request
                        handler:(id<BTPayPalApprovalHandler>)handler
                     completion:(void (^)(BTPayPalAccountNonce * _Nullable tokenizedPayPalAccount, NSError * _Nullable error))completionBlock;

#pragma mark - Delegate

/**
 An optional delegate for receiving notifications about the lifecycle of a PayPal browser switch for updating your UI
*/
@property (nonatomic, weak, nullable) id<BTAppSwitchDelegate> appSwitchDelegate;

/**
 A required delegate to control the presentation and dismissal of view controllers
*/
@property (nonatomic, weak, nullable) id<BTViewControllerPresentingDelegate> viewControllerPresentingDelegate;

@end

NS_ASSUME_NONNULL_END
