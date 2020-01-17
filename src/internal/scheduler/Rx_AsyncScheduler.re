module Impl = (T: {type t;}) => {
  [@bs.module "rxjs/internal/scheduler/AsyncScheduler"][@bs.new]
  external create: (~schedulerAction: Rx_Action.t(Js.t('a))=?, ~now: [@bs.uncurry](unit => float)=?, unit) => T.t = "AsyncScheduler";

  [@bs.send.pipe: T.t]
  external flush: Rx_AsyncAction.t(Js.t('a)) => unit = "flush";
};

type t;
include Rx_Scheduler.Impl({ type nonrec t = t });