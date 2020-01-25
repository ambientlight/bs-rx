module Unsubscribable = {
  type t('a) = Js.t({
    ..
    unsubscribe: unit => unit
  } as 'a)
};

module TimestampProvider = {
  type t('a) = Js.t({
    ..
    now: unit => int
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

module EventListenerOptions = {
  [@bs.deriving abstract]
  type t = {
    [@bs.optional] capture: bool,
    [@bs.optional] passive: bool,
    [@bs.optional] once: bool
  }
}