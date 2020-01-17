open Rx.Testing;
open Jest;
open Expect;

let testMarbles = (name, callback) => test(name, () => {
  let ts = TestScheduler.create(~assertDeepEqual=BsMocha.Assert.deep_equal);
  ts |> TestScheduler.run(_r => {
    callback(ts)
    expect(true) 
  })
  |> toBe(true)
});