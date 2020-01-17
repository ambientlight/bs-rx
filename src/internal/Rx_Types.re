module Unsubscribable = {
  type t('a) = Js.t({
    ..
    unsubscribe: unit => unit
  } as 'a)
};

module Observer = {
  [@bs.deriving abstract]
  type t('a, 'e) = {
    [@bs.optional] closed: bool,
    [@bs.optional] next: 'a => unit,
    [@bs.optional] error: 'e => unit,
    [@bs.optional] complete: unit => unit
  };
};