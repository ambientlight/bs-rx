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

module ThrottleConfig = {
  [@bs.deriving abstract]
  type t = {
    [@bs.optional] leading: bool,
    [@bs.optional] trailing: bool
  };
}

module TimeInterval = {
  [@bs.deriving abstract]
  type t('a) = {
    value: 'a,
    interval: float
  };
}

module Timestamp = {
  [@bs.deriving abstract]
  type t('a) = {
    value: 'a,
    timestamp: float
  };
};