module Impl = (T: {type t;}) => {

};

type t;
include Rx_Scheduler.Impl({ type nonrec t = t });
include Rx_AsyncScheduler.Impl({ type nonrec t = t });
include Impl({ type nonrec t = t });