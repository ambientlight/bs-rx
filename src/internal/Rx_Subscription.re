/**
  Represents a disposable resource, such as the execution of an Observable. A
  Subscription has one important method, `unsubscribe`, that takes no argument
  and just disposes the resource held by the subscription.

  Additionally, subscriptions may be grouped together through the `add()`
  method, which will attach a child Subscription to the current Subscription.
  When a Subscription is unsubscribed, all its children (and its grandchildren)
  will be unsubscribed as well.
 */

module Impl = (T: {type t;}) => {
  /**
  A flag to indicate whether this Subscription has already been unsubscribed

    @return whether this Subscription has already been unsubscribed
  */
  [@bs.get] external closed: T.t => bool = "closed";

  /**
  creates a new subscription object

    @return newly created subscription object
  */
  [@bs.module "rxjs"][@bs.new]
  external create: ([@bs.uncurry] (unit => unit)) => T.t = "Subscription";

  /**
  Disposes the resources held by the subscription. May, for instance, cancel
  an ongoing Observable execution or cancel any other type of work that
  started when the Subscription was created
  */
  [@bs.send] external unsubscribe: T.t => unit = "unsubscribe";

  /**
  Adds a tear down to be called during the unsubscribe() of this
  Subscription. Can also be used to add a child subscription.
  
  If the tear down being added is a subscription that is already
  unsubscribed, is the same reference `add` is being called on, or is
  `Subscription.EMPTY`, it will not be added.
  
  If this subscription is already in an `closed` state, the passed
  tear down logic will be executed immediately.
  
  When a parent subscription is unsubscribed, any child subscriptions that were added to it are also unsubscribed.

    @param teardown The additional logic to execute on teardown
    @return subscription used or created to be added to the inner subscriptions list. This Subscription can be used with `remove()` to remove the passed teardown logic from the inner subscriptions list.
  */
  [@bs.send.pipe: T.t] external add: (
    [@bs.unwrap] [`Unsubscribable(T.t) | `Function(unit => unit)]
  ) => T.t = "add";

  /**
  Removes a Subscription from the internal list of subscriptions that will
  unsubscribe during the unsubscribe process of this Subscription.
    
    @param subscription The subscription to remove.
  */
  [@bs.send.pipe: T.t] external remove: ( 
    [@bs.unwrap] [`Unsubscribable(T.t) | `Function(unit => unit)]
  ) => unit = "remove";
  };

type t;
include Impl({ type nonrec t = t; });