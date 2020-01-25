open Jest;
open Rx.Testing;
open TestUtils;
open TestScheduler;

describe("Observable", () => {
  open Expect;

  test("bindCallback: should emit undefined from a callback without arguments", () => {
    let ts = TestScheduler.create(~assertDeepEqual=BsMocha.Assert.deep_equal);
    let callback = cb => cb();

    let results = [||];
    let boundCallback = Rx.bindCallback(callback |> Obj.magic, ());
    let observable = boundCallback(.);
    observable
    |> Rx.Observable.subscribe(
      ~next=() => 
        results 
        |> Js.Array.push("undefined") 
        |> ignore,
      ~complete=() => 
        results 
        |> Js.Array.push("done") 
        |> ignore)
    |> ignore;

    ts |> TestScheduler.flush;

    expect(
      results[0] == "undefined" &&
      results[1] == "done")
    |> toBe(true);
  });

  testMarbles("combineLatest: should combineLatest the provided observables", ts => {
    let firstSource =  ts |> hot("----a----b----c----|");
    let secondSource = ts |> hot("--d--e--f--g--|");
    let expected =         "----uv--wx-y--z----|";
    let values = {"u": "ad", "v": "ae", "w": "af", "x": "bf", "y": "bg", "z": "cg"};

    let combined = Rx.combineLatest2(
      firstSource |> HotObservable.asObservable,
      secondSource |> HotObservable.asObservable)
    |> Rx.Operators.map(((a, b), _idx) => a ++ b);

    ts |> expectObservable(combined) |> toBeObservable(expected, ~values);
  });

  testMarbles("concat: should emit elements from multiple sources", ts => {
    let e1 = ts |> cold("-a-b-c-|");
    let e1subs =      [|"^------!"|];
    let e2 = ts |> cold("-0-1-|");
    let e2subs =      [|"-------^----!"|];
    let e3 = ts |> cold("-w-x-y-z-|");
    let e3subs = [|"------------^--------!"|];
    let expected = "-a-b-c--0-1--w-x-y-z-|";

    let result = Rx.concat([|
      e1 |> ColdObservable.asObservable,
      e2 |> ColdObservable.asObservable,
      e3 |> ColdObservable.asObservable
    |]);

    ts |> expectObservable(result) |> toBeObservable(expected);
    ts |> expectSubscriptions(e1 |> ColdObservable.subscriptions) |> toBeSubscriptions(e1subs);
    ts |> expectSubscriptions(e2 |> ColdObservable.subscriptions) |> toBeSubscriptions(e2subs);
    ts |> expectSubscriptions(e3 |> ColdObservable.subscriptions) |> toBeSubscriptions(e3subs);
  });

  testMarbles("defer: should create an observable from the provided observable factory", ts => {
    let source = ts |> hot("--a--b--c--|");
    let sourceSubs =     [|"^----------!"|];
    let expected =         "--a--b--c--|"

    let e1 = Rx.defer(`Observable(() => source |> HotObservable.asObservable));
    ts |> expectObservable(e1) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> HotObservable.subscriptions) |> toBeSubscriptions(sourceSubs);
  });

  testMarbles("forkJoin: should emit the last emitted value from each observable", ts => {
    let e1 = ts |> hot("-a--b-----c-d-e-|");
    let e2 = ts |> cold("--1--2-3-4---|   ");
    let e3 = ts |> hot("--------f--g-h-i--j-|");
    let expected = "--------------------(z|)";
    let values = {
      "z": [|"e", "4", "j"|]
    };

    let result = Rx.forkJoin3(
      e1 |> HotObservable.asObservable,
      e2 |> ColdObservable.asObservable,
      e3 |> HotObservable.asObservable
    );

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
  });

  testMarbles("from: should create an observable from an array", ts => {
    let e1 = Rx.from(`Array([|10, 20, 30|]), ())
    |> Rx.Operators.concatMap(`Observable((x, idx) => 
      Rx.of1(x)
      |> Rx.Operators.delay(
        `Int(idx == 0 ? 0 : 2), 
        ~scheduler=ts |> TestScheduler.asScheduler, 
        ())
      )
    );

    let expected = "x-y-(z|)";
    ts |> expectObservable(e1) |> toBeObservable(expected, ~values={"x": 10, "y": 20, "z": 30})    
  });



  ();
});