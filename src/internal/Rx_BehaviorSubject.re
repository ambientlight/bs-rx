module Impl = (T: { type t('a); })  => {
  [@bs.module "rxjs"] [@bs.new]
  external create: ('a) => T.t('a) = "BehaviorSubject";

  [@bs.send] external value: T.t('a) => 'a = "getValue";
};

type t('a);

include Rx_Observable.Observable.Impl({ type nonrec t('a) = t('a) });
include Rx_Subject.Impl({ type nonrec t('a) = t('a) });
include Impl({ type nonrec t('a) = t('a) });