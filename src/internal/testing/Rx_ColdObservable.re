module Impl = (T: { type t('a); }) => {
  type logSubscribedFn = unit => int;
  type logUnsubscribedFn = int => unit;

  [@bs.get] external subscriptions: T.t('a) => array(Rx_SubscriptionLog.t) = "subscriptions";
  [@bs.set] external setSubscriptions: (T.t('a), array(Rx_SubscriptionLog.t)) => unit = "subscriptions";

  [@bs.get] external scheduler: T.t('a) => Rx_Scheduler.t = "scheduler";
  [@bs.set] external setScheduler: (T.t('a), Rx_Scheduler.t) => unit = "scheduler";

  [@bs.get] external logSubscribedFrame: T.t('a) => logSubscribedFn = "logSubscribedFrame";
  [@bs.set] external setLogSubscribedFrame: (T.t('a), logSubscribedFn) => unit = "logSubscribedFrame";
  [@bs.get] external logUnsubscribedFrame: T.t('a) => logUnsubscribedFn = "logUnsubscribedFrame";
  [@bs.set] external setLogUnsubscribedFrame: (T.t('a), logUnsubscribedFn) => unit = "logUnsubscribedFrame";

  [@bs.get] external messages: T.t('a) => array(Rx_TestMessage.t('a)) = "messages";
  [@bs.set] external setMessages: (T.t('a), array(Rx_TestMessage.t('a))) => unit = "messages";

  [@bs.send.pipe: T.t('a)] 
  external scheduleMessages: (~subscriber: Rx_Subscriber.t('b)) => unit = "scheduleMessages";
};

type t('a);

include Rx_Observable.Observable.Impl({ type nonrec t('a) = t('a) });
include Impl({ type nonrec t('a) = t('a) });