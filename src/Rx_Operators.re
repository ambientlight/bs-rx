open Rx_Observable.Observable;

type operator('a, 'b) = t('a) => t('b);

/**
  Ignores source values for a duration determined by another Observable, then
  emits the most recent value from the source Observable, then repeats this
  process.

    @param durationSelector A function that receives a value from the source Observable, for computing the silencing duration, returned as an Observable or a Promise.
    @return {Observable<T>} An Observable that performs rate-limiting of emissions from the source Observable.
 */
[@bs.module "rxjs/operators"]
external audit:
  (
    ~durationSelector: [@bs.unwrap] [
                         | `Subscribable('a => t('a))
                         | `Promise('a => Js.Promise.t('a))
                       ]
  ) =>
  operator('a, 'a) =
  "audit";

/**
  Ignores source values for `duration` milliseconds, then emits the most recent
  value from the source Observable, then repeats this process.

    @param duration Time to wait before emitting the most recent source value, measured in milliseconds or the time unit determined internally by the optional `scheduler`.
    @param scheduler=async The {@link SchedulerLike} to use for managing the timers that handle the rate-limiting behavior.
    @return An Observable that performs rate-limiting of emissions from the source Observable.
 */
[@bs.module "rxjs/operators"]
external auditTime:
  (~duration: float, ~scheduler: Rx_Scheduler.t=?, unit) => operator('a, 'a) =
  "auditTime";

/**
  Buffers the source Observable values until `closingNotifier` emits.

    @param {Observable<any>} closingNotifier An Observable that signals the buffer to be emitted on the output Observable.
    @return An Observable of buffers, which are arrays of values.
 */
[@bs.module "rxjs/operators"]
external buffer: t('a) => operator('b, array('b)) = "buffer";

/**
  Buffers the source Observable values until the size hits the maximum `bufferSize` given.

    @param bufferSize The maximum size of the buffer emitted.
    @param startBufferEvery Interval at which to start a new buffer. For example if `startBufferEvery` is `2`, then a new buffer will be started on every other value from the source. A new buffer is started at the beginning of the source by default.
    @return An Observable of arrays of buffered values
 */
[@bs.module "rxjs/operators"]
external bufferCount:
  (~bufferSize: int, ~startBufferEvery: int=?, unit) =>
  operator('a, array('a)) =
  "bufferCount";

/**
  Buffers the source Observable values for a specific time period.

    @param bufferTimeSpan The amount of time to fill each buffer array.
    @param bufferCreationInterval The interval at which to start new buffers.
    @param maxBufferSize The maximum buffer size.
    @param scheduler(async) The scheduler on which to schedule the intervals that determine buffer boundaries.
    @return An observable of arrays of buffered values.
 */
[@bs.module "rxjs/operators"]
external bufferTime:
  (
    ~timeSpan: int,
    ~bufferCreationInterval: int=?,
    ~maxBufferSize: int=?,
    ~scheduler: Rx_Scheduler.t=?,
    unit
  ) =>
  operator('a, array('a)) =
  "bufferTime";

/**
  Buffers the source Observable values starting from an emission from
  `openings` and ending when the output of `closingSelector` emits

    @param openings A Subscribable or Promise of notifications to start new buffers
    @param closingSelector A function that takes the value emitted by the `openings` observable and returns a Subscribable or Promise which, when it emits, signals that the associated buffer should be emitted and cleared
    @return An observable of arrays of buffered values
 */
[@bs.module "rxjs/operators"]
external bufferToggle:
  (
    ~opening: [@bs.unwrap] [
                | `Subscribable(t('o))
                | `Promise(Js.Promise.t('o))
              ],
    ~closing: [@bs.unwrap] [
                | `Subscribable('o => t('c))
                | `Promise('o => Js.Promise.t('c))
              ]
  ) =>
  operator('a, array('a)) =
  "bufferToggle";

/**
  Buffers the source Observable values, using a factory function of closing
  Observables to determine when to close, emit, and reset the buffer.

    @param closingSelector A function that takes no arguments and returns an Observable that signals buffer closure.
    @return An observable of arrays of buffered values.
 */
