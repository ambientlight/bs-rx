open Rx_Types;

module Observable = {
  type t('a);
  type observableT('a) = t('a);

  module Impl = (T: { type t('a); }) => {
    type teardown;
    type teardownFunc = unit => unit;
    type operatorObject('a, 't) = Js.t({ 
      ..
      call: (Rx_Subscriber.t('a), T.t('a)) => teardown
    } as 't);

    external asObservable : T.t('a) => observableT('a) = "%identity";

    // NOTE: comment blocks are unpadded back for the cleaner vscode comments hightlighting on hover

  /**
  creates a new observable

    @param subscribe the function that is called when the Observable is initially subscribed to. 
    This function is given a Subscriber, to which new values can be `next`ed, 
    or an `error` method can be called to raise an error, or `complete` can be called to notify of a successful completion.
   */
    [@bs.module "rxjs"] [@bs.new]
    external create: (~subscribe: 
      [@bs.unwrap][
        | `NoTeardown(Rx_Subscriber.t('a) => unit) 
        | `FunctionTeardown(Rx_Subscriber.t('a) => teardownFunc)
        | `UnsubscribableTeardown(Rx_Subscriber.t('a) => Unsubscribable.t('b))
      ]=?, unit) => T.t('a) = "Observable";

    // deprecated
    //[@bs.val][@bs.module "rxjs"][@bs.scope "Observable"]
    //external createStatic: (~subscribe: (Rx_Subscriber.t('a) => teardownLogic)=?, unit) => t('a) = "create";

  /**
  Creates a new Observable, with this Observable as the source, and the passed
  operator defined as the new observable's operator.
  
    @param operator the operator defining the operation to take on the observable
    @return a new observable with the Operator applied
   */
    [@bs.send.pipe: T.t('a)]
    external lift: (operatorObject('a, 't)) => T.t('a) = "lift";

    [@bs.send] 
    external _subscribe: (
      T.t('a),
      ~next: [@bs.uncurry]('a => unit),
      ~error: [@bs.uncurry]('error => unit),
      ~complete: [@bs.uncurry](unit => unit)
    ) => Rx_Subscription.t = "subscribe";

  /**
  Invokes an execution of an Observable and registers Observer handlers for notifications it will emit.

    @param observer observer with methods to be called
    @return a subscription reference to the registered handlers
   */
    [@bs.send.pipe: T.t('a)]
    external subscribeObserver: (
      ~observer: Observer.t('a, 'error)=?
    ) => Rx_Subscription.t = "subscribe";

  /**
  Invokes an execution of an Observable and registers Observer handlers for notifications it will emit.

    @param next handler for each value emitted from the subscribed Observable
    @param error A handler for a terminal event resulting from an error. If no error handler is provided, the error will be thrown as unhandled.
    @param complete A handler for a terminal event resulting from successful completion.
    @return a subscription reference to the registered handlers
   */
    let subscribe = (~next=?, ~error=?, ~complete=?, obs) =>
      obs |> subscribeObserver(~observer=Observer.t(~next?, ~error?, ~complete?, ()));

  /**
  returns a promise that will either resolve or reject when the Observable completes or errors
    
    @param next a handler for each value emitted by the observable
    @return a promise that either resolves on observable completion or rejects with the handled error
    */
    [@bs.send.pipe: T.t('a)]
    external forEach: (
      [@bs.uncurry]('a => unit)
      // TODO: ~promiseCtor: 
    ) => Js.Promise.t(unit) = "forEach";

  /**
  Subscribe to this Observable and get a Promise resolving on `complete` with the last emission.

    @return A Promise that resolves with the last value emit, or rejects on an error.    
   */
    // TODO: ~promiseCtor: 
    [@bs.send.pipe: T.t('a)]
    external toPromise: Js.Promise.t('a) = "toPromise";
  };

  include Impl({ type nonrec t('a) = t('a); });
};

/**
  Converts the arguments to an observable sequence.
  Unlike {@link from}, it does not do any flattening and emits each argument in whole as a separate `next` notification.

    @param A list of arguments you want to be emitted
    @return An Observable that emits the arguments described above and then completes
 */
[@bs.module "rxjs"][@bs.variadic] external of_: array('a) => Observable.t('a) = "of";

/**
  Converts the arguments to an observable sequence.
  Unlike {@link from}, it does not do any flattening and emits each argument in whole as a separate `next` notification.

    @param A list of arguments you want to be emitted
    @return An Observable that emits the arguments described above and then completes
 */
[@bs.module "rxjs"] external of1: 'a => Observable.t('a) = "of";

/**
  Creates an Observable that emits no items to the Observer and immediately emits a complete notification.
 */
[@bs.module "rxjs"][@bs.val] external empty: Observable.t('a) = "EMPTY";
