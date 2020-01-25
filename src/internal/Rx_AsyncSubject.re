module Impl = (T: { type t('a); })  => {
  
};

type t('a);

include Rx_Observable.Observable.Impl({ type nonrec t('a) = t('a) });
include Rx_Subject.Impl({ type nonrec t('a) = t('a) });
include Impl({ type nonrec t('a) = t('a) });