[@bs.module "rxjs/operators"]
external bufferWhen: (unit => t('b)) => operator('a, array('a)) =
  "bufferWhen";

/**
  Catches errors on the observable to be handled by returning a new observable or throwing an error.

    @param selector a function that takes as arguments `err`, which is the error, and `caught`, which is the source observable, in case you'd like to "retry" that observable by returning it again. Whatever observable is returned by the `selector` will be used to continue the observable chain.
    @return An observable that originates from either the source or the observable returned by the catch `selector` function.
 */
[@bs.module "rxjs/operators"]
external catchError:
  ([@bs.uncurry] (('err, t('a)) => t('b))) => operator('a, 'b) =
  "catchError";

/**
  Flattens an Observable-of-Observables by applying {@link combineLatest} when the Observable-of-Observables completes.
  `combineAll` takes an Observable of Observables, and collects all Observables from it. Once the outer Observable completes,
  it subscribes to all collected Observables and combines their values using the {@link combineLatest}</a> strategy, such that:

  Every time an inner Observable emits, the output Observable emits
  When the returned observable emits, it emits all of the latest values by:
  an array of all the most recent values is emitted by the output Observable.

    @see combineAllProject
 */
[@bs.module "rxjs/operators"]
external combineAll: unit => operator(t('a), array('a)) = "combineAll";

[@bs.module "rxjs/operators"]
external _combineAllProject: (. (array('a) => 'r)) => operator(t('a), 'r) =
  "combineAll";

type projectFn('a, 'r) = array('a) => 'r;

/**
  Flattens an Observable-of-Observables by applying {@link combineLatest} when the Observable-of-Observables completes.
  `combineAll` takes an Observable of Observables, and collects all Observables from it. Once the outer Observable completes,
  it subscribes to all collected Observables and combines their values using the {@link combineLatest}</a> strategy, such that:

  Every time an inner Observable emits, the output Observable emits
  When the returned observable emits, it emits all of the latest values by:
  it is called with each recent value from each inner Observable in whatever order they arrived, and the result of the `project` function is what is emitted by the output Observable.

    @param project function to map the most recent values from each inner Observable into a new result.
    @see combineAll
 */
let combineAllProject: projectFn('a, 'r) => operator(t('a), 'r) =
  (project: projectFn('a, 'r)) => {
    let _makeVariadic: (. projectFn('a, 'r)) => projectFn('a, 'r) = [%bs.raw
      {|function(fn) {
    return function(){
      return fn([].slice.call(arguments))
    }
  }|}
    ];

    let _project = _makeVariadic(. project);
    _combineAllProject(. _project);
  };

/**
  Converts a higher-order Observable into a first-order Observable
  by concatenating the inner Observables in order.

   Joins every Observable emitted by the source (a higher-order Observable), in a serial fashion.
   It subscribes to each inner Observable only after the previous inner Observable has completed,
   and merges all of their values into the returned observable.

   @return An Observable emitting values from all the inner Observables concatenated.
 */
[@bs.module "rxjs/operators"]
external concatAll: unit => operator(t('a), 'a) = "concatAll";

/**
  Projects each source value to an Observable which is merged in the output Observable,
  in a serialized fashion waiting for each one to complete before merging the next.

  Returns an Observable that emits items based on applying a function that you
  supply to each item emitted by the source Observable, where that function
  returns an (so-called "inner") Observable. Each new inner Observable is
  concatenated with the previous inner Observable.

    @param project A function that, when applied to an item emitted by the source Observable, returns an Observable
    @return An Observable that emits the result of applying the projection function (and the optional deprecated `resultSelector`) to each item emitted by the source Observable and taking values from each projected inner Observable sequentially.
 */
[@bs.module "rxjs/operators"]
external concatMap: ([@bs.uncurry] (('a, int) => t('b))) => operator('a, 'b) =
  "concatMap";

/**
  Projects each source value to the same Observable which is merged multiple
  times in a serialized fashion on the output Observable.
  convinience for concatMap(() => innerObservable)

  Maps each source value to the given Observable `innerObservable` regardless
  of the source value, and then flattens those resulting Observables into one
  single Observable, which is the output Observable. Each new `innerObservable`
  instance emitted on the output Observable is concatenated with the previous
  `innerObservable` instance.

    @param innerObservable An Observable to replace each value from the source Observable.
    @return An observable of values merged together by joining the passed observable with itself, one after the other, for each value emitted from the source.
 */
[@bs.module "rxjs/operators"]
external concatMapTo: t('a) => operator('b, 'a) = "concatMapTo";

/**
  Emits all of the values from the source observable, then, once it completes, subscribes
  to each observable source provided, one at a time, emitting all of their values, and not subscribing
  to the next one until it completes.

    @param otherSources Other observable sources to subscribe to, in sequence, after the original source is complete.
    @return An Observable that emits the result of applying the projection function (and the optional deprecated `resultSelector`) to each item emitted by the source Observable and taking values from each projected inner Observable sequentially.
 */
/*
 [@bs.module "rxjs/operators"][@bs.variadic]
 external concatWith: array(t('a)) => operator('a, array('a)) = "concatWith";
 */
[@bs.module "rxjs/operators"]
//FIXME: fails when predicated is labelled optional
external _count:
  (
    ~predicate: [@bs.uncurry] (
                  (~value: 'a, ~index: int, ~source: t('a)) => bool
                )
  ) =>
  operator('a, int) =
  "count";

/**
  Counts the number of emissions on the source and emits that number when the
  source completes.

  `count` transforms an Observable that emits values into an Observable that
  emits a single value that represents the number of values emitted by the
  source Observable. If the source Observable terminates with an error, `count`
  will pass this error notification along without emitting a value first. If
  the source Observable does not terminate at all, `count` will neither emit
  a value nor terminate. This operator takes an optional `predicate` function
  as argument, in which case the output emission will represent the number of
  source values that matched `true` with the `predicate`.

    @param predicate boolean function to select what values are to be counted
    @return An Observable of one number that represents the count as described above.
 */
let count =
    (
      ~predicate=(~value as _value, ~index as _idx, ~source as _src) => true,
      (),
    ) =>
  _count(~predicate);

/**
  Emits a notification from the source Observable only after a particular time span
  determined by another Observable has passed without another source emission.

    @param durationSelector A function that receives a value from the source Observable, for computing the timeout duration for each source value, returned as an Observable or a Promise.
    @return {Observable} An Observable that delays the emissions of the source Observable by the specified duration Observable returned by `durationSelector`, and may drop some values if they occur too frequently.
 */
[@bs.module "rxjs/operators"]
external debounce:
  (
    ~durationSelector: [@bs.unwrap] [
                         | `Subscribable('a => t('a))
                         | `Promise('a => Js.Promise.t('a))
                       ]
  ) =>
  operator('a, 'a) =
  "debounce";

/**
  Emits a notification from the source Observable only after a particular time span
  has passed without another source emission.

    @param The timeout duration in milliseconds (or the time unit determined internally by the optional `scheduler`) for the window of time required to wait for emission silence before emitting the most recent source value.
    @return An Observable that delays the emissions of the source Observable by the specified `dueTime`, and may drop some values if they occur too frequently.
 */
[@bs.module "rxjs/operators"]
external debounceTime:
  (~dueTime: float, ~scheduler: Rx_Scheduler.t=?, unit) => operator('a, 'a) =
  "debounceTime";

/**
  Emits a given value if the source Observable completes without emitting any
  `next` value, otherwise mirrors the source Observable.

    @param The default value used if the source Observable is empty.
    @return An Observable that emits either the specified `defaultValue` if the source Observable emits no items, or the values emitted by the source Observable
 */
[@bs.module "rxjs/operators"]
external defaultIfEmpty: 'a => operator('a, 'a) = "defaultIfEmpty";

/**
  Delays the emission of items from the source Observable by a given timeout or until a given Date.

    @param delay duration in milliseconds (a `number`) or a `Date` until which the emission of the source items is delayed
    @param scheduler The {@link SchedulerLike} to use for managing the timers that handle the time-shift for each item
    @return The {@link SchedulerLike} to use for managing the timers that handle the time-shift for each item
 */
[@bs.module "rxjs/operators"]
external delay:
  (
    [@bs.unwrap] [ | `Int(int) | `Date(Js.Date.t)],
    ~scheduler: Rx_Scheduler.t=?,
    unit
  ) =>
  operator('a, 'a) =
  "delay";

/**
  Delays the emission of items from the source Observable by a given time span
  determined by the emissions of another Observable

    @param delayDurationSelector A function that returns an Observable for each value emitted by the source Observable, which is then used to delay the emission of that item on the output Observable until the Observable returned from this function emits a value
    @param subscriptionDelay An Observable that triggers the subscription to the source Observable once it emits any value
    @return An Observable that delays the emissions of the source Observable by an amount of time specified by the Observable returned by `delayDurationSelector`
 */
[@bs.module "rxjs/operators"]
external delayWhen:
  (
    ~delayDurationSelector: [@bs.uncurry] (('a, int) => t('b)),
    ~subscriptionDelay: t('c)=?,
    unit
  ) =>
  operator('a, 'a) =
  "delayWhen";

/**
  Converts an Observable of Notification objects into the emissions that they represent
 */
[@bs.module "rxjs/operators"]
external dematerialize: unit => operator(Rx_Notification.t('a), 'a) =
  "dematerialize";

[@bs.module "rxjs/operators"]
external _distinct:
  (
    ~keySelector: [@bs.uncurry] ('a => 'b),
    ~flushes: Rx_Observable.Observable.t('f)=?,
    unit
  ) =>
  operator('a, 'a) =
  "distinct";

/**
  Returns an Observable that emits all items emitted by the source Observable
  that are distinct by comparison from previous items

  If a keySelector function is provided, then it will project each value from the source observable
  into a new value that it will check for equality with previously projected values. If a keySelector function is not provided,
  it will use each value from the source observable directly with an equality check against previous values

    @param keySelector function to select which value you want to check as distinct
    @param flushes Optional Observable for flushing the internal HashSet of the operator
    @return An Observable that emits items from the source Observable with distinct values
 */
let distinct = (~keySelector=v => v, ~flushes=?, ()) =>
  _distinct(~keySelector, ~flushes?, ());

[@bs.module "rxjs/operators"]
external _distinctUntilChanged:
  (~compare: [@bs.uncurry] (('a, 'a) => bool)) => operator('a, 'a) =
  "distinctUntilChanged";

/**
  Returns an Observable that emits all items emitted by the source Observable that are distinct by comparison from the previous item.
  If a comparator function is provided, then it will be called for each item to test for whether or not that value should be emitted.
  If a comparator function is not provided, an equality check(shallow) is used by default.

    @param compare Optional comparison function called to test if an item is distinct from the previous item in the source.
    @return An Observable that emits items from the source Observable with distinct values.
 */
let distinctUntilChanged = (~compare=(lhs, rhs) => lhs === rhs, ()) =>
  _distinctUntilChanged(~compare);

/**
  Returns an Observable that emits all items emitted by the source Observable that are distinct by comparison from the previous item
  using a property accessed by using the key provided to check if the two items are distinct.

  If a comparator function is provided, then it will be called for each item to test for whether or not that value should be emitted.
  If a comparator function is not provided, an equality check(shallow) is used by default.
 */
[@bs.module "rxjs/operators"]
external _distinctUntilKeyChanged:
  (~key: string, ~compare: [@bs.uncurry] (('a, 'a) => bool)) =>
  operator('a, 'a) =
  "distinctUntilKeyChanged";

let distinctUntilKeyChanged = (~key, ~compare=(lhs, rhs) => lhs === rhs, ()) =>
  _distinctUntilKeyChanged(~key, ~compare);

/**
  Emits the single value at the specified `index` in a sequence of emissions
  from the source Observable

    @param index Is the number `i` for the i-th source emission that has happened since the subscription, starting from the number `0`.
    @param default The default value returned for missing indices.
    @return An Observable that emits a single item, if it is found.
 */
[@bs.module "rxjs/operators"]
external elementAt: (int, ~default: 'a=?, unit) => operator('a, 'a) =
  "elementAt";

/**
  Returns an Observable that emits the items you specify as arguments after it finishes emitting
  items emitted by the source Observable.

    @param items Items you want the modified Observable to emit last.
    @return An Observable that emits the items emitted by the source Observable and then emits the items in the specified Iterable.
 */
[@bs.module "rxjs/operators"] [@bs.variadic]
external endWith: array('a) => operator('a, 'a) = "endWith";

/* OPERATOR: every */
[@bs.module "rxjs/operators"]
external every: ([@bs.uncurry] (('a, int) => bool)) => operator('a, 'a) =
  "every";

/* OPERATOR: exhaust */
/* No support for exhausting t of array or promises */
[@bs.module "rxjs/operators"]
external exhaust: unit => operator(t('a), 'a) = "exhaust";

/* OPERATOR: exhaustMap */
[@bs.module "rxjs/operators"]
external exhaustMap: ([@bs.uncurry] (('a, int) => t('b))) => operator('a, 'b) =
  "exhaustMap";

[@bs.module "rxjs/operators"]
external exhaustMapArray:
  ([@bs.uncurry] (('a, int) => array('b))) => operator('a, 'b) =
  "exhaustMap";

[@bs.module "rxjs/operators"]
external exhaustMapPromise:
  ([@bs.uncurry] (('a, int) => Js.Promise.t('b))) => operator('a, 'b) =
  "exhaustMap";

/* OPERATOR: filter */
[@bs.module "rxjs/operators"]
external filter: ([@bs.uncurry] ('a => bool)) => operator('a, 'a) = "filter";

[@bs.module "rxjs/operators"]
external filteri: ([@bs.uncurry] (('a, int) => bool)) => operator('a, 'a) =
  "filter";

/* OPERATOR: findIndex */
[@bs.module "rxjs/operators"]
external findIndex:
  ([@bs.uncurry] (('a, int, t('a)) => bool)) => operator('a, int) =
  "findIndex";

/* OPERATOR: isEmpty */
[@bs.module "rxjs/operators"]
external isEmpty: unit => operator('a, bool) = "isEmpty";

/* OPERATOR: map */
[@bs.module "rxjs/operators"]
external map: ([@bs.uncurry] ('a => 'b)) => operator('a, 'b) = "map";

[@bs.module "rxjs/operators"]
external mapi: ([@bs.uncurry] (('a, int) => 'b)) => operator('a, 'b) = "map";

/* OPERATOR: max */
[@bs.module "rxjs/operators"] external max: unit => operator('a, 'a) = "max";

[@bs.module "rxjs/operators"]
external maxCustom: ([@bs.uncurry] (('a, 'a) => bool)) => operator('a, 'a) =
  "max";

/* OPERATOR: mapTo */
[@bs.module "rxjs/operators"]
external mapTo: 'b => operator('a, 'b) = "mapTo";

/* OPERATOR: mergeAll */
[@bs.module "rxjs/operators"]
external mergeAll: (~concurrent: int=?, unit) => operator(t('a), 'a) =
  "mergeAll";

/* OPERATOR: mergeMap */
[@bs.module "rxjs/operators"]
external mergeMap: ([@bs.uncurry] ('a => t('b))) => operator('a, 'b) =
  "mergeMap";

[@bs.module "rxjs/operators"]
external mergeMapArray: ([@bs.uncurry] ('a => array('b))) => operator('a, 'b) =
  "mergeMap";

[@bs.module "rxjs/operators"]
external mergeMapPromise:
  ([@bs.uncurry] ('a => Js.Promise.t('b))) => operator('a, 'b) =
  "mergeMap";

[@bs.module "rxjs/operators"]
external mergeMapTo: (t('b), ~concurrent: int=?, unit) => operator('a, 'b) =
  "mergeMapTo";

[@bs.module "rxjs/operators"]
external mergeMapToPromise:
  (Js.Promise.t('b), ~concurrent: int=?, unit) => operator('a, 'b) =
  "mergeMapTo";

/* OPERATOR: min */
[@bs.module "rxjs/operators"] external min: unit => operator('a, 'a) = "min";

[@bs.module "rxjs/operators"]
external minCustom: ([@bs.uncurry] (('a, 'a) => bool)) => operator('a, 'a) =
  "min";

/* OPERATOR: pairwise */
[@bs.module "rxjs/operators"]
external pairwise: unit => operator('a, ('a, 'a)) = "pairwise";

/* OPERATOR: reduce */
[@bs.module "rxjs/operators"]
external reduce:
  ([@bs.uncurry] (('acc, 'a, int) => 'acc), 'acc) => operator('a, 'acc) =
  "reduce";

/* OPERATOR: refCount */
[@bs.module "rxjs/operators"]
external refCount: unit => operator('a, 'a) = "refCount";

/* OPERATOR: repeat */
[@bs.module "rxjs/operators"]
external _repeat: (~count: int=?, unit) => operator('a, 'a) = "repeat";

let repeat = (~count=(-1), stream) => _repeat(~count, (), stream);

/* OPERATOR: retry */
[@bs.module "rxjs/operators"]
external retry: (~count: int=?, unit) => operator('a, 'a) = "retry";

/* OPERATOR: retryWhen */
[@bs.module "rxjs/operators"]
external retryWhen: (t(Js.Exn.t) => t('b)) => operator('a, 'a) =
  "retryWhen";

/* OPERATOR: sample */
[@bs.module "rxjs/operators"]
external sample: t('any) => operator('a, 'a) = "sample";

/* OPERATOR: sampleTime */
//TODO:
// [@bs.module "rxjs/operators"]
// external sampleTime: (int, ~scheduler: scheduler=?, unit) => operator('a, 'a) =
//   "";

/* OPERATOR: scan */
[@bs.module "rxjs/operators"]
external scan:
  ([@bs.uncurry] (('acc, 'a, int) => 'acc), 'acc) => operator('a, 'acc) =
  "scan";

/* OPERATOR: share */
[@bs.module "rxjs/operators"]
external share: unit => operator('a, 'a) = "share";

/* OPERATOR: skip */
[@bs.module "rxjs/operators"] external skip: int => operator('a, 'a) = "skip";

/* OPERATOR: skipLast */
[@bs.module "rxjs/operators"]
external skipLast: int => operator('a, 'a) = "skipLast";

/* OPERATOR: skipUntil */
[@bs.module "rxjs/operators"]
external skipUntil: t('any) => operator('a, 'a) = "skipUntil";

/* OPERATOR: skipWhile */
[@bs.module "rxjs/operators"]
external skipWhile: ([@bs.uncurry] (('a, int) => bool)) => operator('a, 'a) =
  "skipWhile";

/* OPERATOR: switchMap */
[@bs.module "rxjs/operators"]
external switchMap: ([@bs.uncurry] ('a => t('b))) => operator('a, 'b) =
  "switchMap";

[@bs.module "rxjs/operators"]
external switchMapArray:
  ([@bs.uncurry] ('a => array('b))) => operator('a, 'b) =
  "switchMap";

[@bs.module "rxjs/operators"]
external switchMapPromise:
  ([@bs.uncurry] ('a => Js.promise('b, 'e))) => operator('a, 'b) =
  "switchMap";

/* OPERATOR: take */
[@bs.module "rxjs/operators"] external take: int => operator('a, 'a) = "take";

/* OPERATOR: takeLast */
[@bs.module "rxjs/operators"]
external takeLast: int => operator('a, 'a) = "takeLast";

/* OPERATOR: takeUntil */
[@bs.module "rxjs/operators"]
external takeUntil: t('any) => operator('a, 'a) = "takeUntil";

/* OPERATOR: takeWhite */
[@bs.module "rxjs/operators"]
external takeWhile:
  ([@bs.uncurry] (('a, int) => bool), ~inclusive: bool=?, unit) =>
  operator('a, 'a) =
  "takeWhile";

/* OPERATOR: tap */
[@bs.module "rxjs/operators"]
external tap: ('a => unit) => operator('a, 'a) = "tap";

[@bs.module "rxjs/operators"]
external tapObserver: Rx_Types.Observer.t('a, 'e) => operator('a, 'a) =
  "tap";

/* OPERATOR: timeout */
[@bs.module "rxjs/operators"]
external timeout: int => operator('a, 'a) = "timeout";

[@bs.module "rxjs/operators"]
external timeoutByDate: Js.Date.t => operator('a, 'a) = "timeout";

/* OPERATOR: withLatestFrom */
[@bs.module "rxjs/operators"]
external withLatestFrom2: t('b) => operator('a, ('a, 'b)) =
  "withLatestFrom";

[@bs.module "rxjs/operators"]
external withLatestFrom3: (t('b), t('c)) => operator('a, ('a, 'b, 'c)) =
  "withLatestFrom";

[@bs.module "rxjs/operators"]
external withLatestFrom4:
  (t('b), t('c), t('d)) => operator('a, ('a, 'b, 'c, 'd)) =
  "withLatestFrom";

[@bs.module "rxjs/operators"]
external withLatestFrom5:
  (t('b), t('c), t('d), t('e)) => operator('a, ('a, 'b, 'c, 'd, 'e)) =
  "withLatestFrom";

[@bs.module "rxjs/operators"]
external withLatestFrom6:
  (t('b), t('c), t('d), t('e), t('f)) =>
  operator('a, ('a, 'b, 'c, 'd, 'e, 'f)) =
  "withLatestFrom";