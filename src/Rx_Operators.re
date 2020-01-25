open Rx_Observable.Observable;

type unaryFunction('a, 'b) = 'a => 'b;
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
    [@bs.unwrap] [
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
  (~duration: int, ~scheduler: Rx_Scheduler.t=?, unit) => operator('a, 'a) =
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

type projectFn('a, 'r) = 'a => 'r;

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
let combineAllProject: projectFn(array('a), 'r) => operator(t('a), 'r) = (project: projectFn(array('a), 'r)) => {
    let _makeVariadic: (. projectFn(array('a), 'r)) => projectFn(array('a), 'r) = [%bs.raw
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
external concatMap: ([@bs.unwrap] [
  | `Observable(('a, int) => t('b))
  | `Promise(('a, int) => Js.Promise.t('b))
  | `Array(('a, int) => array('b))
]) => operator('a, 'b) = "concatMap";

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
external concatMapTo: ([@bs.unwrap] [
  | `Observable(t('a))
  | `Promise(Js.Promise.t('a))
  | `Array( array('a))
]) => operator('b, 'a) = "concatMapTo";

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

/**
  Returns an Observable that emits whether or not every item of the source satisfies the condition specified.

    @param predicate A function for determining if an item meets a specified condition
    @return An Observable of booleans that determines if all items of the source Observable meet the condition specified
 */
[@bs.module "rxjs/operators"]
external every: ([@bs.uncurry] (('a, int, t('a)) => bool)) => operator('a, 'a) = "every";

/**
  Converts a higher-order Observable into a first-order Observable by dropping
  inner Observables while the previous inner Observable has not yet completed.

  `exhaust` ignores every new inner Observable if the previous Observable has
  not yet completed. Once that one completes, it will accept and flatten the
  next inner Observable and repeat this process.

    @return An Observable that takes a source of Observables and propagates the first observable exclusively until it completes before subscribing to the next.
 */
[@bs.module "rxjs/operators"]
external exhaust: unit => operator(t('a), 'a) = "exhaust";

/**
  Projects each source value to an Observable which is merged in the output
  Observable only if the previous projected Observable has completed.

    @param project A function that, when applied to an item emitted by the source Observable, returns an Observable or Promise or iterable.
    @return An Observable containing projected Observables of each item of the source, ignoring projected Observables that start before their preceding Observable has completed.
 */
[@bs.module "rxjs/operators"]
external exhaustMap: (
  [@bs.unwrap] [
    | `Observable(('a, int) => t('b))
    | `Promise(('a, int) => Js.Promise.t('b))
    | `Array(('a, int) => array('b))
  ]
) => operator('a, 'b) = "exhaustMap";

/**
  Recursively projects each source value to an Observable which is merged in the output Observable.

    @param project A function that, when applied to an item emitted by the source or the output Observable, returns an Observable.
    @param cuncurrent Maximum number of input Observables being subscribed to concurrently.
    @param scheduler The `Scheduler` to use for subscribing to each projected inner Observable.
    @return An Observable that emits the source values and also result of applying the projection function to each value emitted on the output Observable and merging the results of the Observables obtained from this transformation.
 */
[@bs.module "rxjs/operators"]
external expand: (
  [@bs.unwrap] [
    | `Observable(('a, int) => t('b))
    | `Promise(('a, int) => Js.Promise.t('b))
    | `Array(('a, int) => array('b))
  ],
  ~concurrent: int=?,
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => operator('a, 'b) = "expand";

/**
  Filter items emitted by the source Observable by only emitting those that satisfy a specified predicate

    @param predicate A function that evaluates each value emitted by the source Observable.
 */
[@bs.module "rxjs/operators"]
external filter: ([@bs.uncurry] (('a, int) => bool)) => operator('a, 'a) = "filter";

/**
  Returns an Observable that mirrors the source Observable, but will call a specified function when
  the source terminates on complete or error.

    @param callback Function to be called when source terminates.
    @return An Observable that mirrors the source, but will call the specified function on termination.
 */
[@bs.module "rxjs/operators"]
external finalize: ([@bs.uncurry] (unit => unit)) => operator('a, 'a) = "finalize";

/**
  Emits only the first value emitted by the source Observable that meets some condition.

    @param predicate A function called with each item to test for condition matching.
    @return An Observable of the first item that matches the condition
 */
[@bs.module "rxjs/operators"]
external find: ([@bs.uncurry] (('a, int, t('a)) => bool)) => operator('a, 'a) = "find";

/**
  Emits only the index of the first value emitted by the source Observable that meets some condition.

    @param predicate A function called with each item to test for condition matching
    @return An Observable of the index of the first item that matches the condition.
 */
[@bs.module "rxjs/operators"]
external findIndex: ([@bs.uncurry] (('a, int, t('a)) => bool)) => operator('a, int) = "findIndex";

[@bs.module "rxjs/operators"]
external _first: (
  ~predicate: [@bs.uncurry](('a, int, t('a)) => bool),
  ~defaultValue: 'b=?, 
  unit
) => operator('a, 'b) = "first";

/**
  Emits only the first value (or the first value that meets some condition) emitted by the source Observable.

    @param predicate An optional function called with each item to test for condition matching.
    @param The default value emitted in case no valid value was found on the source.
    @return An Observable of the first item that matches the condition
 */
let first = (~predicate=(_x, _idx, _src) => true, ~defaultValue=?, ()) => _first(~predicate, ~defaultValue?, ());

/**
  Groups the items emitted by an Observable according to a specified criterion,
  and emits these grouped items as `GroupedObservables`, one per group.

  When the Observable emits an item, a key is computed for this item with the keySelector function.
  If a GroupedObservable for this key exists, this roupedObservable emits. Elsewhere, a new GroupedObservable for this key is created and emits.

    @param keySelector A function that extracts the key for each item.
    @param elementSelector A function that extracts the return element for each item.
    @param durationSelectorA function that returns an Observable to determine how long each group should exist.
    @return An Observable that emits GroupedObservables, each of which corresponds to a unique key value and each of which emits those items from the source Observable that share that key
 */
[@bs.module "rxjs/operators"]
external groupBy: (
  ('a) => string,
  //FIXME: groupBy won't like the next set as undefined when applied, those won't work with default values, so seperate functions are needed
  //~elementSelector: [@bs.uncurry]('a => 'b)=?,
  //~durationSelector: [@bs.uncurry](t('b) => t('c))=?,
  //~subjectSelector: [@bs.uncurry](unit => Rx_Subject.t('b))=?,
  //unit
) => operator('a, t('b)) = "groupBy";

/**
  Ignores all items emitted by the source Observable and only passes calls of `complete` or `error`.

    @return An empty Observable that only calls `complete` or `error`, based on which one is called by the source Observable.
 */
[@bs.module "rxjs/operators"]
external ignoreElements: unit => operator('a, 'a) = "ignoreElements";

/**
   Emits `false` if the input Observable emits any values, or emits `true` if the input Observable completes 
   without emitting any values

    @return An Observable of a boolean value indicating whether observable was empty or not.
 */
[@bs.module "rxjs/operators"]
external isEmpty: unit => operator('a, bool) = "isEmpty";

[@bs.module "rxjs/operators"]
external _last: (
  ~predicate: [@bs.uncurry](('a, int, t('a)) => bool),
  ~defaultValue: 'b=?, 
  unit
) => operator('a, 'b) = "last";

/**
  Emits only the last value (or the last value that meets some condition) emitted by the source Observable.

    @param predicate An optional function called with each item to test for condition matching.
    @param The default value emitted in case no valid value was found on the source.
    @return An Observable of the last item that matches the condition
 */
let last = (~predicate=(_x, _idx, _src) => true, ~defaultValue=?, ()) => _last(~predicate, ~defaultValue?, ());

/**
  Applies a given `project` function to each value emitted by the source Observable
  and emits the resulting values as an Observable.

    @param project The function to apply to each `value` emitted by the source Observable. The `index` parameter is the number `i` for the i-th emission that has happened since the subscription, starting from the number `0`.
    @return An Observable that emits the values from the source Observable transformed by the given `project` function.
 */
[@bs.module "rxjs/operators"]
external map: ([@bs.uncurry] (('a, int) => 'b)) => operator('a, 'b) = "map";

/**
  Emits the given constant value on the output Observable every time the source Observable emits a value.
  
    @param value The value to map each source value to.
    @return An Observable that emits the given value every time the source Observable emits something.
 */
[@bs.module "rxjs/operators"]
external mapTo: 'b => operator('a, 'b) = "mapTo";

/**
  Represents all of the notifications from the source Observable as `next`
  emissions marked with their original types within Notification objects.

    @return An Observable that emits objects that wrap the original emissions from the source Observable with metadata.
 */
[@bs.module "rxjs/operators"]
external materialize: unit => operator('a, Rx_Notification.t('a)) = "materialize";

/**
  The Max operator operates on an Observable that emits numbers (or items that can be compared with a provided function)and when source Observable completes it emits a single item: the item with the largest value.

    @return An Observable that emits item with the largest value.
 */
[@bs.module "rxjs/operators"] external max: unit => operator('a, 'a) = "max";

/**
  The Max operator operates on an Observable that emits numbers (or items that can be compared with a provided function)and when source Observable completes it emits a single item: the item with the largest value.

    @param Optional comparer function that it will use instead of its default to compare the value of two items.
    @return An Observable that emits item with the largest value.
 */
[@bs.module "rxjs/operators"]
external maxWithComparer: ([@bs.uncurry] (('a, 'a) => float)) => operator('a, 'a) = "max";

/**
  Converts a higher-order Observable into a first-order Observable which
  concurrently delivers all values that are emitted on the inner Observables.

    @param Maximum number of inner Observables being subscribed to concurrently.
    @return {Observable} An Observable that emits values coming from all the inner Observables emitted by the source Observable.
 */
[@bs.module "rxjs/operators"]
external mergeAll: (~concurrent: int=?, unit) => operator(t('a), 'a) = "mergeAll";

/**
  Projects each source value to an Observable which is merged in the output Observable.

    @param project A function that, when applied to an item emitted by the source Observable, returns an Observable.
    @param Maximum number of input Observables being subscribed to concurrently.
    @return An Observable that emits the result of applying the projection function (and the optional deprecated `resultSelector`) to each item emitted by the source Observable and merging the results of the Observables obtained from this transformation.
 */
[@bs.module "rxjs/operators"]
external mergeMap: (
  [@bs.unwrap] [
    | `Observable(('a, int) => t('b))
    | `Promise(('a, int) => Js.Promise.t('b))
    | `Array(('a, int) => array('b))
  ],
  ~concurrent: int=?,
  unit
) => operator('a, 'b) = "mergeMap";

/**
  Projects each source value to the same Observable which is merged multiple times in the output Observable.

    @param innerObservable An Observable to replace each value from the source Observable.
    @param concurrent Maximum number of input Observables being subscribed to concurrently.
    @return An Observable that emits items from the given `innerObservable`
 */
[@bs.module "rxjs/operators"]
external mergeMapTo: (
  [@bs.unwrap] [
    | `Observable(t('b))
    | `Promise(Js.Promise.t('b))
    | `Array( array('b))
  ],
  ~concurrent: int=?, 
  unit
) => operator('a, 'b) = "mergeMapTo";

/**
  Applies an accumulator function over the source Observable where the
  accumulator function itself returns an Observable, then each intermediate
  Observable returned is merged into the output Observable.

    @param accumulator The accumulator function called on each source value.
    @param seed The initial accumulation value.
    @param concurrent Maximum number of input Observables being subscribed to concurrently
    @return An observable of the accumulated values.
 */
[@bs.module "rxjs/operators"]
external mergeScan: (
  [@bs.unwrap] [
    | `Observable(('b, 'a, int) => t('b))
    | `Promise(('b, 'a, int) => Js.Promise.t('b))
    | `Array(('b, 'a, int) => array('b))
  ],
  ~seed: 'b,
  ~concurrent: int=?,
  unit
) => operator('a, 'b) = "mergeScan";

/**
  The Min operator operates on an Observable that emits numbers (or items that can be compared with a provided function)and when source Observable completes it emits a single item: the item with the smallest value.

    @return An Observable that emits item with the smallest value.
 */
[@bs.module "rxjs/operators"] external min: unit => operator('a, 'a) = "min";

/**
  The Min operator operates on an Observable that emits numbers (or items that can be compared with a provided function)and when source Observable completes it emits a single item: the item with the largest value.

    @param Optional comparer function that it will use instead of its default to compare the value of two items.
    @return An Observable that emits item with the smallest value.
 */
[@bs.module "rxjs/operators"]
external minWithComparer: ([@bs.uncurry] (('a, 'a) => float)) => operator('a, 'a) = "min";

/**
  Returns an Observable that emits the results of invoking a specified selector on items
  emitted by a ConnectableObservable that shares a single subscription to the underlying stream.

    @param subjectOrSubjectFactory Factory function to create an intermediate subject through which the source sequence's elements will be multicasted to the selector function or Subject to push source elements into.
    @param selector Optional selector function that can use the multicasted source stream as many times as needed, without causing multiple subscriptions to the source stream Subscribers to the given source will receive all notifications of the source from the time of the subscription forward.
    @return An Observable that emits the results of invoking the selector on the items emitted by a `ConnectableObservable` that shares a single subscription to the underlying stream.
 */
[@bs.module "rxjs/operators"]
external multicast: (
  [@bs.unwrap] [
    | `Subject(Rx_Subject.t('a))
    | `SubjectFactory(unit => Rx_Subject.t('a))
  ],
  ~selector: (t('a) => t('b))=?,
  unit
) => unaryFunction(t('a), Rx_ConnectableObservable.t('a)) = "multicast";

/**
  Re-emits all notifications from source Observable with specified scheduler.
  Ensure a specific scheduler is used, from outside of an Observable.

  `observeOn` is an operator that accepts a scheduler as a first parameter, which will be used to reschedule
  notifications emitted by the source Observable. It might be useful, if you do not have control over
  internal scheduler of a given Observable, but want to control when its values are emitted nevertheless.

    @param scheduler Scheduler that will be used to reschedule notifications from source Observable.
    @param delay Number of milliseconds that states with what delay every notification should be rescheduled.
    @return Observable that emits the same notifications as the source Observable, but with provided scheduler.
 */
[@bs.module "rxjs/operators"]
external observeOn: (Rx_Scheduler.t, ~delay: int=?, unit) => operator('a, 'a) = "observeOn";

/**
  When any of the provided Observable emits an complete or error notification, it immediately subscribes to the next one
  that was passed.

    @param observables Observables passed as an array
    @return An Observable that emits values from source Observable, but - if it errors - subscribes to the next passed Observable and so on, until it completes or runs out of Observables.
 */
[@bs.module "rxjs/operators"]
external onErrorResumeNext: (
  [@bs.unwrap] [
    | `Observable(array(t('b)))
    | `Promise(array(Js.Promise.t('b)))
    | `Array(array(array('b)))
  ]
) => operator('a, 'b) = "onErrorResumeNext";

/**
  Groups pairs of consecutive emissions together and emits them as an array of two values.

    @return An Observable of pairs (as arrays) of consecutive values from the source Observable.
 */
[@bs.module "rxjs/operators"]
external pairwise: unit => operator('a, ('a, 'a)) = "pairwise";

/**
  Splits the source Observable into two, one with values that satisfy a predicate, 
  and another with values that don't satisfy the predicate

    @param predicate A function that evaluates each value emitted by the source Observable.
    @return Observables tuple: one with values that passed the predicate, and another with values that did not pass the predicate.
 */
[@bs.module "rxjs/operators"]
external partition: (('a, int) => bool) => unaryFunction(t('a), (t('a), t('a))) = "partition";

// Don't feel like pluck is a good fit. use map instead
 
/**
  Returns a ConnectableObservable, which is a variety of Observable that waits until its connect method is called
  before it begins emitting items to those Observers that have subscribed to it.

    @param Optional selector function which can use the multicasted source sequence as many times as needed, without causing multiple subscriptions to the source sequence.
    @return A ConnectableObservable that upon connection causes the source Observable to emit items to its Observers
 */
[@bs.module "rxjs/operators"]
external publish: (~selector: (t('a) => t('b))=?, unit) => unaryFunction(t('a), Rx_ConnectableObservable.t('b)) = "publish";

[@bs.module "rxjs/operators"]
external publishBehavior: 'a => unaryFunction(t('a), Rx_ConnectableObservable.t('a)) = "publishBehavior";

/**
  Returns a connectable observable sequence that shares a single subscription to the
  underlying sequence containing only the last notification.

  Similar to {@link publish}, but it waits until the source observable completes and stores the last emitted value.

    @return An observable sequence that contains the elements of a sequence produced by multicasting the source sequence.
 */
[@bs.module "rxjs/operators"]
external publishLast: unit => unaryFunction(t('a), Rx_ConnectableObservable.t('a)) = "publishLast";

[@bs.module "rxjs/operators"]
external publishReplay: (
  ~bufferSize: int=?,
  ~windowTime: int=?,
  ~selectorOrScheduler: [@bs.unwrap][
    | `Selector(operator('a, 'b))
    | `Scheduler(Rx_Scheduler.t)
  ]=?,
  unit
) => unaryFunction(t('a), Rx_ConnectableObservable.t('a)) = "publishReplay";

/**
  Applies an accumulator function over the source Observable, and returns the
  accumulated result when the source completes, given an optional seed value.

    @param accumulator The accumulator function called on each source value.
    @param seed The initial accumulation value.
    @return An Observable that emits a single value that is the result of accumulating the values emitted by the source Observable.
 */
[@bs.module "rxjs/operators"]
external reduce: ([@bs.uncurry] (('acc, 'a, int) => 'acc), 'acc) => operator('a, 'acc) = "reduce";

/**
  Make a ConnectableObservable behave like a ordinary observable and automates the way you can connect to it.

  Internally it counts the subscriptions to the observable and subscribes (only once) to the source if
  the number of subscriptions is larger than 0. If the number of subscriptions is smaller than 1, it
  unsubscribes from the source. This way you can make sure that everything before the *published*
  refCount has only a single subscription independently of the number of subscribers to the target observable
 */
[@bs.module "rxjs/operators"]
external refCount: unit => unaryFunction(Rx_ConnectableObservable.t('a), t('a)) = "refCount";

/** 
  Returns an Observable that will resubscribe to the source stream when the source stream completes, at most count times.
    
    @param count The number of times the source Observable items are repeated, a count of 0 will yield an empty Observable.
    @return An Observable that will resubscribe to the source stream when the source stream completes, at most count times.
 */
[@bs.module "rxjs/operators"]
external repeat: (~count: int=?, unit) => operator('a, 'a) = "repeat";

/**
  Returns an Observable that mirrors the source Observable with the exception of a `complete`. If the source
  Observable calls `complete`, this method will emit to the Observable returned from `notifier`. If that Observable
  calls `complete` or `error`, then this method will call `complete` or `error` on the child subscription. Otherwise
  this method will resubscribe to the source Observable.

    @param notifier - Receives an Observable of notifications with which a user can `complete` or `error`, aborting the repetition.
    @return The source Observable modified with repeat logic
 */
[@bs.module "rxjs/operators"]
external repeatWhen: ([@bs.uncurry](t('error) => t('n))) => operator('a, 'a) = "repeatWhen";

/**
  Returns an Observable that mirrors the source Observable with the exception of an `error`. If the source Observable
  calls `error`, this method will resubscribe to the source Observable for a maximum of `count` resubscriptions 
  (given as a number parameter) rather than propagating the `error` call.

    @param count Number of retry attempts before failing.
    @return The source Observable modified with the retry logic.
 */
[@bs.module "rxjs/operators"]
external retry: (~count: int=?, unit) => operator('a, 'a) = "retry";

/**
  Returns an Observable that mirrors the source Observable with the exception of an `error`. If the source Observable
  calls `error`, this method will emit the Throwable that caused the error to the Observable returned from `notifier`.
  If that Observable calls `complete` or `error` then this method will call `complete` or `error` on the child
  subscription. Otherwise this method will resubscribe to the source Observable.

    @param notifier Receives an Observable of notifications with which a user can `complete` or `error`, aborting the retry
    @return The source Observable modified with retry logic.
 */
[@bs.module "rxjs/operators"]
external retryWhen: ([@bs.uncurry](t('error) => t('n))) => operator('a, 'a) = "retryWhen";

/**
  Emits the most recently emitted value from the source Observable whenever another Observable, the `notifier`, emits.

    @param The Observable to use for sampling the source Observable.
    @return An Observable that emits the results of sampling the values emitted by the source Observable whenever the notifier Observable emits value or completes.
 */
[@bs.module "rxjs/operators"]
external sample: t('n) => operator('a, 'a) = "sample";

/**
  Emits the most recently emitted value from the source Observable within periodic time intervals.

    @param The sampling period expressed in milliseconds or the time unit determined internally by the optional `scheduler`.
    @param scheduler The Scheduler to use for managing the timers that handle the sampling.
    @return An Observable that emits the results of sampling the values emitted by the source Observable at the specified time interval.
 */
[@bs.module "rxjs/operators"]
external sampleTime: (int, ~scheduler: Rx_Scheduler.t=?, unit) => operator('a, 'a) = "sampleTime";

/**
  Applies an accumulator function over the source Observable, and returns each
  intermediate result, with an optional seed value.

  It's like reduce}, but emits the current accumulation whenever the source emits a value.

    @param accumulator The accumulator function called on each source value.
    @param seed The initial accumulation value.
    @return An observable of the accumulated values.
 */
[@bs.module "rxjs/operators"]
external scan: ([@bs.uncurry] (('acc, 'a, int) => 'acc), 'acc) => operator('a, 'acc) = "scan";

[@bs.module "rxjs/operators"]
external _sequenceEqual: (t('a), ~comparator:[@bs.uncurry](('a, 'a) => bool)) => operator('a, bool) = "sequenceEqual";

/**
  Compares all values of two observables in sequence using an optional comparator function
  and returns an observable of a single boolean value representing whether or not the two sequences are equal.

    @param compareTo The observable sequence to compare the source sequence to.
    @param comparator An optional function to compare each value pair
    @return An Observable of a single boolean value representing whether or not the values emitted by both observables were equal in sequence.
 */
let sequenceEqual = (other, ~comparator=(lhs, rhs) => lhs === rhs, ()) => _sequenceEqual(other, ~comparator);

/**
  Returns a new Observable that multicasts (shares) the original Observable. As long as there is at least one
  Subscriber this Observable will be subscribed and emitting data. When all subscribers have unsubscribed it will
  unsubscribe from the source Observable. Because the Observable is multicasting it makes the stream `hot`.
  This is an alias for obs |> Rx.Operators.multicast(`SubjectFactory(() => Rx.Subject.create()), ()) |> Rx.Operators.refCount()`.

    @return An Observable that upon connection causes the source Observable to emit items to its Observers.
 */
[@bs.module "rxjs/operators"]
external share: unit => operator('a, 'a) = "share";

/**
  Share source and replay specified number of emissions on subscription.
  
  This operator is a specialization of `replay` that connects to a source observable
  and multicasts through a `ReplaySubject` constructed with the specified arguments.
  A successfully completed source will stay cached in the `shareReplayed observable` forever,
  but an errored source can be retried.

    @param bufferSize Maximum element count of the replay buffer.
    @param windowTime Maximum time length of the replay buffer in milliseconds.
    @param scheduler Scheduler where connected observers within the selector function will be invoked on.
    @return An observable sequence that contains the elements of a sequence produced by multicasting the source sequence within a selector function.
 */
[@bs.module "rxjs/operators"]
external shareReplay: (
  ~bufferSize: int=?,
  ~windowTime: int=?,
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => operator('a, 'a) = "shareReplay";

[@bs.module "rxjs/operators"]
external _single: (
  ~predicate: [@bs.uncurry](('a, int, t('a)) => bool)
) => operator('a, 'b) = "single";

/**
  Returns an Observable that emits the single item emitted by the source Observable that matches a specified
  predicate, if that Observable emits one such item. If the source Observable emits more than one such item or no
  items, notify of an IllegalArgumentException or NoSuchElementException respectively. If the source Observable
  emits items but none match the specified predicate then `undefined` is emitted.
  
  Like first, but emit with error notification if there is more than one value
 */
let single = (~predicate=(_x, _idx, _src) => true, ()) => _single(~predicate);

/**
  Returns an Observable that skips the first `count` items emitted by the source Observable.

    @param The number of times, items emitted by source Observable should be skipped.
    @return An Observable that skips values emitted by the source Observable.
 */
[@bs.module "rxjs/operators"] 
external skip: int => operator('a, 'a) = "skip";

/**
  Skip the last `count` values emitted by the source Observable.

    @param count Number of elements to skip from the end of the source Observable.
    @param An Observable that skips the last count values emitted by the source Observable.
 */
[@bs.module "rxjs/operators"]
external skipLast: int => operator('a, 'a) = "skipLast";

/**
  Returns an Observable that skips items emitted by the source Observable until a second Observable emits an item.

  The `skipUntil` operator causes the observable stream to skip the emission of values ​​until the passed in observable emits the first value.
  This can be particularly useful in combination with user interactions, responses of http requests or waiting for specific times to pass by.

    @param The second Observable that has to emit an item before the source Observable's elements begin to be mirrored by the resulting Observable.
    @return An Observable that skips items from the source Observable until the second Observable emits an item, then emits the remaining items.
 */
[@bs.module "rxjs/operators"]
external skipUntil: t('n) => operator('a, 'a) = "skipUntil";

/**
  Returns an Observable that skips all items emitted by the source Observable as long as a specified condition holds
  true, but emits all further source items as soon as the condition becomes false.

    @param predicate A function to test each item emitted from the source Observable.
    @return An Observable that begins emitting items emitted by the source Observable when the specified predicate becomes false.
 */
[@bs.module "rxjs/operators"]
external skipWhile: ([@bs.uncurry] (('a, int) => bool)) => operator('a, 'a) = "skipWhile";

/**
  Returns an Observable that emits the items you specify as arguments before it begins to emit
  items emitted by the source Observable.

    @param Items you want the modified Observable to emit first
    @return An Observable that emits the items in the specified Iterable and then emits the items emitted by the source Observable.
 */
[@bs.module "rxjs/operators"][@bs.variadic]
external startWith: array('a) => operator('a, 'a) = "startWith";

/**
  Asynchronously subscribes Observers to this Observable on the specified Scheduler
  With subscribeOn you can decide what type of scheduler a specific Observable will be using when it is subscribed to.

    @param The SchedulerLike to perform subscription actions on.
    @param delay delay
    @return The source Observable modified so that its subscriptions happen on the specified Scheduler
 */
[@bs.module "rxjs/operators"]
external subscribeOn: (Rx_Scheduler.t, ~delay: int=?, unit) => operator('a, 'a) = "subscribeOn";

/**
  Converts a higher-order Observable into a first-order Observable
  producing values only from the most recent observable sequence
 */
[@bs.module "rxjs/operators"]
external switchAllObservable: unit => operator(t('a), 'a) = "switchAll";

/**
  Converts a higher-order Observable into a first-order Observable
  producing values only from the most recent observable sequence
 */
[@bs.module "rxjs/operators"]
external switchAllPromise: unit => operator(Js.Promise.t('a), 'a) = "switchAll";

/**
  Converts a higher-order Observable into a first-order Observable
  producing values only from the most recent observable sequence
 */
[@bs.module "rxjs/operators"]
external switchAllArray: unit => operator(array('a), 'a) = "switchAll";

/**
  Projects each source value to an Observable which is merged in the output
  Observable, emitting values only from the most recently projected Observable.

    @param A function that, when applied to an item emitted by the source Observable, returns an Observable.
    @return An Observable that emits the result of applying the projection function (and the optional deprecated `resultSelector`) to each item emitted by the source Observable and taking only the values from the most recently projected inner Observable.
 */
[@bs.module "rxjs/operators"]
external switchMap: ([@bs.uncurry] (('a, int) => t('b))) => operator('a, 'b) = "switchMap";

/**
  Projects each source value to the same Observable which is flattened multiple
  times with switchMap in the output Observable.

    @param innerObservable An Observable to replace each value from the source Observable.
    @return An Observable that emits items from the given `innerObservable` (and optionally transformed through the deprecated `resultSelector` every time a value is emitted on the source Observable, and taking only the values from the most recently projected inner Observable.
 */
[@bs.module "rxjs/operators"]
external switchMapTo: ([@bs.unwrap] [
  | `Observable(t('a))
  | `Promise(Js.Promise.t('a))
  | `Array( array('a))
]) => operator('b, 'a) = "switchMapTo";

/**
  Emits only the first `count` values emitted by the source Observable.

    @param count The maximum number of `next` values to emit.
    @return An Observable that emits only the first `count` values emitted by the source Observable, or all of the values from the source if the source emits fewer than `count` values.
 */
[@bs.module "rxjs/operators"] external take: int => operator('a, 'a) = "take";

/**
  Emits only the last `count` values emitted by the source Observable.

    @param count The maximum number of values to emit from the end of the sequence of values emitted by the source Observable.
    @return An Observable that emits at most the last count values emitted by the source Observable.
 */
[@bs.module "rxjs/operators"]
external takeLast: int => operator('a, 'a) = "takeLast";

/**
  Emits the values emitted by the source Observable until a `notifier` Observable emits a value.

  Lets values pass until a second Observable, `notifier`, emits a value. Then, it completes.

    @param notifier The Observable whose first emitted value will cause the output Observable of `takeUntil` to stop emitting values from the source Observable.
    @return An Observable that emits the values from the source Observable until such time as `notifier` emits its first value.
 */
[@bs.module "rxjs/operators"]
external takeUntil: t('n) => operator('a, 'a) = "takeUntil";

/**
  Emits values emitted by the source Observable so long as each value satisfies
  the given `predicate`, and then completes as soon as this `predicate` is not
  satisfied.

    @param predicate A function that evaluates a value emitted by the source Observable and returns a boolean. Also takes the (zero-based) index as the second argument.
    @param inclusive When set to `true` the value that caused `predicate` to return `false` will also be emitted.
    @return An Observable that emits the values from the source Observable so long as each value satisfies the condition defined by the `predicate`, then completes.
 */
[@bs.module "rxjs/operators"]
external takeWhile:
  ([@bs.uncurry] (('a, int) => bool), ~inclusive: bool=?, unit) => operator('a, 'a) = "takeWhile";

[@bs.module "rxjs/operators"]
external _tap: (
  ~next: [@bs.uncurry]('a => unit), 
  ~error: [@bs.uncurry]('e => unit), 
  ~complete: [@bs.uncurry](unit => unit),
) => operator('a, 'a) = "tap";

/**
  Perform a side effect for every emission on the source Observable, but return an Observable that is identical to the source.

  Intercepts each emission on the source and runs a
  function, but returns an output which is identical to the source as long as errors don't occur.

    @param next a callback for `next`.
    @param error Callback for errors in the source.
    @param complete Callback for the completion of the source.
    @return An Observable identical to the source, but runs the specified callback(s) for each item.
 */
let tap = (~next=_val=>(), ~error=_err=>(), ~complete=()=>(), ()) => _tap(~next, ~error, ~complete);

/**
  Perform a side effect for every emission on the source Observable, but return an Observable that is identical to the source.

  Intercepts each emission on the source and runs a
  function, but returns an output which is identical to the source as long as errors don't occur.

    @param observer A normal Observer object
    @return An Observable identical to the source, but runs the specified Observer for each item.
 */
[@bs.module "rxjs/operators"]
external tapObserver: Rx_Types.Observer.t('a, 'e) => operator('a, 'a) = "tap";

/**
  Emits a value from the source Observable, then ignores subsequent source
  values for a duration determined by another Observable, then repeats this
  process.

    @param durationSelector A function that receives a value from the source Observable, for computing the silencing duration for each source value, returned as an Observable or a Promise.
    @param config a configuration object to define `leading` and `trailing` behavior. Defaults to { leading: true, trailing: false}.
    @return An Observable that performs the throttle operation to limit the rate of emissions from the source.
 */
[@bs.module "rxjs/operators"]
external throttle: (
  [@bs.unwrap] [`Subscribable('a => t('a)) | `Promise('a => Js.Promise.t('a))],
  ~config: Rx_Types.ThrottleConfig.t=?,
  unit
) => operator('a, 'a) = "throttle";

/**
  Emits a value from the source Observable, then ignores subsequent source
  values for `duration` milliseconds, then repeats this process.

  Lets a value pass, then ignores source values for the next `duration` milliseconds.

    @param duration Time to wait before emitting another value after emitting the last value, measured in milliseconds or the time unit determined internally by the optional `scheduler`.
    @param The Scheduler to use for managing the timers that handle the throttling.
    @param config a configuration object to define `leading` and `trailing` behavior. Defaults to { leading: true, trailing: false}.
    @return An Observable that performs the throttle operation to limit the rate of emissions from the source.
 */
[@bs.module "rxjs/operators"]
external throttleTime: (
  ~duration: int, 
  ~scheduler: Rx_Scheduler.t=?,
  ~config: Rx_Types.ThrottleConfig.t=?, 
  unit
) => operator('a, 'a) = "throttleTime";

/**
  If the source observable completes without emitting a value, it will emit
  an error. The error will be created at that time by the `errorFactor` argument

    @param A factory function called to produce the error to be thrown when the source observable completes without emitting a value.
 */
[@bs.module "rxjs/operators"]
external throwIfEmpty: ([@bs.uncurry](unit => 'err)) => operator('a, 'a) = "throwIfEmpty";

/**
  Emits an object containing the current value, and the time that has
  passed between emitting the current value and the previous value, which is
  calculated by using the provided `scheduler`'s `now()` method to retrieve
  the current time at each emission, then calculating the difference. The `scheduler`
  defaults to syncScheduler, so by default, the `interval` will be in milliseconds.

    @param scheduler Scheduler used to get the current time.
    @return Observable that emit infomation about value and interval
 */
[@bs.module "rxjs/operators"]
external timeInterval: (~scheduler: Rx_Scheduler.t=?, unit) => operator('a, Rx_Types.TimeInterval.t('a)) = "timeInterval"; 

/**
  Errors if Observable does not emit a value in given time span.
  Timeouts on Observable that doesn't emit values fast enough.

    @param due Number specifying period within which Observable must emit values or Date specifying before when Observable should complete
    @param scheduler Scheduler controlling when timeout checks occur.
    @return Observable that mirrors behaviour of source, unless timeout checks fail.
 */
[@bs.module "rxjs/operators"]
external timeout: (
  [@bs.unwrap][ `Number(int) | `Date(Js.Date.t)],
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => operator('a, 'a) = "timeout";

/**
  Errors if Observable does not emit a value in given time span, in case of which
  subscribes to the second Observable.

  It's a version of `timeout` operator that let's you specify fallback Observable.

    @param due Number specifying period within which Observable must emit values or Date specifying before when Observable should complete
    @param withObservable Observable which will be subscribed if source fails timeout check.
    @return Observable that mirrors behaviour of source or, when timeout check fails, of an Observable passed as a second parameter.
 */
[@bs.module "rxjs/operators"]
external timeoutWith: (
  ~due: [@bs.unwrap][ `Number(int) | `Date(Js.Date.t)],
  ~withObservable: [@bs.unwrap] [
    | `Observable(t('a))
    | `Promise(Js.Promise.t('a))
    | `Array( array('a))
  ],
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => operator('a, 'a) = "timeoutWith";

/**
  Attaches a timestamp to each item emitted by an observable indicating when it was emitted

  The `timestamp` operator maps the *source* observable stream to an object of type
  `{value: T, timestamp: R}`. The properties are generically typed. The `value` property contains the value
  and type of the *source* observable. The `timestamp` is generated by the schedulers `now` function. By
  default it uses the *async* scheduler which simply returns `Date.now()`
  (milliseconds since 1970/01/01 00:00:00:000) and therefore is of type `number`.
 */
[@bs.module "rxjs/operators"]
external timestamp: (
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => operator('a, Rx_Types.Timestamp.t('a)) = "timestamp";

/**
  Collects all source emissions and emits them as an array when the source completes.

    @return An array from an observable sequence.
 */
[@bs.module "rxjs/operators"]
external toArray: unit => operator('a, array('a)) = "toArray";

/**
  Branch out the source Observable values as a nested Observable whenever `windowBoundaries` emits.

    @param windowBoundaries An Observable that completes the previous window and starts a new window.
    @return An Observable of windows, which are Observables emitting values of the source Observable.
 */
[@bs.module "rxjs/operators"]
external window: t('n) => operator('a, t('a)) = "window";

/**
  Branch out the source Observable values as a nested Observable with each
  nested Observable emitting at most `windowSize` values.

  It's like bufferCount, but emits a nested Observable instead of an array.

    @param windowSize The maximum number of values emitted by each window.
    @return An Observable of windows, which in turn are Observable of values.
 */
[@bs.module "rxjs/operators"]
external windowCount: (int, ~startWindowEvery: int=?, unit) => operator('a, t('a)) = "windowCount";

/**
  Branch out the source Observable values as a nested Observable periodically in time.
  
  It's like bufferTime, but emits a nested Observable instead of an array.

    @param windowTimeSpan The amount of time to fill each window.
    @param windowCreationInterval The interval at which to start new windows.
    @param maxWindowSize Max number of values each window can emit before completion.
    @param scheduler The scheduler on which to schedule the intervals that determine window boundaries.
    @return An observable of windows, which in turn are Observables.
 */
[@bs.module "rxjs/operators"]
external windowTime: (
  ~windowTimeSpan: int,
  ~windowCreationTimeInterval: int,
  ~maxWindowSize: int,
  ~scheduler: Rx_Scheduler.t=?,
  unit
) => operator('a, t('a)) = "windowTime";

/**
  Branch out the source Observable values as a nested Observable starting from
  an emission from `openings` and ending when the output of `closingSelector` emits.

  It's like bufferToggle, but emits a nested Observable instead of an array.

    @param openings An observable of notifications to start new windows.
    @param closingSelector A function that takes the value emitted by the `openings` observable and returns an Observable, which, when it emits (either `next` or `complete`), signals that the associated window should complete.
    @return An observable of windows, which in turn are Observables.
 */
[@bs.module "rxjs/operators"]
external windowToggle: (
  ~opening: t('o),
  ~closingSelector: [@bs.uncurry]('o => t('s))
) => operator('a, t('a)) = "windowToggle";

/**
  Branch out the source Observable values as a nested Observable using a
  factory function of closing Observables to determine when to start a new window.

    @param closingSelector A function that takes no arguments and returns an Observable that signals (on either `next` or `complete`) when to close the previous window and start a new one.
    @return An observable of windows, which in turn are Observables.
 */
[@bs.module "rxjs/operators"]
external windowWhen: ([@bs.uncurry]('o => t('s))) => operator('a, t('a)) = "windowWhen";

/**
  Combines the source Observable with other Observables to create an Observable
  whose values are calculated from the latest values of each, only when the source emits.

    @param other An input Observable array to combine with the source Observable.
    An Observable of projected values from the most recent values from each input Observable, or an array of the most recent values from each input Observable.
 */
[@bs.module "rxjs/operators"][@bs.variadic]
external withLatestFrom: array(t('a)) => operator('a, array('a)) = "withLatestFrom";
[@bs.module "rxjs/operators"]
external withLatestFrom2: t('b) => operator('a, ('a, 'b)) = "withLatestFrom";
[@bs.module "rxjs/operators"]
external withLatestFrom3: (t('b), t('c)) => operator('a, ('a, 'b, 'c)) = "withLatestFrom";
[@bs.module "rxjs/operators"]
external withLatestFrom4: (t('b), t('c), t('d)) => operator('a, ('a, 'b, 'c, 'd)) = "withLatestFrom";
[@bs.module "rxjs/operators"]
external withLatestFrom5: (t('b), t('c), t('d), t('e)) => operator('a, ('a, 'b, 'c, 'd, 'e)) = "withLatestFrom";
[@bs.module "rxjs/operators"]
external withLatestFrom6: (t('b), t('c), t('d), t('e), t('f)) => operator('a, ('a, 'b, 'c, 'd, 'e, 'f)) = "withLatestFrom";
[@bs.module "rxjs/operators"]
external withLatestFrom7: (t('b), t('c), t('d), t('e), t('f), t('g)) => operator('a, ('a, 'b, 'c, 'd, 'e, 'f, 'g)) = "withLatestFrom";
[@bs.module "rxjs/operators"]
external withLatestFrom8: (t('b), t('c), t('d), t('e), t('f), t('g), t('h)) => operator('a, ('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h)) = "withLatestFrom";

[@bs.module "rxjs/operators"]
external zipAll: unit => operator(t('a), array('a)) = "zipAll";

[@bs.module "rxjs/operators"]
external zipAllPromise: unit => operator(Js.Promise.t('a), array('a)) = "zipAll";

[@bs.module "rxjs/operators"]
external _zipAllProject: (. (array('a) => 'r)) => operator(t('a), 'r) = "zipAll";

let zipAllProject = (project: projectFn(array('a),'r)) => {
  let _makeVariadic: (. projectFn(array('a),'r)) => projectFn(array('a),'r) = [%bs.raw{|
    function(fn) {
      return function(){
        return fn([].slice.call(arguments))
      }
    }
  |}];

  let _project = _makeVariadic(. project);
  _zipAllProject(. _project);
};

[@bs.module "rxjs/operators"]
external _zipAllPromiseProject: (. (array('a) => 'r)) => operator(Js.Promise.t('a), 'r) = "zipAll";

let zipAllPromiseProject = (project: projectFn(array('a),'r)) => {
  let _makeVariadic: (. projectFn(array('a),'r)) => projectFn(array('a),'r) = [%bs.raw{|
    function(fn) {
      return function(){
        return fn([].slice.call(arguments))
      }
    }
  |}];

  let _project = _makeVariadic(. project);
  _zipAllPromiseProject(. _project);
};  