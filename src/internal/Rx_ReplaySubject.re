module Impl = (T: { type t('a); })  => {
  [@bs.module "rxjs"] [@bs.new]
  external create: (
    ~bufferSize: int=?,
    ~windowTime: int=?,
    ~scheduler: Rx_Scheduler.t=?,
    unit
  ) => T.t('a) = "ReplaySubject";
};

type t('a);

include Rx_Observable.Observable.Impl({ type nonrec t('a) = t('a) });
include Rx_Subject.Impl({ type nonrec t('a) = t('a) });
include Impl({ type nonrec t('a) = t('a) });