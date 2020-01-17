module Impl = (T: {type t('a);}) => {
  [@bs.send.pipe: T.t('a)]
  external execute: (~state: 'a, ~delay: int) => unit = "execute"
};

type t('a);
include Rx_Action.Impl({ type nonrec t('a) = t('a); });
include Impl({ type nonrec t('a) = t('a) })