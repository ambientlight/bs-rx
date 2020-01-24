open Rx.Testing;
open Jest;
open Expect;
open TestScheduler;

let testMarbles = (name, callback) => test(name, () => {
  let ts = TestScheduler.create(~assertDeepEqual=BsMocha.Assert.deep_equal);
  ts |> TestScheduler.run(_r => {
    callback(ts)
    expect(true) 
  })
  |> toBe(true)
});

let testMarblesWithHotPreset = (
  ~name,
  ~hot as _hot,
  ~expected,
  ~subs,
  ~operator,
  ~values=?,
  ~hotValues=?,
  ()
) => {
  testMarbles(name, ts => {
    let e1 = ts |> hot(_hot, ~values=?(hotValues |. Belt.Option.map(gen => gen(ts))));
    ts |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> operator)
    |> toBeObservable(expected, ~values?);

    ts 
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(subs);
  })
}

let testMarblesWithColdPreset = (
  ~name,
  ~cold as _cold,
  ~expected,
  ~subs,
  ~operator,
  ~values=?,
  ~coldValues=?,
  ()
) => {
  testMarbles(name, ts => {
    let e1 = ts |> cold(_cold, ~values=?(coldValues |. Belt.Option.map(gen => gen(ts))));
    ts |> expectObservable(
      e1
      |> ColdObservable.asObservable
      |> operator)
    |> toBeObservable(expected, ~values?);

    ts 
    |> expectSubscriptions(e1 |> ColdObservable.subscriptions)
    |> toBeSubscriptions(subs);
  })
}