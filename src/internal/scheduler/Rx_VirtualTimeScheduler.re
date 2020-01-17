module Impl = (T: {type t;}) => {
  [@bs.module "rxjs"][@bs.new]
  external create: (~schedulerAction: Rx_AsyncAction.t(Js.t('a))=?, ~maxFrames: int=?, unit) => T.t = "VirtualTimeScheduler";

  /**
  The current frame for the state of the virtual scheduler instance. The the difference
  between two "frames" is synonymous with the passage of "virtual time units". So if
  you record `scheduler.frame` to be `1`, then later, observe `scheduler.frame` to be at `11`,
  that means `10` virtual time units have passed.
   */
  [@bs.get] external frame: T.t => int = "frame";

  /**
  The current frame for the state of the virtual scheduler instance. The the difference
  between two "frames" is synonymous with the passage of "virtual time units". So if
  you record `scheduler.frame` to be `1`, then later, observe `scheduler.frame` to be at `11`,
  that means `10` virtual time units have passed.
   */
  [@bs.set] external setFrame: (T.t, int) => unit = "frame";


  /**
  Prompt the Scheduler to execute all of its queued actions, therefore
  clearing its queue 
   */
  [@bs.send.pipe: T.t]
  external flush: unit => unit = "flush";
};

type t;

include Rx_Scheduler.Impl({ type nonrec t = t });
include Rx_AsyncScheduler.Impl({ type nonrec t = t });
include Impl({ type nonrec t = t });