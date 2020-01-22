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

let testMarblesWithPreset = (
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