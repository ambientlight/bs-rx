module Impl(T: { type t; }){
  type toBeObs;
  [@bs.send.pipe: toBeObs]
  external toBeObservable: (string, ~values: 'a=?, ~errorValue: 'e=?) => unit = "toBe";

  type toBeSub;
  [@bs.send.pipe: toBeSub]
  external toBeSubscriptions: array(string) => unit = "toBe";

  module RunHelpers = {
    type t;
    // FIXME: when returning ColdObservable instead and using asObservable(%identity, instead of obj.magic) bucklescript for some reason applies asObservable as method and thus fails on runtime
    [@bs.send.pipe: t] external cold: (string, ~values: 'v=?, ~error: 'e=?) => Rx_Observable.Observable.t('a) = "cold";
    // FIXME: when returning HotObservable instead and using asObservable(%identity, instead of obj.magic) bucklescript for some reason applies asObservable as method and thus fails on runtime
    [@bs.send.pipe: t] external hot: (string, ~values: 'v=?, ~error: 'e=?) => Rx_Observable.Observable.t('a) = "hot";
    [@bs.send.pipe: t] external flush: unit => unit = "flush";
    [@bs.send.pipe: t] external time: (string) => int = "time";
    [@bs.send.pipe: t] external expectObservable: (~observable: Rx_Observable.Observable.t('a), ~subscriptionMarbles: string=?) => toBeObs = "expectObservable";
    [@bs.send.pipe: t] external expectSubscriptions: (~subscriptions: array(Rx_SubscriptionLog.t)) => toBeSub = "expectSubscriptions";
  };

  [@bs.module "rxjs/testing"][@bs.new]
  external create: (~assertDeepEqual: [@bs.uncurry](('a, 'b) => 'c)) => T.t = "TestScheduler";
  
  /**
  The number of virtual time units each character in a marble diagram represents. If
  the test scheduler is being used in "run mode", via the `run` method, this is temporarly
  set to `1` for the duration of the `run` block, then set back to whatever value it was.
   */
  [@bs.module "rxjs/testing"][@bs.scope "TestScheduler"][@bs.val]
  external frameTimeFactor: int = "frameTimeFactor";

  [@bs.send.pipe: T.t]
  external createTime: (string) => int = "createTime";
  let time = createTime;

  /**
    @param marbles A diagram in the marble DSL. Letters map to keys in `values` if provided.
    @param values Values to use for the letters in `marbles`. If ommitted, the letters themselves are used.
    @param error The error to use for the `#` marble (if present).
   */
  [@bs.send.pipe: T.t]
  external createColdObservable: (string, ~values: 'v=?, ~error: 'e=?) => Rx_ColdObservable.t('a) = "createColdObservable";
  let cold = createColdObservable;

  /**
    @param marbles A diagram in the marble DSL. Letters map to keys in `values` if provided.
    @param values Values to use for the letters in `marbles`. If ommitted, the letters themselves are used.
    @param error The error to use for the `#` marble (if present).
   */
  [@bs.send.pipe: T.t]
  external createHotObservable: (string, ~values: 'v=?, ~error: 'e=?) => Rx_HotObservable.t('a) = "createHotObservable";
  let hot = createHotObservable;

  [@bs.send.pipe: T.t]
  external expectObservable: (Rx_Observable.Observable.t('a), ~subscriptionMarbles: string=?) => toBeObs = "expectObservable";

  [@bs.send.pipe: T.t]
  external expectSubscriptions: (array(Rx_SubscriptionLog.t)) => toBeSub = "expectSubscriptions";

  [@bs.send.pipe: T.t]
  external flush: unit = "flush";

  [@bs.module "rxjs"][@bs.scope "TestScheduler"][@bs.val]
  external parseMarblesAsSubscriptions: (string, ~runMode: bool=?, unit) => Rx_SubscriptionLog.t = "parseMarblesAsSubscriptions";

  [@bs.module "rxjs"][@bs.scope "TestScheduler"][@bs.val]
  external parseMarbles: (string, ~value: 'a=?, ~errorValue: 'error=?, ~materializeInnerObservables: bool=?, ~runMode: bool=?, unit) => array(Rx_TestMessage.t('a)) = "parseMarbles";

  [@bs.send.pipe: T.t]
  external run: ([@bs.uncurry](RunHelpers.t => 'a)) => 'a = "run";
}

type t;

include Rx_Scheduler.Impl({ type nonrec t = t });
include Rx_AsyncScheduler.Impl({ type nonrec t = t });
include Rx_VirtualTimeScheduler.Impl({ type nonrec t = t });
include Impl({ type nonrec t = t });