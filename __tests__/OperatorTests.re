open Jest;
open Rx.Testing;
open TestScheduler;
open TestUtils;

describe("Operators", () => {
  testMarbles("audit: should emit the last value in each time window", ts => {
    let e1 = ts |> hot("     -a-xy-----b--x--cxxx-|");
    let e1subs = [|"         ^--------------------!"|];
    let e2 = ts |> cold("     ----|                ")
    let e2subs = [|
      "                      -^---!                ",
      "                      ----------^---!       ",
      "                      ----------------^---! "
    |];

    let expected = "         -----y--------x-----x|";
    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.audit(~durationSelector=`Subscribable(_ => e2 |> ColdObservable.asObservable));

    ts 
    |> expectObservable(result) 
    |> toBeObservable(expected);

    ts 
    |> expectSubscriptions(e1 |> HotObservable.subscriptions) 
    |> toBeSubscriptions(e1subs);

    ts 
    |> expectSubscriptions(e2 |> ColdObservable.subscriptions) 
    |> toBeSubscriptions(e2subs);
  });

  testMarbles("auditTime: should emit the last value in each time window", ts => {
    let e1 = ts |> hot("-a-x-y----b---x-cx---|");
    let subs = [|"     ^--------------------!"|];
    let expected = "   ------y--------x-----|";
    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.auditTime(~duration=5., ~scheduler=ts |> asScheduler, ());

    ts 
    |> expectObservable(result)
    |> toBeObservable(expected);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(subs);
  });

  testMarbles("buffer: should emit buffers that close and reopen", ts => {
    let a = ts |> hot("-a-b-c-d-e-f-g-h-i-|");
    let b = ts |> hot("-----B-----B-----B-|");
    let expected = "   -----x-----y-----z-|";
    let values = {
      "x": [|"a", "b", "c"|],
      "y": [|"d", "e", "f"|],
      "z": [|"g", "h", "i"|]
    };

    ts 
    |> expectObservable(
      a 
      |> HotObservable.asObservable
      |> Rx.Operators.buffer(b |> HotObservable.asObservable))
    |> toBeObservable(expected, ~values);
  });

  testMarbles("bufferCount: should emit buffers at intervals", ts => {
    let values = {
      "v": [|"a", "b", "c"|],
      "w": [|"c", "d", "e"|],
      "x": [|"e", "f", "g"|],
      "y": [|"g", "h", "i"|],
      "z": [|"i"|]
    };

    let e1 = ts |> hot("--a--b--c--d--e--f--g--h--i--|");
    let expected = "    --------v-----w-----x-----y--(z|)";

    ts
    |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx.Operators.bufferCount(~bufferSize=3, ~startBufferEvery=2, ()))
    |> toBeObservable(expected, ~values);
  });

  testMarbles("bufferTime: should emit buffers at intervals", ts => {
    let e1 = ts |> hot(   "---a---b---c---d---e---f---g-----|   ");
    let subs = [|"         ^--------------------------------!   "|];
    let time = ts |> time("----------|                          ");
    let expected =        "----------w---------x---------y--(z|)";
    let values = {
      "w": [|"a", "b"|],
      "x": [|"c", "d", "e"|],
      "y": [|"f", "g"|],
      "z": [||]
    };

    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.bufferTime(
      ~timeSpan=time, 
      ~maxBufferSize=Js.Int.max, 
      ~scheduler=ts |> TestScheduler.asScheduler, ());

    ts
    |> expectObservable(result)
    |> toBeObservable(expected, ~values);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(subs)
  });

  testMarbles("bufferToggle: should emit buffers using hot openings and hot closings", ts => {
    let e1 = ts |> hot("  ---a---b---c---d---e---f---g---|") |> HotObservable.asObservable;
    let e2 = ts |> hot("  --o------------------o---------|") |> HotObservable.asObservable;
    let e3 = ts |> hot("  ---------c---------------c-----|") |> HotObservable.asObservable;
    let expected =       "---------x---------------y-----|";
    let values = {
      "x": [|"a", "b"|],
      "y": [|"f"|]
    };

    let result = e1
    |> Rx.Operators.bufferToggle(~opening=`Subscribable(e2), ~closing=`Subscribable(_ => e3));

    ts 
    |> expectObservable(result) 
    |> toBeObservable(expected, ~values);
  });
  
  testMarbles("bufferWhen: should emit buffers that close and reopen", ts => {
    let e1 = ts |> hot("--a--^---b---c---d---e---f---g---------|   ") |> HotObservable.asObservable;
    let e2 = ts |> cold("    --------------(s|)                    ") |> ColdObservable.asObservable;
    let expected = "         --------------x-------------y-----(z|)";
    let values = {
      "x": [|"b", "c", "d"|],
      "y": [|"e", "f", "g"|],
      "z": [||]
    };

    ts
    |> expectObservable(e1 |> Rx.Operators.bufferWhen(() => e2))
    |> toBeObservable(expected, ~values);
  });

  testMarbles("catchError: should catch error and replace with a cold Observable", ts => {
    let e1 = ts |> hot("  --a--b--#       ") |> HotObservable.asObservable;
    let e2 = ts |> cold("         -1-2-3-|") |> ColdObservable.asObservable;
    let expected = "      --a--b---1-2-3-|";

    let result = e1 |> Rx.Operators.catchError((_e, _obs) => e2);
    ts
    |> expectObservable(result)
    |> toBeObservable(expected);
  });

  testMarbles("combineAll: should combine events from two observables", ts => {
    let x = ts |> cold("                  -a-----b---|") |> ColdObservable.asObservable;
    let y = ts |> cold("                  --1-2-|     ") |> ColdObservable.asObservable;
    let outer = ts |> hot("-x----y--------|           ", ~values={"x": x, "y": y}) |> HotObservable.asObservable;
    let expected = "       -----------------A-B--C---|";

    let result = outer |> Rx.Operators.combineAllProject(v => { v[0] ++ v[1] });
    ts
    |> expectObservable(result)
    |> toBeObservable(expected, ~values={ "A": "a1", "B": "a2", "C": "b2"});
  });

  testMarbles("concatAll: should concat an observable of observables", ts => {
    let x = ts |> cold(    "----a------b------|                 ");
    let y = ts |> cold(                      "---c-d---|        ");
    let z = ts |> cold(                               "---e--f-|");
    let outer = ts |> hot("-x---y----z------|              ", ~values={ "x": x, "y": y, "z": z }) |> HotObservable.asObservable;
    let expected = "-----a------b---------c-d------e--f-|";
    ts 
    |> expectObservable(outer |> Rx.Operators.concatAll())
    |> toBeObservable(expected)
  });

  testMarbles("concatMap: should map-and-flatten each item to an Observable", ts => {
    let e1 = ts |> hot( "--1-----3--5-------|");
    //FIXME: in rxjs unsubscribe is in 1st (when spaces between sub unsub) frame - weird
    let e1subs =      [|"^------------------!"|];
    let e2 = ts |> cold("x-x-x|              ", ~values={"x": 10}) |> ColdObservable.asObservable;
    let expected =      "--x-x-x-y-y-yz-z-z-|";
    let values = {"x": 10, "y": 30, "z": 50};

    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.concatMap((x, _idx) => 
      e2 
      |> Rx.Operators.map(i => i * int_of_string(x)
    ));
    
    ts
    |> expectObservable(result)
    |> toBeObservable(expected, ~values);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(e1subs);
  });

  testMarbles("concatMapTo: should map-and-flatten each item to an Observable", ts => {
    let e1 = ts |> hot("--1-----3--5-------|");
    let e1subs =     [|"^------------------!"|];
    let e2 =  ts |> cold("x-x-x|              ", ~values={"x": 10}) |> ColdObservable.asObservable;
    let expected = "--x-x-x-x-x-xx-x-x-|";
    let values = {"x": 10};

    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.concatMapTo(e2);

    ts
    |> expectObservable(result)
    |> toBeObservable(expected, ~values);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(e1subs)
  });

  /*
  //NOTE: not yet available in 7.0
  testMarbles("concatWith: should concatenate two cold observables", ts => {
    let e1 = ts |> cold(" --a--b-|") |> ColdObservable.asObservable;
    let e2 = ts |> cold("        --x---y--|") |> ColdObservable.asObservable;
    let expected =       "--a--b---x---y--|";

    let result = e1 |> Rx.Operators.concatWith([|e2|]);
    ts
    |> expectObservable(result)
    |> toBeObservable(expected)
  });
  */

  testMarbles("count: should count the values of an observable", ts => {
    let source = ts |> hot("--a--b--c--|");
    let subs =           [|"^----------!"|];
    let expected =         "-----------(x|)";

    let result = source 
    |> HotObservable.asObservable 
    |> Rx.Operators.count();

    ts 
    |> expectObservable(result)
    |> toBeObservable(expected, ~values={"x": 3});

    ts
    |> expectSubscriptions(source |> HotObservable.subscriptions)
    |> toBeSubscriptions(subs);
  });

  testMarbles("debounce: should debounce values by a specified cold Observable", ts => {
    let e1 = ts |> hot("-a--bc--d---|") |> HotObservable.asObservable;
    let e2 = ts |> cold("--|          ") |> ColdObservable.asObservable;
    let expected = "---a---c--d-|";

    let result = e1
    |> Rx.Operators.debounce(~durationSelector=`Subscribable(_ => e2));

    ts
    |> expectObservable(result)
    |> toBeObservable(expected);
  });

  testMarbles("debounceTime: should debounce values by 20 time units", ts => {
    let e1 = ts |> hot("-a--bc--d---|") |> HotObservable.asObservable;
    let expected =     "---a---c--d-|";

    //FIXME: fails
    ts
    |> expectObservable(
      e1
      |> Rx.Operators.debounceTime(~dueTime=float_of_int(TestScheduler.frameTimeFactor), ~scheduler=ts |> TestScheduler.asScheduler)
    )
    |> toBeObservable(expected)
  });

  testMarbles("defaultIfEmpty: should return the Observable if not empty with a default value", ts => {
    let e1 = ts |> hot("--------|") |> HotObservable.asObservable;
    let expected =     "--------(x|)";

    ts
    |> expectObservable(e1 |> Rx.Operators.defaultIfEmpty(42))
    |> toBeObservable(expected, ~values={"x": 42})
  });

  testMarbles("delay: should delay by specified timeframe", ts => {
    let e1 = ts |> hot("---a--b--|  ");
    let t = ts |> time(   "--|      ");
    let expected = "-----a--b--|";
    let subs =    [|"^--------!  "|];

    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.delay(`Int(t), ~scheduler=ts |> TestScheduler.asScheduler, ());

    ts
    |> expectObservable(result)
    |> toBeObservable(expected);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(subs)
  });

  testMarbles("delayWhen: should delay by duration selector", ts => {
    let e1 =   ts |>hot("---a---b---c--|");
    let expected =      "-----a------c----(b|)";
    let subs =        [|"^-------------!"|];
    let selector = [|
      ts |> cold(       "--x--|"),
      ts |> cold(       "----------(x|)"),
      ts |> cold(       "-x--|")
    |];
    let selectorSubs = [|
      [|"---^-!------------"|],
      [|"-------^---------!"|],
      [|"-----------^!-----"|]
    |];

    let idx = ref(0)
    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.delayWhen(
      ~delayDurationSelector=(_x, _c) => {
        let res = selector[idx^] |> ColdObservable.asObservable
        idx := idx^ + 1;
        res
      }, 
      ());
    
    ts |> expectObservable(result) |> toBeObservable(expected)
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(subs)

    Belt.Array.range(0, Array.length(selector) - 1)
    |. Belt.Array.forEach(idx => 
      ts |> expectSubscriptions(selector[idx] |> ColdObservable.subscriptions) |> toBeSubscriptions(selectorSubs[idx])
    );
  });

  testMarbles("demeterialize: should dematerialize an Observable", ts => {
    let values = {
      "a": "{x}",
      "b": "{y}",
      "c": "{z}",
      "d": "|"
    };

    let e1 = ts |> hot("--a--b--c--d-", ~values) |> HotObservable.asObservable;
    let expected = "--x--y--z--|";
    let result = e1
    |> Rx_Operators.map(x => x == "|" 
        ? Rx_Notification.createComplete() 
        : Rx_Notification.createNext(x 
          |> Js.String.replace("{", "") 
          |> Js.String.replace("}", "")))
    |> Rx_Operators.dematerialize();
    
    ts
    |> expectObservable(result) 
    |> toBeObservable(expected)
  });

  testMarbles("distinct: should destinguish between values", ts => {
    let e1 = ts |> hot("--a--a--a--b--b--a--|");
    let e1subs =     [|"^-------------------!"|];
    let expected =     "--a--------b--------|";

    ts 
    |> expectObservable(
      e1 
      |> HotObservable.asObservable
      |> Rx_Operators.distinct())
    |> toBeObservable(expected);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions) 
    |> toBeSubscriptions(e1subs);
  });

  testMarbles("distinct: should distinguish values by key", ts => {
    let values = { "a": 1, "b": 2, "c": 3, "d": 4, "e": 5, "f": 6};
    let e1 = ts |> hot("--a--b--c--d--e--f--|", ~values);
    let e1subs =     [|"^-------------------!"|];
    let expected =     "--a--b--c-----------|";
    
    ts 
    |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx_Operators.distinct(~keySelector=value => value mod 3, ()))
    |> toBeObservable(expected, ~values);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(e1subs);
  });

  testMarbles("distinct: should support a flushing stream", ts => {
    let e1 = ts |> hot("--a--b--a--b--a--b--|");
    let e1subs =     [|"^-------------------!"|];
    let e2 = ts |> hot("-----------x--------|");
    let e2subs =     [|"^-------------------!"|];
    let expected =     "--a--b--------a--b--|";

    ts
    |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx_Operators.distinct(~flushes=e2 |> HotObservable.asObservable, ()))
    |> toBeObservable(expected);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(e1subs);

    ts
    |> expectSubscriptions(e2 |> HotObservable.subscriptions)
    |> toBeSubscriptions(e2subs);
  });

  testMarbles("distinctUntilChanged: should distinguish between values", ts => {
    let e1 = ts |> hot("-1--2-2----1-3-|") |> HotObservable.asObservable;
    let expected =     "-1--2------1-3-|";

    ts
    |> expectObservable(
      e1
      |> Rx_Operators.distinctUntilChanged())
    |> toBeObservable(expected)
  });  

  testMarbles("distinctUntilKeyChanged: should distinguish between values", ts => {
    let values = {"a": {"k": 1}, "b": {"k": 2}, "c": {"k": 3}};
    let e1 = ts |> hot("-a--b-b----a-c-|", ~values) |> HotObservable.asObservable;
    let expected =     "-a--b------a-c-|";

    let result = e1
    |> Rx_Operators.distinctUntilKeyChanged(~key="k", ());

    ts
    |> expectObservable(result)
    |> toBeObservable(expected, ~values)
  });

  testMarbles("elementAt: hould return last element by zero-based index", ts => {
    let source = ts |> hot("--a--b--c-d---|")
    let subs =           [|"^-------!"|];
    let expected =         "--------(c|)";

    ts
    |> expectObservable(
      source |> HotObservable.asObservable
      |> Rx_Operators.elementAt(2, ()))
    |> toBeObservable(expected)

    ts
    |> expectSubscriptions(source |> HotObservable.subscriptions)
    |> toBeSubscriptions(subs)
  });

  testMarbles("endWith: should append to a cold Observable", ts => {
    let e1 = ts |> cold("---a--b--c--|");
    let e1subs =      [|"^-----------!"|];
    let expected =     "---a--b--c--(s|)";

    ts
    |> expectObservable(
      e1 
      |> ColdObservable.asObservable
      |> Rx_Operators.endWith([|"s"|]))
    |> toBeObservable(expected)

    ts
    |> expectSubscriptions(e1 |> ColdObservable.subscriptions)
    |> toBeSubscriptions(e1subs)
  });

  ()
});