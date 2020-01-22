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
    //spaces between subcribe unsubscribe frames appears to work there but not here...
    let e1subs =      [|"^------------------!"|];
    let e2 = ts |> cold("x-x-x|              ", ~values={"x": 10}) |> ColdObservable.asObservable;
    let expected =      "--x-x-x-y-y-yz-z-z-|";
    let values = {"x": 10, "y": 30, "z": 50};

    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.concatMap((x, _idx) => 
      e2 
      |> Rx.Operators.map((i, _idx) => i * int_of_string(x)
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

    ts
    |> expectObservable(
      e1
      //FIXME: rxjs spec/operators/debounceTime-spec.ts due time is 20 units
      // is there different non-default frame duration set on TestScheduler there?
      |> Rx.Operators.debounceTime(~dueTime=2., ~scheduler=ts |> TestScheduler.asScheduler, ())
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
    |> Rx.Operators.map((x, _idx) => x == "|" 
        ? Rx.Notification.createComplete() 
        : Rx.Notification.createNext(x 
          |> Js.String.replace("{", "") 
          |> Js.String.replace("}", "")))
    |> Rx.Operators.dematerialize();
    
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
      |> Rx.Operators.distinct())
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
      |> Rx.Operators.distinct(~flushes=e2 |> HotObservable.asObservable, ()))
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
    |> Rx.Operators.distinctUntilKeyChanged(~key="k", ());

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
      |> Rx.Operators.endWith([|"s"|]))
    |> toBeObservable(expected)

    ts
    |> expectSubscriptions(e1 |> ColdObservable.subscriptions)
    |> toBeSubscriptions(e1subs)
  });

  testMarbles("every: should return false if only some of element matches with predicate", ts => {
    let e1 = ts |> hot("--a--b--c--d--e--|", ~values={"a": 5, "b": 10, "c": 15, "d": 18, "e": 20});
    let sourceSubs = [|"^----------!      "|];
    let expected =     "-----------(F|)   ";

    ts
    |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx_Operators.every((x, _index, _source) => x mod 5 == 0))
    |> toBeObservable(expected, ~values={"F": false});

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(sourceSubs)
  });

  testMarbles("exhaust: should handle a hot observable of hot observables", ts => {
    let x = ts |> cold(      "--a---b---c--|               ");
    let y = ts |> cold(              "---d--e---f---|      ");
    let z = ts |> cold(                    "---g--h---i---|");
    let e1 = ts 
    |> hot(  "------x-------y-----z-------------|", ~values={ "x": x, "y": y, "z": z }) 
    |> HotObservable.asObservable;

    let expected = "--------a---b---c------g--h---i---|";
    ts 
    |> expectObservable(e1 |> Rx.Operators.exhaust()) 
    |> toBeObservable(expected)
  });

  testMarbles("exhaustMap: should map-and-flatten each item to an Observable", ts => {
    let e1 = ts |> hot("--1-----3--5-------|");
    let e1subs =     [|"^------------------!"|];
    let e2 = ts |> cold("x-x-x|              ", ~values={"x": 10});
    let expected = "--x-x-x-y-y-y------|";
    let values = {"x": 10, "y": 30, "z": 50};

    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.exhaustMap(
      `Observable((x, _idx) => e2 
        |> ColdObservable.asObservable 
        |> Rx.Operators.map((i, _idx) => i * x)));

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs)
  });

  testMarbles("expand: should recursively map-and-flatten each item to an Observable", ts => {
    let e1 = ts |> hot(  "--x----|  ", ~values={"x": 1});
    let e1subs =       [|"^------!  "|];
    let e2 =  ts |> cold(  "--c|    ", ~values={"c": 2});
    let expected =       "--a-b-c-d|";
    let values = {"a": 1, "b": 2, "c": 4, "d": 8};

    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.expand(
      `Observable((x, _idx) => x == 8 
        ? Rx.empty 
        : e2 |> ColdObservable.asObservable |> Rx.Operators.map((c, _idx) => c * x)),
      ())
    
    ts |> expectObservable(result) |> toBeObservable(expected, ~values)
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs)
  });

  testMarbles("filter: should filter out even values", ts => {
    let source = ts |> hot("--0--1--2--3--4--|");
    let subs =           [|"^----------------!"|];
    let expected =         "-----1-----3-----|";

    ts |> expectObservable(
      source 
      |> HotObservable.asObservable
      |> Rx.Operators.filter((x, _idx) => x mod 2 == 1))
    |> toBeObservable(expected);
    ts 
    |> expectSubscriptions(source |> HotObservable.subscriptions)
    |> toBeSubscriptions(subs) 
  });

  test("finalize: should handle basic hot observable", () => {
    open Expect;

    let ts = create(~assertDeepEqual=BsMocha.Assert.deep_equal);
    ts |> run(_r => {
      let executed = ref(false);
      let s1 = ts |> hot( "--a--b--c--|");
      let subs =       [| "^----------!"|];
      let expected =      "--a--b--c--|";

      let result = s1
      |> HotObservable.asObservable
      |> Rx.Operators.finalize(() => { executed := true });

      ts |> expectObservable(result) |> toBeObservable(expected)
      ts |> expectSubscriptions(s1 |> HotObservable.subscriptions) |> toBeSubscriptions(subs);

      // manually flush so `finalize()` has chance to execute before the test is over.
      ts |> flush;
      expect(executed^) 
    })
    |> toBe(true)
  });

  testMarbles("find: should return matching element from source emits single element", ts => {
    let values = {"a": 3, "b": 9, "c": 15, "d": 20};
    let source = ts |> hot("---a--b--c--d---|", ~values);
    let subs =           [|"^--------!       "|];
    let expected =         "---------(c|)    ";

    ts |> expectObservable(
      source 
      |> HotObservable.asObservable
      |> Rx.Operators.find((x, _idx, _src) => x mod 5 == 0))
    |> toBeObservable(expected, ~values);

    ts 
    |> expectSubscriptions(source |> HotObservable.subscriptions) 
    |> toBeSubscriptions(subs)
  });

  testMarbles("findIndex: should return matching element from source emits single element", ts => {
    let values = {"a": 3, "b": 9, "c": 15, "d": 20};
    let source = ts |> hot("---a--b--c--d---|", ~values);
    let subs =           [|"^--------!       "|];
    let expected =         "---------(x|)    ";

    ts |> expectObservable(
      source 
      |> HotObservable.asObservable
      |> Rx.Operators.findIndex((x, _idx, _src) => x mod 5 == 0))
    |> toBeObservable(expected, ~values={"x": 2});

    ts 
    |> expectSubscriptions(source |> HotObservable.subscriptions) 
    |> toBeSubscriptions(subs)
  });

  testMarbles("first: should take the first value of an observable with many values", ts => {
    let e1 = ts |> hot("-----a--b--c---d---|");
    let expected =     "-----(a|)           ";
    let sub =        [|"^----!              "|];

    ts |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx.Operators.first(()))
    |> toBeObservable(expected);

    ts 
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(sub);
  });

  testMarbles("groupBy: should group numbers by odd/even", ts => {
    let e1 = ts |> hot("--1---2---3---4---5---|") |> HotObservable.asObservable;
    let expected =     "--x---y---------------|";
    let x = ts |> cold(  "1-------3-------5---|");
    let y = ts |> cold(      "2-------4-------|");
    let values = { "x": x, "y": y };

    let source = e1 
    |> Rx.Operators.groupBy(x => (int_of_string(x) mod 2) |> string_of_int);

    ts
    |> expectObservable(source)
    |> toBeObservable(expected, ~values)
  });

  testMarblesWithPreset(
    ~name="ignoreElements: should ignore all the elements of the source",
    ~hot=     "--a--b--c--d--|",
    ~expected="--------------|",
    ~subs=  [|"^-------------!"|],
    ~operator=Rx.Operators.ignoreElements(()),
    ()
  );

  testMarblesWithPreset(
    ~name="isEmpty: should return true if source is empty",
    ~hot=     "-----|",
    ~expected="-----(T|)",
    ~subs=  [|"^----!"|],
    ~operator=Rx.Operators.isEmpty(()),
    ~values={"T": true},
    ()
  );

  testMarblesWithPreset(
    ~name="last: should take the first value of an observable with many values",
    ~hot=     "--a----b--c--|",
    ~expected="-------------(c|)",
    ~subs=  [|"^------------!"|],
    ~operator=Rx.Operators.last(()),
    ()
  );

  testMarblesWithPreset(
    ~name="map: should map multiple values",
    ~hot=     "--1--2--3--|",
    ~expected="--x--y--z--|",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.map((x, _idx) => 10 * x),
    ~values={"x": 10, "y": 20, "z": 30},
    ()
  );

  testMarblesWithPreset(
    ~name="mapTo: should map multiple values",
    ~hot=     "--1--2--3--|",
    ~expected="--a--a--a--|",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.mapTo("a"),
    ()
  );

  testMarblesWithPreset(
    ~name="materialize: should materialize a happy stream",
    ~hot=     "--a--b--c--|",
    ~expected="--w--x--y--(z|)",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.materialize(),
    ~values={
      "w": Rx.Notification.createNext("a"),
      "x": Rx.Notification.createNext("b"),
      "y": Rx.Notification.createNext("c"),
      "z": Rx.Notification.createComplete()
    },
    ()
  );

  testMarblesWithPreset(
    ~name="max: should find the max of values of an observable",
    ~hot=     "--a--b--c--|",
    ~hotValues=_ts=>{ "a": 42, "b": -1, "c": 3 },
    ~expected="-----------(x|)",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.max(),
    ~values={"x": 42},
    ()
  );

  testMarblesWithPreset(
    ~name="maxWithPredicate: should handle a constant predicate on observable that throws",
    ~hot=     "-a-^-b--c--d-|",
    ~expected="----------(w|)",
    ~subs=  [|"^---------!"|],
    ~operator=Rx.Operators.maxWithComparer((lhs, rhs) => lhs > rhs ? -1. : 1.),
    ~values={"w": "b"},
    ()
  );

  testMarblesWithPreset(
    ~name="mergeAll: should handle merging a hot observable of observables",
    ~hot="--x--y--|         ",
    ~hotValues=ts=>{
      "x": ts |> cold("a---b---c---|   "),
      "y": ts |> cold("d---e---f---|")
    },
    ~expected="--a--db--ec--f---|",
    ~subs= [|"^-------!---------"|],
    ~operator=Rx.Operators.mergeAll(),
    ()
  );

  testMarbles("mergeMap: should map-and-flatten each item to an Observable", ts => {
    let e1 = ts |> hot("--1-----3--5-------|");
    let e1subs =     [|"^------------------!"|];
    let e2 = ts |> cold("x-x-x|              ", ~values={"x": 10});
    let expected = "--x-x-x-y-yzyz-z---|";
    let values = {"x": 10, "y": 30, "z": 50};

    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.mergeMap(`Observable((x, _idx) => 
      e2 
      |> ColdObservable.asObservable
      |> Rx.Operators.map((i, _idx) => i * x)
    ), ());

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs);
  });

  ()
});