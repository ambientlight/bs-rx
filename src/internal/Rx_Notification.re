[@bs.deriving jsConverter]
type kind = [
  | [@bs.as "N"] `Next
  | [@bs.as "E"] `Error
  | [@bs.as "C"] `Complete
];

type t('a);

[@bs.module "rxjs"][@bs.new]
external _create: (~kind: string, ~value: 'a=?, ~error: 'e) => t('a) = "Notification";

/**
  creates a new notification object
 */
let create = (~kind, ~value, ~error) => _create(~kind=kindToJs(kind), ~value?, ~error);

/**
  Delivers to the given `observer` the value wrapped by this Notification

    @param observer
 */
[@bs.send.pipe: t('a)]
external observe: Rx_Types.Observer.t('t, 'a) => unit = "observe";

/**
  Given some {@link Observer} callbacks, deliver the value represented by the
  current Notification to the correctly corresponding callback.
  
    @param next An Observer `next` callback.
    @param error An Observer `error` callback.
    @param complete An Observer `complete` callback.
   */
[@bs.send.pipe: t('a)]
external do_: (~next: ([@bs.uncurry] 'a => unit), ~error: ([@bs.uncurry] 'e => unit)=?, ~complete: ([@bs.uncurry] unit => unit)=?) => unit = "do";

/**
  Takes an Observer or its individual callback functions, and calls `observe`
  or `do` methods accordingly.

    @param nextOrObserver An Observer or the `next` callback.
    @param error An Observer `error` callback.
    @param complete An Observer `complete` callback.
 */
[@bs.send.pipe: t('a)]
external accept: (
  [@bs.unwrap] [`Observer(Rx_Types.Observer.t('a, 'e)) | `Next('a => unit)],
  ~error: ([@bs.uncurry] 'e => unit)=?, 
  ~complete: ([@bs.uncurry] unit => unit)=?
) => unit = "accept";

/**
  Returns a simple Observable that just delivers the notification represented by this Notification instance
 */
[@bs.send.pipe: t('a)]
external toObservable: Rx_Observable.Observable.t('a) = "toObservable";

/**
  A shortcut to create a Notification instance of the type `next` from a given value.
    
    @param value The `next` value
 */
[@bs.module "rxjs"][@bs.scope "Notification"][@bs.val]
external createNext: 'a => t('a) = "createNext";

/**
  A shortcut to create a Notification instance of the type `error` from a given error.

    @param err The `error` error.
 */
[@bs.module "rxjs"][@bs.scope "Notification"][@bs.val]
external createError: 'err => t('a) = "createError";

/**
  A shortcut to create a Notification instance of the type `complete`.
 */
[@bs.module "rxjs"][@bs.scope "Notification"][@bs.val]
external createComplete: unit => t('a) = "createComplete";



