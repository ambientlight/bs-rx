module Impl = (T: { type t('a); }) => {
  [@bs.get] external observers: T.t('a) => array(Rx_Types.Observer.t('a, 'e)) = "observers";
  [@bs.set] external setObservers: (T.t('a), array(Rx_Types.Observer.t('a, 'e))) => unit = "observers";

  [@bs.get] external closed: T.t('a) => bool = "closed";
  [@bs.set] external setClosed: (T.t('a), bool) => unit = "closed";

  [@bs.get] external isStopped: T.t('a) => bool = "isStopped";
  [@bs.set] external setIsStopped: (T.t('a), bool) => unit = "isStopped";

  [@bs.get] external hasError: T.t('a) => bool = "hasError";
  [@bs.set] external setHasError: (T.t('a), bool) => unit = "hasError";

  [@bs.get] external thrownError: T.t('a) => Js.Nullable.t('e) = "thrownError";
  [@bs.set] external setThrownError: (T.t('a), Js.Nullable.t('e)) => unit = "thrownError";

  /**
  The {@link Observer} callback to receive notifications of type `next` from
  the Observable, with a value. The Observable may call this method 0 or more
  times.

    @param value The `next` value.
  */
  [@bs.send.pipe: T.t('a)] external next: 'a => unit = "next";

  /**
  The {@link Observer} callback to receive notifications of type `error` from
  the Observable, with an attached `Error`. Notifies the Observer that
  the Observable has experienced an error condition.
    
    @param err The `error` exception.
  */
  [@bs.send.pipe: T.t('a)] external error: 'error => unit = "error";

  /**
  The {@link Observer} callback to receive a valueless notification of type
  `complete` from the Observable. Notifies the Observer that the Observable
  has finished sending push-based notifications.  
   */
  [@bs.send.pipe: T.t('a)] external complete: unit = "complete";


  [@bs.send.pipe: T.t('a)] external unsubscribe: unit = "unsubscribe";

  /**
  Creates a new Observable with this Subject as the source. You can do this
  to create customize Observer-side logic of the Subject and conceal it from
  code that uses the Observable.
    @return Observable that the Subject casts to
   */
  [@bs.send.pipe: T.t('a)] external asObservable: Rx_Observable.Observable.t('a) = "asObservable";
};


type t('a);

include Rx_Observable.Observable.Impl({ type nonrec t('a) = t('a) });
include Impl({ type nonrec t('a) = t('a) });