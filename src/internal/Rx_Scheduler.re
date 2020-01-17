type t;
type schedulerT = t;

module Impl = (T: {type t;}) => {
  external asScheduler : T.t => schedulerT = "%identity";

  /**
  Schedules a function, work, for execution. 
  May happen at some point in the future, according to the delay parameter, if specified. 
  May be passed some context object, state, which will be passed to the work function.

    @param work A function representing a task, or some unit of work to be executed by the Scheduler.
    @param delay Time to wait before executing the work, where the time unit is implicit and defined by the Scheduler itself.
    @param state Some contextual data that the `work` function uses when called by the Scheduler.
    @return A subscription in order to be able to unsubscribe the scheduled work.
   */
  [@bs.send.pipe: T.t] external schedule: (~work: ([@bs.uncurry] 'a => unit), ~delay: float, ~state: 'a=?) => Rx_Subscription.t = "schedule"
}

include Impl({ type nonrec t = t; });