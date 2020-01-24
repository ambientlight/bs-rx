module Impl = (T: {type t;}) => {
  [@bs.send.pipe: T.t]
  external flush: Rx_AsyncAction.t(Js.t('a)) => unit = "flush";
};

type t;
include Rx_Scheduler.Impl({ type nonrec t = t });
include Rx_AsyncScheduler.Impl({ type nonrec t = t });
include Impl({ type nonrec t = t });