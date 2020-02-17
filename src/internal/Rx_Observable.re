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

  /**
  Subscribe to this Observable and get a Promise resolving on `complete` with the last emission.

    @return A Promise that resolves with the last value emit, or rejects on an error.    
   */
    [@bs.send.pipe: T.t('a)]
    external toRepromise: Promise.t('a) = "toPromise";
  };

  include Impl({ type nonrec t('a) = t('a); });
};

type obsUnitCreator('a) = (. unit) => Observable.t('a);
type obsCreator('arg, 'a) = (. 'arg) => Observable.t('a);
type obsCreator2('arg1, 'arg2, 'b) = (. 'arg1, 'arg2) => Observable.t('b);
type obsCreator3('arg1, 'arg2, 'arg3, 'b) = (. 'arg1, 'arg2, 'arg3) => Observable.t('b);
type obsCreator4('arg1, 'arg2, 'arg3, 'arg4, 'b) = (. 'arg1, 'arg2, 'arg3, 'arg4) => Observable.t('b);
type obsCreator5('arg1, 'arg2, 'arg3, 'arg4, 'arg5, 'b) = (. 'arg1, 'arg2, 'arg3, 'arg4, 'arg5) => Observable.t('b);

/**
  Converts a callback API to a function that returns an Observable.

  Give it a function `f` of type `f(x, callback)` and
  it will return a function `g` that when called as `g(x)` will output an
  Observable.

    @param func A function with a callback as the last parameter.
    @param {SchedulerLike} [scheduler] The scheduler on which to schedule the callbacks.
    @return A function which returns the Observable that delivers the same values the callback would deliver.
 */
[@bs.module "rxjs"]
external bindCallback: ([@bs.uncurry](unit => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsUnitCreator(unit) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback1Arg0: ([@bs.uncurry]('a => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator(unit, 'a) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback2Arg0: ([@bs.uncurry](('a, 'b) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator(unit, ('a, 'b)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback3Arg0: ([@bs.uncurry](('a, 'b, 'c) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator(unit, ('a, 'b, 'c)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback4Arg0: ([@bs.uncurry](('a, 'b, 'c, 'd) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator(unit, ('a, 'b, 'c, 'd)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback5Arg0: ([@bs.uncurry](('a, 'b, 'c, 'd, 'e) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator(unit, ('a, 'b, 'c, 'd, 'e)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback6Arg0: ([@bs.uncurry](('a, 'b, 'c, 'd, 'e, 'f) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator(unit, ('a, 'b, 'c, 'd, 'e, 'f)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback7Arg0: ([@bs.uncurry](('a, 'b, 'c, 'd, 'e, 'f, 'g) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator(unit, ('a, 'b, 'c, 'd, 'e, 'f, 'g)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback8Arg0: ([@bs.uncurry](('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator(unit, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback1Arg1: ([@bs.uncurry]('a1, 'a => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator('a1, 'a) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback2Arg1: ([@bs.uncurry]('a1, ('a, 'b) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator('a1, ('a, 'b)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback3Arg1: ([@bs.uncurry]('a1, ('a, 'b, 'c) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator('a1, ('a, 'b, 'c)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback4Arg1: ([@bs.uncurry]('a1, ('a, 'b, 'c, 'd) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator('a1, ('a, 'b, 'c, 'd)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback5Arg1: ([@bs.uncurry]('a1, ('a, 'b, 'c, 'd, 'e) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator('a1, ('a, 'b, 'c, 'd, 'e)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback6Arg1: ([@bs.uncurry]('a1, ('a, 'b, 'c, 'd, 'e, 'f) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator('a1, ('a, 'b, 'c, 'd, 'e, 'f)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback7Arg1: ([@bs.uncurry]('a1, ('a, 'b, 'c, 'd, 'e, 'f, 'g) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator('a1, ('a, 'b, 'c, 'd, 'e, 'f, 'g)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback8Arg1: ([@bs.uncurry]('a1, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator('a1, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback1Arg2: ([@bs.uncurry]('a1, 'a2, 'a => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator2('a1, 'a2, 'a) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback2Arg2: ([@bs.uncurry]('a1, 'a2, ('a, 'b) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator2('a1, 'a2, ('a, 'b)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback3Arg2: ([@bs.uncurry]('a1, 'a2, ('a, 'b, 'c) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator2('a1, 'a2, ('a, 'b, 'c)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback4Arg2: ([@bs.uncurry]('a1, 'a2, ('a, 'b, 'c, 'd) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator2('a1, 'a2, ('a, 'b, 'c, 'd)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback5Arg2: ([@bs.uncurry]('a1, 'a2, ('a, 'b, 'c, 'd, 'e) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator2('a1, 'a2, ('a, 'b, 'c, 'd, 'e)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback6Arg2: ([@bs.uncurry]('a1, 'a2, ('a, 'b, 'c, 'd, 'e, 'f) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator2('a1, 'a2, ('a, 'b, 'c, 'd, 'e, 'f)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback7Arg2: ([@bs.uncurry]('a1, 'a2, ('a, 'b, 'c, 'd, 'e, 'f, 'g) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator2('a1, 'a2, ('a, 'b, 'c, 'd, 'e, 'f, 'g)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback8Arg2: ([@bs.uncurry]('a1, 'a2, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator2('a1, 'a2, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback1Arg3: ([@bs.uncurry]('a1, 'a2, 'a3, 'a => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator3('a1, 'a2, 'a3, 'a) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback2Arg3: ([@bs.uncurry]('a1, 'a2, 'a3, ('a, 'b) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator3('a1, 'a2, 'a3, ('a, 'b)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback3Arg3: ([@bs.uncurry]('a1, 'a2, 'a3, ('a, 'b, 'c) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator3('a1, 'a2, 'a3, ('a, 'b, 'c)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback4Arg3: ([@bs.uncurry]('a1, 'a2, 'a3, ('a, 'b, 'c, 'd) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator3('a1, 'a2, 'a3, ('a, 'b, 'c, 'd)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback5Arg3: ([@bs.uncurry]('a1, 'a2, 'a3, ('a, 'b, 'c, 'd, 'e) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator3('a1, 'a2, 'a3, ('a, 'b, 'c, 'd, 'e)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback6Arg3: ([@bs.uncurry]('a1, 'a2, 'a3, ('a, 'b, 'c, 'd, 'e, 'f) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator3('a1, 'a2, 'a3, ('a, 'b, 'c, 'd, 'e, 'f)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback7Arg3: ([@bs.uncurry]('a1, 'a2, 'a3, ('a, 'b, 'c, 'd, 'e, 'f, 'g) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator3('a1, 'a2, 'a3, ('a, 'b, 'c, 'd, 'e, 'f, 'g)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback8Arg3: ([@bs.uncurry]('a1, 'a2, 'a3, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator3('a1, 'a2, 'a3, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback1Arg4: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator4('a1, 'a2, 'a3, 'a4, 'a) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback2Arg4: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, ('a, 'b) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator4('a1, 'a2, 'a3, 'a4, ('a, 'b)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback3Arg4: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator4('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback4Arg4: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator4('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback5Arg4: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd, 'e) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator4('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd, 'e)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback6Arg4: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd, 'e, 'f) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator4('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd, 'e, 'f)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback7Arg4: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd, 'e, 'f, 'g) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator4('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd, 'e, 'f, 'g)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback8Arg4: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator4('a1, 'a2, 'a3, 'a4, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback1Arg5: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a5, 'a => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator5('a1, 'a2, 'a3, 'a4, 'a5, 'a) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback2Arg5: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator5('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback3Arg5: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator5('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback4Arg5: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator5('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback5Arg5: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd, 'e) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator5('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd, 'e)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback6Arg5: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd, 'e, 'f) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator5('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd, 'e, 'f)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback7Arg5: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd, 'e, 'f, 'g) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator5('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd, 'e, 'f, 'g)) = "bindCallback";
[@bs.module "rxjs"]
external bindCallback8Arg5: ([@bs.uncurry]('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h) => 'cr) => 'fr, ~scheduler: Rx_Scheduler.t=?, unit) => obsCreator5('a1, 'a2, 'a3, 'a4, 'a5, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h)) = "bindCallback";

/**
  Combines multiple Observables to create an Observable whose values are
  calculated from the latest values of each of its input Observables.
  
  Whenever any input Observable emits a value, it
  computes a formula using the latest values from all the inputs, then emits
  the output of that formula.

    @param An array of input Observables to combine with each other.
    @return An Observable of projected values from the most recent values from each input Observable, or an array of the most recent values from each input Observable.
 */
[@bs.module "rxjs"]
external combineLatest: array(Observable.t('a)) => Observable.t(array('a)) = "combineLatest";
[@bs.module "rxjs"]
external combineLatest2: (Observable.t('b), Observable.t('c)) => Observable.t(('b, 'c)) = "combineLatest";
[@bs.module "rxjs"]
external combineLatest3: (Observable.t('b), Observable.t('c), Observable.t('d)) => Observable.t(('b, 'c, 'd)) = "combineLatest";
[@bs.module "rxjs"]
external combineLatest4: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e)) => Observable.t(('b, 'c, 'd, 'e)) = "combineLatest";
[@bs.module "rxjs"]
external combineLatest5: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f)) => Observable.t(('b, 'c, 'd, 'e, 'f)) = "combineLatest";
[@bs.module "rxjs"]
external combineLatest6: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g)) = "combineLatest";
[@bs.module "rxjs"]
external combineLatest7: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g), Observable.t('h)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g, 'h)) = "combineLatest";
[@bs.module "rxjs"]
external combineLatest8: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g), Observable.t('h), Observable.t('i)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g, 'h, 'i)) = "combineLatest";

/**
  Creates an output Observable which sequentially emits all values from given
  Observable and then moves on to the next.

  Concatenates multiple Observables together by
  sequentially emitting their values, one Observable after the other.

    @param An array of input Observables to concat with each other.
    @return Combined observable
 */
[@bs.module "rxjs"][@bs.variadic]
external concat: array(Observable.t('a))  => Observable.t('a) = "concat";

/**
  Creates an Observable that, on subscribe, calls an Observable factory to
  make an Observable for each new Observer.

  Creates the Observable lazily, that is, only when it is subscribed.

    @param observableFactory The Observable factory function to invoke for each Observer that subscribes to the output Observable. May also return a Promise, which will be converted on the fly to an Observable.
    @return An Observable whose Observers' subscriptions trigger an invocation of the given Observable factory function.
 */
[@bs.module "rxjs"]
external defer: (
  [@bs.unwrap] [
    | `Observable(unit => Observable.t('a))
    | `Promise(unit => Js.Promise.t('a))
    | `Repromise(unit => Promise.t('a))
    | `Array(unit => array('a))
  ]
) => Observable.t('a) = "defer";

/**
  Accepts an `Array` of ObservableInput or a dictionary `Object` of ObservableInput and returns
  an Observable that emits either an array of values in the exact same order as the passed array,
  or a dictionary of values in the same shape as the passed dictionary.

  Wait for Observables to complete and then combine last values they emitted.

    @param sources Any number of Observables provided either as an array or as an arguments passed directly to the operator.
    @return Observable emitting either an array of last values emitted by passed Observables or value from project function.
 */
[@bs.module "rxjs"]
external forkJoin: (
  [@bs.unwrap][
    | `PromiseArray(array(Js.Promise.t('a)))
    | `RepromiseArray(array(Promise.t('a)))
    | `ObservableArray(array(Observable.t('a)))
    | `ArrayLikeArray(array(array('a)))
    | `Dict(Js.t({..}))
  ]
) => Observable.t('a) = "forkJoin";

[@bs.module "rxjs"]
external forkJoin2: (Observable.t('b), Observable.t('c)) => Observable.t(('b, 'c)) = "forkJoin";
[@bs.module "rxjs"]
external forkJoin3: (Observable.t('b), Observable.t('c), Observable.t('d)) => Observable.t(('b, 'c, 'd)) = "forkJoin";
[@bs.module "rxjs"]
external forkJoin4: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e)) => Observable.t(('b, 'c, 'd, 'e)) = "forkJoin";
[@bs.module "rxjs"]
external forkJoin5: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f)) => Observable.t(('b, 'c, 'd, 'e, 'f)) = "forkJoin";
[@bs.module "rxjs"]
external forkJoin6: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g)) = "forkJoin";
[@bs.module "rxjs"]
external forkJoin7: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g), Observable.t('h)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g, 'h)) = "forkJoin";
[@bs.module "rxjs"]
external forkJoin8: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g), Observable.t('h), Observable.t('i)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g, 'h, 'i)) = "forkJoin";

/**
  Creates an Observable from an Array, an array-like object, a Promise, an iterable object, or an Observable-like object.

    @param A subscription object, a Promise, an Observable-like, an Array
    @param An optional {@link SchedulerLike} on which to schedule the emission of values.
    @return An observable
 */
[@bs.module "rxjs"]
external from: (
  [@bs.unwrap][
    | `Array(array('a))
    | `Promise(Js.Promise.t('a))
    | `Repromise(Promise.t('a))
    | `Observable(Observable.t('a))
    | `Subscription(Rx_Subscription.t)
  ],
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => Observable.t('a) = "from";

/**
  Creates an Observable that emits events of a specific type coming from the
  given event target.

  Creates an Observable from DOM events, or Node.js EventEmitter events or others.

    @param target The DOM EventTarget, Node.js, EventEmitter, JQuery-like event target, NodeList or HTMLCollection to attach the event handler to
    @param eventName The event name of interest, being emitted by the `target`
    @return observable
 */
[@bs.module "rxjs"]
external fromEvent: (
  ~target: 'target,
  ~eventName: string
) => Observable.t('a) = "fromEvent";

/**
  Creates an Observable that emits events of a specific type coming from the
  given event target.

  Creates an Observable from DOM events, or Node.js EventEmitter events or others.

    @param target The DOM EventTarget, Node.js, EventEmitter, JQuery-like event target, NodeList or HTMLCollection to attach the event handler to
    @param eventName The event name of interest, being emitted by the `target`
    @param options Options to pass through to addEventListener
    @return observable
 */
[@bs.module "rxjs"]
external fromEventWithOptions: (
  ~target: 'target,
  ~eventName: string,
  ~options: Rx_Types.EventListenerOptions.t
) => Observable.t('a) = "fromEvent";

/**
  Creates an Observable from an arbitrary API for registering event handlers.
  When that method for adding event handler was something was not prepared for.

    @param addHandler A function that takes a `handler` function as argument and attaches it somehow to the actual source of events.
    @param removeHandler A function that takes a `handler` function as an argument and removes it from the event source. If `addHandler` returns some kind of token, `removeHandler` function will have it as a second parameter.
    @return Observable which, when an event happens, emits first parameter passed to registered event handler. Alternatively it emits whatever project function returns at that moment.
 */
[@bs.module "rxjs"]
external fromEventPattern: (
  ~addHandler: [@bs.uncurry]('handler => 'res),
  ~removeHandler: [@bs.uncurry](('handler, 'signal) => unit)=?,
  unit
) => Observable.t('a) = "fromEventPattern";

/**
  Generates an observable sequence by running a state-driven loop
  producing the sequence's elements, using the specified scheduler to send out observer messages.

    @param initialState Initial state.
    @param condition Condition to terminate generation (upon returning false).
    @param iterate Iteration step function.
    @param scheduler A Scheduler on which to run the generator loop. If not provided, defaults to emit immediately.
    @return Observable of generated sequence.
 */
[@bs.module "rxjs"]
external generate: (
  ~initialState: 's,
  ~condition: [@bs.uncurry]('s => bool),
  ~iterate: [@bs.uncurry]('s => 's),
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => Observable.t('t) = "generate";

/**
  Decides at subscription time which Observable will actually be subscribed.
  `If` statement for Observables.

    @param condition Condition which Observable should be chosen.
    @param trueObservable An Observable that will be subscribed if condition is true.
    @param falseObservable An Observable that will be subscribed if condition is false.
    @return Either first or second Observable, depending on condition
 */
[@bs.module "rxjs"]
external iif: (
  ~condition: [@bs.uncurry](unit => bool),
  ~trueResult: [@bs.unwrap][
    | `Observable(Observable.t('a))
    | `Promise(Js.Promise.t('a))
    | `Repromise(Promise.t('a))
  ],
  ~falseResult: [@bs.unwrap][
    | `Observable(Observable.t('a))
    | `Promise(Js.Promise.t('a))
    | `Repromise(Promise.t('a))
  ] 
) => Observable.t('a) = "iif";

/**
  Creates an Observable that emits sequential numbers every specified
  interval of time, on a specified Scheduler.

  Emits incremental numbers periodically in time.

    @param period The interval size in milliseconds (by default) or the time unit determined by the scheduler's clock.
    @param scheduler The Scheduler to use for scheduling the emission of values, and providing a notion of "time".
    @return Observable that emits a sequential number each time interval.
 */
[@bs.module "rxjs"]
external interval: (
  ~period: int,
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => Observable.t(int) = "interval";

/**
  Creates an output Observable which concurrently emits all values from every
  given input Observable.

  Flattens multiple Observables together by blending their values into one Observable.

    @param observables Input Observables to merge together.
    @return an Observable that emits items that are the result of every input Observable.
 */
[@bs.module "rxjs"][@bs.variadic]
external merge: array(Observable.t('a))  => Observable.t('a) = "merge";

//TODO: rxjs implementation won't accept undefined in merge. Rethink how to support ~conncurrent, ~scheduler

[@bs.module "rxjs"]
external merge2C: (Observable.t('a), Observable.t('a), ~concurrent: int)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge2S: (Observable.t('a), Observable.t('a), ~scheduler: Rx_Scheduler.t)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge2SC: (Observable.t('a), Observable.t('a), ~concurrent: int, ~scheduler: Rx_Scheduler.t)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge3C: (Observable.t('a), Observable.t('a), Observable.t('a), ~concurrent: int)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge3S: (Observable.t('a), Observable.t('a), Observable.t('a), ~scheduler: Rx_Scheduler.t)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge3SC: (Observable.t('a), Observable.t('a), Observable.t('a), ~concurrent: int, ~scheduler: Rx_Scheduler.t)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge4C: (Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), ~concurrent: int)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge4S: (Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), ~scheduler: Rx_Scheduler.t)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge4SC: (Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), ~concurrent: int, ~scheduler: Rx_Scheduler.t)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge5C: (Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), ~concurrent: int)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge5S: (Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), ~scheduler: Rx_Scheduler.t)  => Observable.t('a) = "merge";
[@bs.module "rxjs"]
external merge5SC: (Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), Observable.t('a), ~concurrent: int, ~scheduler: Rx_Scheduler.t)  => Observable.t('a) = "merge";

/**
  Converts the arguments to an observable sequence.
  Unlike from, it does not do any flattening and emits each argument in whole as a separate `next` notification.

    @param A list of arguments you want to be emitted
    @return An Observable that emits the arguments described above and then completes
 */
[@bs.module "rxjs"][@bs.variadic] external of_: array('a) => Observable.t('a) = "of";

/**
  Converts the arguments to an observable sequence.
  Unlike from, it does not do any flattening and emits each argument in whole as a separate `next` notification.

    @param A list of arguments you want to be emitted
    @return An Observable that emits the arguments described above and then completes
 */
[@bs.module "rxjs"] external of1: 'a => Observable.t('a) = "of";

/**
  When any of the provided Observable emits an complete or error notification, it immediately subscribes to the next one
  that was passed.

  Execute series of Observables no matter what, even if it means swallowing errors.

    @param Observables passed as an array.
    @return An Observable that concatenates all sources, one after the other, ignoring all errors, such that any error causes it to move on to the next source.
 */
[@bs.module "rxjs"][@bs.variadic]
external onErrorResumeNext: array(Observable.t('a)) => Observable.t('a) = "onErrorResumeNext";

/**
  Convert JS object into an Observable of `[key, value]` pairs.

    @param obj The object to inspect and turn into an Observable sequence.
    @param scheduler An optional IScheduler to schedule when resulting Observable will emit values.
    @return An observable sequence of [key, value] pairs from the object.
 */
[@bs.module "rxjs"]
external pairs: (
  Js.t({..}),
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => Observable.t((string, 'a)) = "pairs";

/**
  Splits the source Observable into two, one with values that satisfy a predicate, 
  and another with values that don't satisfy the predicate

    @param source source observable
    @param predicate A function that evaluates each value emitted by the source Observable.
    @return Observables tuple: one with values that passed the predicate, and another with values that did not pass the predicate.
 */
[@bs.module "rxjs"]
external partition: (Observable.t('a), ('a, int) => bool) => (Observable.t('a), Observable.t('a)) = "partition";

/**
  Returns an observable that mirrors the first source observable to emit an item.
  `race` returns an observable, that when subscribed to, subscribes to all source observables immediately.
  As soon as one of the source observables emits a value, the result unsubscribes from the other sources.
  The resulting observable will forward all notifications, including error and completion, from the "winning"
  source observable.

    @param observables sources used to race for which Observable emits first.
    @return an Observable that mirrors the output of the first Observable to emit an item.
 */
[@bs.module "rxjs"][@bs.variadic]
external race: array(Observable.t('a))  => Observable.t('a) = "race";

/**
  Creates an Observable that emits a sequence of numbers within a specified range.

    @param start=0 The value of the first integer in the sequence.
    @param count The number of sequential integers to generate.
    @param scheduler A Scheduler to use for scheduling the emissions of the notifications.
 */
[@bs.module "rxjs"]
external range: (
  ~start: int=?,
  ~count: int=?,
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => Observable.t(int) = "range";

/**
  Creates an Observable that emits no items to the Observer and immediately
  emits an error notification.

  Just emits 'error', and nothing else.

    @param error The particular Error to pass to the error notification.
    @param scheduler A Scheduler to use for scheduling the emission of the error notification.
    @return An error Observable: emits only the error notification using the given error argument.
 */
[@bs.module "rxjs"]
external throwError: (
  'err,
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => Observable.t('a) = "throwError";

/**
  Creates an Observable that starts emitting after an `dueTime` and
  emits ever increasing numbers after each `period` of time thereafter.

  Its like index/interval, but you can specify when should the emissions start.

    @param dueTime The initial delay time specified as a Date object or as an integer denoting milliseconds to wait before emitting the first value of `0`
    @param period The period of time between emissions of the subsequent numbers.
    @param The Scheduler to use for scheduling the emission of values, and providing a notion of "time"
    @return An Observable that emits a `0` after the `dueTime` and ever increasing numbers after each `period` of time thereafter.
 */
[@bs.module "rxjs"]
external timer: (
  ~dueTime: [@bs.unwrap][`Int(int) | `Date(Js.Date.t)]=?,
  ~period: int=?,
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => Observable.t(int) = "timer";

/**
  Creates an Observable that uses a resource which will be disposed at the same time as the Observable.

  Use it when you catch yourself cleaning up after an Observable.

    @param resourceFactory A function which creates any resource object that implements `unsubscribe` method.
    @param observableFactory A function which creates an Observable, that can use injected resource object.
    @return An Observable that behaves the same as Observable returned by `observableFactory`, but which - when completed, errored or unsubscribed - will also call `unsubscribe` on created resource object.
 */
[@bs.module "rxjs"]
external using: (
  ~resourceFactory: [@bs.unwrap][
    | `Unsubscribable(unit => Unsubscribable.t('o))
    | `Subscription(unit => Rx_Subscription.t)
    | `Unit(unit => unit)
  ],
  ~observableFactory: [@bs.unwrap][
    | `Unsubscribable(Unsubscribable.t('o) => Observable.t('a))
    | `Subscription(Rx_Subscription.t => Observable.t('a))
    | `Unit(unit => Observable.t('a))
  ]
) => Observable.t('a) = "using";

/**
  Combines multiple Observables to create an Observable whose values are calculated from the values, 
  in order, of each of its input Observables.

    @param observables Observables to zip
    @return Observable
 */
[@bs.module "rxjs"][@bs.variadic]
external zip: array(Observable.t('a)) => Observable.t(array('a)) = "zip";
[@bs.module "rxjs"]
external zip2: (Observable.t('b), Observable.t('c)) => Observable.t(('b, 'c)) = "zip";
[@bs.module "rxjs"]
external zip3: (Observable.t('b), Observable.t('c), Observable.t('d)) => Observable.t(('b, 'c, 'd)) = "zip";
[@bs.module "rxjs"]
external zip4: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e)) => Observable.t(('b, 'c, 'd, 'e)) = "zip";
[@bs.module "rxjs"]
external zip5: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f)) => Observable.t(('b, 'c, 'd, 'e, 'f)) = "zip";
[@bs.module "rxjs"]
external zip6: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g)) = "zip";
[@bs.module "rxjs"]
external zip7: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g), Observable.t('h)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g, 'h)) = "zip";
[@bs.module "rxjs"]
external zip8: (Observable.t('b), Observable.t('c), Observable.t('d), Observable.t('e), Observable.t('f), Observable.t('g), Observable.t('h), Observable.t('i)) => Observable.t(('b, 'c, 'd, 'e, 'f, 'g, 'h, 'i)) = "zip";

/**
  Converts from a common type to an observable where subscription and emissions
  are scheduled on the provided scheduler.

    @param input The observable, array, promise, iterable, etc you would like to schedule
    @param scheduler The scheduler to use to schedule the subscription and emissions from the returned observable.
    @return Observable
 */
[@bs.module "rxjs"]
external scheduled: (
  [@bs.unwrap][
    | `Array(array('a))
    | `Promise(Js.Promise.t('a))
    | `Observable(Observable.t('a))
  ],
  ~scheduler: Rx_Scheduler.t
) => Observable.t('a) = "scheduled";

/**
  Creates an Observable that emits no items to the Observer and immediately emits a complete notification.
 */
[@bs.module "rxjs"][@bs.val] external empty: Observable.t('a) = "EMPTY";

/**
  An Observable that emits no items to the Observer and never completes.
 */
[@bs.module "rxjs"][@bs.val] external never: Observable.t('a) = "NEVER";

/**
  An observable of animation frames
  
  Emits the the amount of time elapsed since subscription on each animation frame. Defaults to elapsed
  milliseconds. Does not end on its own.
  
  Every subscription will start a separate animation loop. Since animation frames are always scheduled
  by the browser to occur directly before a repaint, scheduling more than one animation frame synchronously
  should not be much different or have more overhead than looping over an array of events during
  a single animation frame. However, if for some reason the developer would like to ensure the
  execution of animation-related handlers are all executed during the same task by the engine,
  the `share` operator can be used.
 
  This is useful for setting up animations with RxJS.

    @param timestampProvider An object with a `now` method that provides a numeric timestamp
 */
[@bs.module "rxjs"]
external animationFrames: (~timestampProvider: Rx_Types.TimestampProvider.t('o)=?, unit) => Observable.t(int) = "animationFrames";