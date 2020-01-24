module Impl = (T: { type t('a); }) => {
  [@bs.send.pipe: T.t('a)] external connect: Rx_Subscription.t = "connect";
  [@bs.send.pipe: T.t('a)] external refCount: Rx_Observable.Observable.t('a) = "refCount";
};


type t('a);

include Rx_Observable.Observable.Impl({ type nonrec t('a) = t('a) });
include Impl({ type nonrec t('a) = t('a) });