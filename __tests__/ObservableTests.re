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
        ~scheduler=ts |> TestScheduler.asScheduler)));

    let expected = "x-y-(z|)";
    ts |> expectObservable(e1) |> toBeObservable(expected, ~values={"x": 10, "y": 20, "z": 30})    
  });

  testMarbles("generate: should produce all values synchronosly", ts => {
    let source = Rx.generate(
      ~initialState=1,
      ~condition=x=>x<3,
      ~iterate=x=>x+1,
      ~scheduler=ts|.TestScheduler.asScheduler,
      ()
    );

    let expected = "(12|)";
    ts |> expectObservable(source) |> toBeObservable(expected, ~values={"1": 1, "2": 2})
  });

  testMarbles("iif: should subscribe to thenSource when the conditional returns true", ts => {
    let e1 = Rx.iif(
      ~condition=() => true,
      ~trueResult=`Observable(Rx.of1("a")),
      ~falseResult=`Observable(Rx.empty));

    ts |> expectObservable(e1) |> toBeObservable("(a|)");    
  });

  testMarbles("interval: should create an observable emitting periodically", ts => {
    let e1 = Rx.concat([|
      Rx.interval(~period=2, ~scheduler=ts |> TestScheduler.asScheduler, ()) |> Rx.Operators.take(6),
      Rx.never
    |]);

    let expected = "--a-b-c-d-e-f-";
    let values = { "a": 0, "b": 1, "c": 2, "d": 3, "e": 4, "f": 5 };
    ts |> expectObservable(e1) |> toBeObservable(expected, ~values)
  });

  testMarbles("merge: should merge cold and cold", ts => {
    let e1 = ts |> cold("---a-----b-----c----|");
    let e1subs =      [|"^-------------------!"|];
    let e2 = ts |> cold("------x-----y-----z----|");
    let e2subs =      [|"^----------------------!"|];
    let expected =      "---a--x--b--y--c--z----|";

    let result = Rx.merge2S(
      e1 |> ColdObservable.asObservable, 
      e2 |> ColdObservable.asObservable, 
      ~scheduler=ts |> TestScheduler.asScheduler, 
    );

    ts |> expectObservable(result) |> toBeObservable(expected);
    ts |> expectSubscriptions(e1 |> ColdObservable.subscriptions) |> toBeSubscriptions(e1subs);
    ts |> expectSubscriptions(e2 |> ColdObservable.subscriptions) |> toBeSubscriptions(e2subs);
  });

  testMarbles("onErrorResumeNext: should continue observable sequence with next observable", ts => {
    let source = ts |> hot("--a--b--#");
    let next = ts |> cold( "        --c--d--|") |> ColdObservable.asObservable;
    let subs =           [|"^-------!"|];
    let expected =         "--a--b----c--d--|";

    let result = Rx.onErrorResumeNext([|
      source |> HotObservable.asObservable,
      next
    |]);

    ts |> expectObservable(result) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> HotObservable.subscriptions) |> toBeSubscriptions(subs);
  });

  testMarbles("pairs: should create an observable emits key-value pair", ts => {
    let e1 = Rx.pairs({"a": 1, "b": 2}, ~scheduler=ts |> TestScheduler.asScheduler, ())
    |> Rx.Operators.map(((key, value), _idx) => key ++ string_of_int(value));

    let expected = "(ab|)";
    let values = {
      "a": "a1",
      "b": "b2"
    };

    ts |> expectObservable(e1) |> toBeObservable(expected, ~values)
  });

  testMarbles("partition: should partition an observable of integers into even and odd", ts => {
    let e1 = ts |> hot("--1-2---3------4--5---6--|");
    let e1subs =     [|"^------------------------!", 
                       "^------------------------!"|];
    let expected =   [|"--1-----3---------5------|",
                       "----2----------4------6--|"|];

    let (fst, snd) = Rx.partition(e1 |> HotObservable.asObservable, (x, _idx) => x mod 2 == 1);
    ts |> expectObservable(fst) |> toBeObservable(expected[0]);
    ts |> expectObservable(snd) |> toBeObservable(expected[1]);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs)
  });

  testMarbles("race: should race cold and cold", ts => {
    let e1 = ts |> cold("---a-----b-----c----|");
    let e1subs =      [|"^-------------------!"|];
    let e2 = ts |> cold("------x-----y-----z----|");
    let e2subs =      [|"^--!"|];
    let expected =      "---a-----b-----c----|";

    let result = Rx.race([|e1 |> ColdObservable.asObservable, e2 |> ColdObservable.asObservable|]);
    ts |> expectObservable(result) |> toBeObservable(expected);
    ts |> expectSubscriptions(e1 |> ColdObservable.subscriptions) |> toBeSubscriptions(e1subs);
    ts |> expectSubscriptions(e2 |> ColdObservable.subscriptions) |> toBeSubscriptions(e2subs);
  });

  testMarbles("range: should work with two subscribers", ts => {
    let e1 = Rx.range(~start=1, ~count=5, ())
    |> Rx.Operators.concatMap(`Observable((x, idx) => 
      Rx.of1(x)
      |> Rx.Operators.delay(`Int(idx == 0 ? 0 : 2), ~scheduler=ts |> TestScheduler.asScheduler)
    ));

    let expected = "a-b-c-d-(e|)";
    let values = {"a": 1, "b": 2, "c": 3, "d": 4, "e": 5};
    ts |> expectObservable(e1) |> toBeObservable(expected, ~values)
  });
  
  testMarbles("throwError: should create a cold observable that just emits an error", ts => {
    let expected = "#";
    let e1 = Rx.throwError("error", ());
    ts |> expectObservable(e1) |> toBeObservable(expected);
  });

  testMarbles("timer: should schedule a value of 0 then complete", ts => {
    let dueTime = ts |> time("-----|");
    let expected = "-----(x|)";
    let source = Rx.timer(~dueTime=`Int(dueTime), ~scheduler=ts |> TestScheduler.asScheduler, ());
    ts |> expectObservable(source) |> toBeObservable(expected, ~values={"x": 0});
  });

  testMarbles("zip: should combine a source with a second", ts => {
    let a = ts |> hot("---1---2---3---");
    let asubs =    [|"^"|];
    let b = ts |> hot("--4--5--6--7--8--");
    let bsubs =  [|"^"|];
    let expected = "---x---y---z";
    let values = {
      "x": [|"1", "4"|],
      "y": [|"2", "5"|],
      "z": [|"3", "6"|]
    };

    let result = Rx.zip([|a |> HotObservable.asObservable, b |> HotObservable.asObservable|]);
    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(a |> HotObservable.subscriptions) |> toBeSubscriptions(asubs);
    ts |> expectSubscriptions(b |> HotObservable.subscriptions) |> toBeSubscriptions(bsubs);
  });

  testMarbles("scheduled: should schedule a sync observable", ts => {
    let input = Rx.of_([|"a", "b", "c"|]);
    ts 
    |> expectObservable(Rx.scheduled(`Observable(input), ~scheduler=ts |> TestScheduler.asScheduler))
    |> toBeObservable("(abc|)")
  });

  ();
});