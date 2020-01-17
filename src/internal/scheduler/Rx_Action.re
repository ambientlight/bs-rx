/**
  A unit of work to be executed in a `scheduler`. An action is typically
  created from within a {@link SchedulerLike} and an RxJS user does not need to concern
  themselves about creating and manipulating an Action.
 */

module Impl = (T: {type t('a);}) => {
  [@bs.module "rxjs"][@bs.new]
  external create: (~scheduler: Rx_Scheduler.t, ~work: [@bs.uncurry](~state: 'a=?, unit) => unit) => T.t('a) = "Action";

  /**
  Schedules this action on its parent {@link SchedulerLike} for execution. May be passed
  some context object, `state`. May happen at some point in the future,
  according to the `delay` parameter, if specified.

    @param state Some contextual data that the `work` function uses when called by the Scheduler.
    @param delay Time to wait before executing the work, where the time unit is implicit and defined by the Scheduler.
   */
  [@bs.send.pipe: T.t('a)]
  external schedule: (~state: 'a, ~delay: int) => Rx_Subscription.t = "schedule"
};

type t('a);
include Rx_Subscription.Impl({ type nonrec t = t(unit); });
include Impl({ type nonrec t('a) = t('a); });