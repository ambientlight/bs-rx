type t('a);

/**
  creates a new subscriber

    @param {Observer|function(value: T): void} [destinationOrNext] A partially defined Observer or a `next` callback function.
    @param {function(e: ?any): void} [error] The `error` callback of an Observer.
    @param {function(): void} [complete] The `complete` callback of an Observer.
    @return new subscriber
*/
[@bs.module "rxjs"][@bs.new]
external create: (
  ~destinationOrNext: [@bs.unwrap][`Destination(t('a)) | `Next('a => unit)],
  ~error: [@bs.uncurry]('error => unit),
  ~complete: [@bs.uncurry](unit => unit),
  unit
) => t('a) = "Subscriber";

/**
  A static factory for a Subscriber, given a (potentially partial) definition
  of an Observer.
  
    @param next The `next` callback of an Observer.
    @param error The `error` callback of an Observer.
    @param complete The `complete` callback of an Observer.
    @return A Subscriber wrapping the (partially defined) Observer represented by the given arguments.
   */
[@bs.val][@bs.module "rxjs"][@bs.scope "Subscriber"]
external createStatic: (
  ~next: [@bs.uncurry]('a => unit),
  ~error: [@bs.uncurry]('error => unit),
  ~complete: [@bs.uncurry](unit => unit),
  unit
) => t('a) = "create";

/**
  The {@link Observer} callback to receive notifications of type `next` from
  the Observable, with a value. The Observable may call this method 0 or more
  times.

    @param value The `next` value.
*/
[@bs.send.pipe: t('a)] external next: 'a => unit = "next";

/**
  The {@link Observer} callback to receive notifications of type `error` from
  the Observable, with an attached `Error`. Notifies the Observer that
  the Observable has experienced an error condition.
    
    @param err The `error` exception.
*/
[@bs.send.pipe: t('a)] external error: 'error => unit = "error";

/**
  The {@link Observer} callback to receive a valueless notification of type
  `complete` from the Observable. Notifies the Observer that the Observable
  has finished sending push-based notifications.  
*/
[@bs.send.pipe: t('a)] external complete: unit = "complete";


[@bs.send.pipe: t('a)] external unsubscribe: unit = "unsubscribe";