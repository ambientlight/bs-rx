module Ajax = Rx_Ajax;
module Fetch = Rx_Fetch;
module Operators = Rx_Operators;
module Testing = Rx_Testing;
module WebSocket = Rx_WebSocket;

module Notification = Rx_Notification;
module ConnectableObservable = Rx_ConnectableObservable;

module Subject = Rx_Subject;
module BehaviorSubject = Rx_BehaviorSubject;
module ReplaySubject = Rx_ReplaySubject;
module AsyncSubject = Rx_AsyncSubject;

include Rx_Observable;
include Rx_Types;
include Rx_Utils;

/**
  Asap Scheduler
  Perform task as fast as it can be performed asynchronously

  `asap` scheduler behaves the same as {@link asyncScheduler} scheduler when you use it to delay task
  in time. If however you set delay to `0`, `asap` will wait for current synchronously executing
  code to end and then it will try to execute given task as fast as possible.
 */
[@bs.module "rxjs"][@bs.val] external asapScheduler: Rx_AsapScheduler.t = "asapScheduler";
/** shortcut to spare Ex.AsapScheduler.asScheduler call */
[@bs.module "rxjs"][@bs.val] external asap: Rx_Scheduler.t = "asapScheduler";

/**
  Async Scheduler
  Schedule task as if you used setTimeout(task, duration)

  `async` scheduler schedules tasks asynchronously, by putting them on the JavaScript
  event loop queue. It is best used to delay tasks in time or to schedule tasks repeating
  in intervals.
 */
[@bs.module "rxjs"][@bs.val] external asyncScheduler: Rx_AsyncScheduler.t = "asyncScheduler";
/** shortcut to spare Rx.AsyncScheduler.asScheduler call */
[@bs.module "rxjs"][@bs.val] external async: Rx_Scheduler.t = "asyncScheduler";

/**
  Queue Scheduler
  Put every next task on a queue, instead of executing it immediately

  `queue` scheduler, when used with delay, behaves the same as {@link asyncScheduler} scheduler.

  When used without delay, it schedules given task synchronously - executes it right when
  it is scheduled. However when called recursively, that is when inside the scheduled task,
  another task is scheduled with queue scheduler, instead of executing immediately as well,
  that task will be put on a queue and wait for current one to finish.
 */
[@bs.module "rxjs"][@bs.val] external queueScheduler: Rx_QueueScheduler.t = "queueScheduler";
/** shortcut to spare Rx.QueueScheduler.asScheduler call */
[@bs.module "rxjs"][@bs.val] external queue: Rx_Scheduler.t = "queueScheduler";

/**
  Animation Frame Scheduler
  Perform task when `window.requestAnimationFrame` would fire

  When `animationFrame` scheduler is used with delay, it will fall back to {@link asyncScheduler} scheduler behaviour

  Without delay, `animationFrame` scheduler can be used to create smooth browser animations.
  It makes sure scheduled task will happen just before next browser content repaint,
  thus performing animations as efficiently as possible.
 */
[@bs.module "rxjs"][@bs.val] external animationFrameScheduler: Rx_AnimationFrameScheduler.t = "animationFrameScheduler";
/** shortcut to spare Rx.AnimationFrameScheduler.asScheduler call */
[@bs.module "rxjs"][@bs.val] external animationFrame: Rx_Scheduler.t = "animationFrameScheduler";

module AsapScheduler = Rx_AsapScheduler;
module AsyncScheduler = Rx_AsyncScheduler;
module QueueScheduler = Rx_QueueScheduler;
module AnimationFrameScheduler = Rx_AnimationFrameScheduler;

module VirtualTimeScheduler = Rx_VirtualTimeScheduler;
// module VirtualAction = Rx_VirtualAction;
module Scheduler = Rx_Scheduler;