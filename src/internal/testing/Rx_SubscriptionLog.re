type t;

[@bs.get] external subscribedFrame: t => int = "subscribedFrame";
[@bs.set] external setSubscribedFrame: (t, int) => unit = "subscribedFrame";
[@bs.get] external unsubscribedFrame: t => int = "unsubscribedFrame";
[@bs.set] external setUnsubscribedFrame: (t, int) => unit = "unsubscribedFrame";