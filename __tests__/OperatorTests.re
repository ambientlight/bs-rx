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
    |> Rx.Operators.audit(`Subscribable(_ => e2 |> ColdObservable.asObservable));
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
    |> Rx.Operators.auditTime(~duration=5, ~scheduler=ts |> asScheduler, ());

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
    |> Rx.Operators.concatMap(`Observable((x, _idx) => 
      e2 
      |> Rx.Operators.map((i, _idx) => i * int_of_string(x)
    )));
    
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
    |> Rx.Operators.concatMapTo(`Observable(e2));

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

  testMarblesWithHotPreset(
    ~name="ignoreElements: should ignore all the elements of the source",
    ~hot=     "--a--b--c--d--|",
    ~expected="--------------|",
    ~subs=  [|"^-------------!"|],
    ~operator=Rx.Operators.ignoreElements(()),
    ()
  );

  testMarblesWithHotPreset(
    ~name="isEmpty: should return true if source is empty",
    ~hot=     "-----|",
    ~expected="-----(T|)",
    ~subs=  [|"^----!"|],
    ~operator=Rx.Operators.isEmpty(()),
    ~values={"T": true},
    ()
  );

  testMarblesWithHotPreset(
    ~name="last: should take the first value of an observable with many values",
    ~hot=     "--a----b--c--|",
    ~expected="-------------(c|)",
    ~subs=  [|"^------------!"|],
    ~operator=Rx.Operators.last(()),
    ()
  );

  testMarblesWithHotPreset(
    ~name="map: should map multiple values",
    ~hot=     "--1--2--3--|",
    ~expected="--x--y--z--|",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.map((x, _idx) => 10 * x),
    ~values={"x": 10, "y": 20, "z": 30},
    ()
  );

  testMarblesWithHotPreset(
    ~name="mapTo: should map multiple values",
    ~hot=     "--1--2--3--|",
    ~expected="--a--a--a--|",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.mapTo("a"),
    ()
  );

  testMarblesWithHotPreset(
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

  testMarblesWithHotPreset(
    ~name="max: should find the max of values of an observable",
    ~hot=     "--a--b--c--|",
    ~hotValues=_ts=>{ "a": 42, "b": -1, "c": 3 },
    ~expected="-----------(x|)",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.max(),
    ~values={"x": 42},
    ()
  );

  testMarblesWithHotPreset(
    ~name="maxWithPredicate: should handle a letant predicate on observable that throws",
    ~hot=     "-a-^-b--c--d-|",
    ~expected="----------(w|)",
    ~subs=  [|"^---------!"|],
    ~operator=Rx.Operators.maxWithComparer((lhs, rhs) => lhs > rhs ? -1. : 1.),
    ~values={"w": "b"},
    ()
  );

  testMarblesWithHotPreset(
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

  testMarbles("mergeMapTo: should map-and-flatten each item to an Observable", ts => {
    let e1 = ts |> hot("--1-----3--5-------|");
    let e1subs = [|"^------------------!"|];
    let e2 = ts |> cold("x-x-x|              ", ~values={"x": 10});
    let expected = "--x-x-x-x-xxxx-x---|";

    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.mergeMapTo(`Observable(e2|>ColdObservable.asObservable), ());

    ts |> expectObservable(result) |> toBeObservable(expected, ~values={"x": 10});
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs)
  });

  testMarblesWithHotPreset(
    ~name="mergeScan: should mergeScan things",
    ~hot="--a--^--b--c--d--e--f--g--|",
    ~expected="---u--v--w--x--y--z--|",
    ~subs=  [|"^--------------------!"|],
    ~operator=Rx.Operators.mergeScan(`Observable((acc, x, _idx) => Rx.of1(Array.concat([acc, [|x|]]))), ~seed=[||], ()),
    ~values={
      "u": [|"b"|],
      "v": [|"b", "c"|],
      "w": [|"b", "c", "d"|],
      "x": [|"b", "c", "d", "e"|],
      "y": [|"b", "c", "d", "e", "f"|],
      "z": [|"b", "c", "d", "e", "f", "g"|]
    },
    ()
  );

  testMarblesWithHotPreset(
    ~name="min: should find the min of values of an observable",
    ~hot=     "--a--b--c--|",
    ~hotValues=_ts=>{ "a": 42, "b": -1, "c": 3 },
    ~expected="-----------(x|)",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.min(),
    ~values={"x": -1},
    ()
  );

  testMarblesWithHotPreset(
    ~name="minWithPredicate: should handle a letant predicate on observable that throws",
    ~hot=     "-a-^-b--c--d-|",
    ~expected="----------(w|)",
    ~subs=  [|"^---------!"|],
    ~operator=Rx.Operators.minWithComparer((lhs, rhs) => lhs > rhs ? -1. : 1.),
    ~values={"w": "d"},
    ()
  );

  testMarbles("multicast: should mirror a simple source Observable", ts => {
    let source = ts |> cold("--1-2---3-4--5-|");
    let sourceSubs =      [|"^--------------!"|];
    let multicasted = source 
    |> ColdObservable.asObservable
    |> Rx.Operators.multicast(`SubjectFactory(() => Rx.Subject.create()), ());
    let expected =    "--1-2---3-4--5-|";

    ts |> expectObservable(multicasted |> Rx.ConnectableObservable.asObservable) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> ColdObservable.subscriptions) |> toBeSubscriptions(sourceSubs)

    multicasted |> Rx.ConnectableObservable.connect;
  });

  testMarbles("observeOn: should observe on specified scheduler", ts => {
    let e1 = ts |> hot("--a--b--|");
    let expected =     "--a--b--|";
    let sub =        [|"^-------!"|];

    ts |> expectObservable(
      e1 
      |> HotObservable.asObservable
      |> Rx.Operators.observeOn(ts |> TestScheduler.asScheduler, ())) 
    |> toBeObservable(expected);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(sub);
  });

  testMarbles("onErrorResumeNext: should continue observable sequence with next observable", ts => {
    let source = ts |> hot("--a--b--#");
    let next = ts |> cold( "        --c--d--|") |> ColdObservable.asObservable;
    let subs =           [|"^-------!"|];
    let expected =         "--a--b----c--d--|";

    ts |> expectObservable(
      source
      |> HotObservable.asObservable
      |> Rx.Operators.onErrorResumeNext(`Observable([|next|]))) 
    |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> HotObservable.subscriptions) |> toBeSubscriptions(subs);
  });

  testMarblesWithHotPreset(
    ~name="pairwise: should group consecutive emissions as arrays of two",
    ~hot=     "--a--^--b--c--d--e--f--g--|",
    ~expected=     "------v--w--x--y--z--|",
    ~subs=       [|"^--------------------!"|],
    ~operator=Rx.Operators.pairwise(),
    ~values={
      "v": [|"b", "c"|],
      "w": [|"c", "d"|],
      "x": [|"d", "e"|],
      "y": [|"e", "f"|],
      "z": [|"f", "g"|]
    },
    ()
  );

  testMarbles("partition: should partition an observable of integers into even and odd", ts => {
    let e1 = ts |> hot("--1-2---3------4--5---6--|");
    let e1subs =     [|"^------------------------!", 
                       "^------------------------!"|];
    let expected =   [|"--1-----3---------5------|",
                       "----2----------4------6--|"|];

    let (fst, snd) = e1
    |> HotObservable.asObservable
    |> Rx.Operators.partition((x, _idx) => x mod 2 == 1);

    ts |> expectObservable(fst) |> toBeObservable(expected[0]);
    ts |> expectObservable(snd) |> toBeObservable(expected[1]);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs)
  });

  testMarbles("publish: should mirror a simple source Observable", ts => {
    let source = ts |> cold("--1-2---3-4--5-|");
    let sourceSubs =      [|"^--------------!"|];
    let published = source 
    |> ColdObservable.asObservable
    |> Rx.Operators.publish();
    let expected =    "--1-2---3-4--5-|";

    ts |> expectObservable(published |> Rx.ConnectableObservable.asObservable) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> ColdObservable.subscriptions) |> toBeSubscriptions(sourceSubs)

    published |> Rx.ConnectableObservable.connect;
  });

  testMarbles("publishBehavior: should mirror a simple source Observable", ts => {
    let source = ts |> cold("--1-2---3-4--5-|");
    let sourceSubs =      [|"^--------------!"|];
    let published = source 
    |> ColdObservable.asObservable
    |> Rx.Operators.publishBehavior(0);
    let expected =    "0-1-2---3-4--5-|";

    ts |> expectObservable(published |> Rx.ConnectableObservable.asObservable) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> ColdObservable.subscriptions) |> toBeSubscriptions(sourceSubs)

    published |> Rx.ConnectableObservable.connect;
  });

  testMarbles("publishLast: should emit last notification of a simple source Observable", ts => {
    let source = ts |> cold("--1-2---3-4--5-|");
    let sourceSubs =      [|"^--------------!"|];
    let published = source 
    |> ColdObservable.asObservable
    |> Rx.Operators.publishLast();
    let expected =          "---------------(5|)";

    ts |> expectObservable(published |> Rx.ConnectableObservable.asObservable) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> ColdObservable.subscriptions) |> toBeSubscriptions(sourceSubs)

    published |> Rx.ConnectableObservable.connect;
  });

  testMarbles("publishReplay: should mirror a simple source Observable", ts => {
    let source = ts |> cold("--1-2---3-4--5-|");
    let sourceSubs =      [|"^--------------!"|];
    let published = source 
    |> ColdObservable.asObservable
    |> Rx.Operators.publishReplay(~bufferSize=1, ());
    let expected =          "--1-2---3-4--5-|";

    ts |> expectObservable(published |> Rx.ConnectableObservable.asObservable) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> ColdObservable.subscriptions) |> toBeSubscriptions(sourceSubs)

    published |> Rx.ConnectableObservable.connect;
  });

  testMarblesWithHotPreset(
    ~name="reduce: should reduce",
    ~hot=     "--a--b--c--|",
    ~hotValues=_ts=>{
      "a": 1, "b": 3, "c": 5, "x": 9
    },
    ~expected="-----------(x|)",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.reduce((o, x, _idx) => o + x, 0),
    ~values={
      "a": 1, "b": 3, "c": 5, "x": 9
    },
    ()
  );

  testMarbles("refCount: should turn a multicasted Observable an automatically disconnecting a hot one", ts => {
    let source = ts |> cold("--1-2---3-4--5-|");
    let sourceSubs =      [|"^--------------!"|];
    let published = source 
    |> ColdObservable.asObservable
    |> Rx.Operators.publish()
    |> Rx.Operators.refCount();
    let expected =          "--1-2---3-4--5-|";

    ts |> expectObservable(published) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> ColdObservable.subscriptions) |> toBeSubscriptions(sourceSubs)
  });

  testMarblesWithColdPreset(
    ~name="repeat: should resubscribe count number of times",
    ~cold=    "--a--b--|",
    ~expected="--a--b----a--b----a--b--|",
    ~subs=  [|"^-------!----------------",
              "--------^-------!-------",
              "----------------^-------!"|],
    ~operator=Rx.Operators.repeat(~count=3, ()),
    ()
  );

  testMarbles("repeatWhen: should handle a source with eventual complete using a hot notifier", ts => {
    let source = ts |>  cold("-1--2--|");
    let notifier = ts |> hot("-------------r------------r-|");
    let expected =           "-1--2---------1--2---------1--2--|";

    let result = source 
    |> ColdObservable.asObservable
    |> Rx_Operators.repeatWhen(_notif => notifier |> HotObservable.asObservable);

    ts |> expectObservable(result) |> toBeObservable(expected);
  }); 

  testMarblesWithColdPreset(
    ~name="retry: should handle a basic source that emits next then errors",
    ~cold=    "--a--b--#",
    ~expected="--a--b----a--b----a--b--#",
    ~subs=  [|"^-------!----------------",
              "--------^-------!-------",
              "----------------^-------!"|],
    ~operator=Rx.Operators.retry(~count=2, ()),
    ()
  );

  testMarbles("retryWhen: should handle a source with eventual complete using a hot notifier", ts => {
    let source = ts |>  cold("-1--2--#");
    let notifier = ts |> hot("-----------r-------r---------#");
    let expected =           "-1--2-------1--2----1--2-----#";

    let result = source 
    |> ColdObservable.asObservable
    |> Rx_Operators.retryWhen(_notif => notifier |> HotObservable.asObservable);

    ts |> expectObservable(result) |> toBeObservable(expected);
  });

  testMarbles("sample: should get samples when the notifier emits", ts => {
    let e1 = ts |> hot("---a----b---c----------d-----|   ");
    let e1subs =     [|"^----------------------------!   "|];
    let e2 = ts |> hot("-----x----------x---x------x---|");
    let e2subs = [|"^----------------------------!   "|];
    let expected = "-----a----------c----------d-|   ";

    ts
    |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx.Operators.sample(e2 |> HotObservable.asObservable))
    |> toBeObservable(expected)
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs)
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e2subs)
  });

  testMarbles("sampleTime: should get samples on a delay", ts => {
    let e1 = ts |> hot("a---b-c---------d--e---f-g-h--|");
    let e1subs = [|"^-----------------------------!"|];
    let expected = "-------c-------------e------h-|";

    ts
    |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx.Operators.sampleTime(7, ~scheduler=ts |> TestScheduler.asScheduler, ()))
    |> toBeObservable(expected)

    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs)
  });

  testMarbles("sequnceEqual: should return true for two sync observables that match", ts => {
    let s1 = ts |> cold("(abcdefg|)")
    let s2 = ts |> cold("(abcdefg|)")
    let expected = "(T|)";

    let result = s1
    |> ColdObservable.asObservable
    |> Rx.Operators.sequenceEqual(s2 |> ColdObservable.asObservable, ());
    ts |> expectObservable(result) |> toBeObservable(expected, ~values={"T": true});
  });

  testMarbles("share: should mirror a simple source Observable", ts => {
    let source = ts |> cold("--1-2---3-4--5-|");
    let sourceSubs =      [|"^--------------!"|];
    let shared = source 
    |> ColdObservable.asObservable
    |> Rx.Operators.share();
    let expected =    "--1-2---3-4--5-|";

    ts |> expectObservable(shared) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> ColdObservable.subscriptions) |> toBeSubscriptions(sourceSubs)
  });

  testMarbles("shareReplay: should mirror a simple source Observable", ts => {
    let source = ts |> cold("--1-2---3-4--5-|");
    let sourceSubs =      [|"^--------------!"|];
    let shared = source 
    |> ColdObservable.asObservable
    |> Rx.Operators.shareReplay();
    let expected =    "--1-2---3-4--5-|";

    ts |> expectObservable(shared) |> toBeObservable(expected);
    ts |> expectSubscriptions(source |> ColdObservable.subscriptions) |> toBeSubscriptions(sourceSubs)
  });

  testMarbles("single: should raise error from empty predicate if observable emits multiple time", ts => {
    let e1 = ts |> hot("--a--b--c--|");
    let sub =     [|"^----!     "|];
    let expected =  "^----#     ";

    ts |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx.Operators.single(()))
    |> toBeObservable(expected, ~errorValue="Sequence contains more than one element");

    ts 
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(sub);
  });

  testMarblesWithHotPreset(
    ~name="skip: should skip values before a total",
    ~hot=     "--a--b--c--d--e--|",
    ~expected="-----------d--e--|",
    ~subs=  [|"^----------------!"|],
    ~operator=Rx.Operators.skip(3),
    ()
  );

  testMarblesWithHotPreset(
    ~name="skipLast: should skip two values of an observable with many values",
    ~hot=     "--a--b--c--d--e--|",
    ~expected="-----------a--b--|",
    ~subs=  [|"^----------------!"|],
    ~operator=Rx.Operators.skipLast(3),
    ()
  );

  testMarbles("skipUntil: should skip values until another observable notifies", ts => {
    let e1 = ts |> hot("--a--b--c--d--e----|");
    let e1subs =     [|"^------------------!"|];
    let skip = ts |> hot("---------x------|   ");
    let skipSubs = [|"^--------!---------"|];
    let expected =   "-----------d--e----|";

    ts
    |> expectObservable(
      e1
      |> HotObservable.asObservable
      |> Rx.Operators.skipUntil(skip |> HotObservable.asObservable))
    |> toBeObservable(expected);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs);
    ts |> expectSubscriptions(skip |> HotObservable.subscriptions) |> toBeSubscriptions(skipSubs)
  });

  testMarblesWithHotPreset(
    ~name="skipWhile: should skip all elements until predicate is false",
    ~hot=     "--1--2--3--4--5--|",
    ~expected="-----------4--5--|",
    ~subs=  [|"^----------------!"|],
    ~operator=Rx.Operators.skipWhile((v, _idx) => v < 4),
    ()
  );

  testMarblesWithColdPreset(
    ~name="startWith: should prepend to a cold Observable",
    ~cold=    "--1--2--3--4--5--|",
    ~expected="0-1--2--3--4--5--|",
    ~subs=  [|"^----------------!"|],
    ~operator=Rx.Operators.startWith([|0|]),
    ()
  );

  testMarbles("subscribeOn: should observe on specified scheduler", ts => {
    let e1 = ts |> hot("--a--b--|");
    let expected =     "--a--b--|";
    let sub =        [|"^-------!"|];

    ts |> expectObservable(
      e1 
      |> HotObservable.asObservable
      |> Rx.Operators.subscribeOn(ts |> TestScheduler.asScheduler, ())) 
    |> toBeObservable(expected);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(sub);
  });

  testMarbles("switchMap: should map-and-flatten each item to an Observable", ts => {
    let e1 = ts |> hot( "--1-----3--5-------|");
    let e1subs =      [|"^------------------!"|];
    let e2 = ts |> cold("x-x-x|              ", ~values={"x": 10}) |> ColdObservable.asObservable;
    let expected =      "--x-x-x-y-yz-z-z---|";
    let values = {"x": 10, "y": 30, "z": 50};

    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.switchMap((x, _idx) => 
      e2 
      |> Rx.Operators.map((i, _idx) => i * x
    ));
    
    ts
    |> expectObservable(result)
    |> toBeObservable(expected, ~values);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(e1subs);
  });

  testMarbles("switchMapTo: should map-and-flatten each item to an Observable", ts => {
    let e1 = ts |> hot("--1-----3--5-------|");
    let e1subs =     [|"^------------------!"|];
    let e2 =  ts |> cold("x-x-x|              ", ~values={"x": 10}) |> ColdObservable.asObservable;
    let expected = "--x-x-x-x-xx-x-x---|";
    let values = {"x": 10};

    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.switchMapTo(`Observable(e2));

    ts
    |> expectObservable(result)
    |> toBeObservable(expected, ~values);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(e1subs)
  });

  testMarblesWithHotPreset(
    ~name="take: should take two values of an observable with many values",
    ~hot=     "--a--b--c--d--e--|",
    ~expected="--a--(b|)",
    ~subs=  [|"^----!------------"|],
    ~operator=Rx.Operators.take(2),
    ()
  );

  testMarblesWithHotPreset(
    ~name="takeLast: should take two values of an observable with many values",
    ~hot=     "--a--b--c--d--e--|",
    ~expected="-----------------(de|)",
    ~subs=  [|"^----------------!"|],
    ~operator=Rx.Operators.takeLast(2),
    ()
  );

  testMarbles("takeUntil: should take values until notifier emits", ts => {
    let e1 = ts |> hot("--a--b--c--d--e--f--g--|");
    let e1subs =     [|"^------------!----------"|];
    let e2 = ts |> hot("-------------z--|       ");
    let e2subs =     [|"^------------!----------"|];
    let expected =     "--a--b--c--d-|";

    ts |> expectObservable(
      e1 
      |> HotObservable.asObservable
      |> Rx.Operators.takeUntil(e2 |> HotObservable.asObservable)) 
    |> toBeObservable(expected);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs);
    ts |> expectSubscriptions(e2 |> HotObservable.subscriptions) |> toBeSubscriptions(e2subs);
  });

  testMarblesWithHotPreset(
    ~name="takeWhile: should take all elements until predicate is false",
    ~hot=     "--1--2--3--4--5--|",
    ~expected="--1--2--3--|      ",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.takeWhile((x, _idx) => x < 4, ()),
    ()
  );

  testMarblesWithHotPreset(
    ~name="tap: should mirror multiple values and complete",
    ~hot=     "--1--2--3--|",
    ~expected="--1--2--3--|",
    ~subs=  [|"^----------!"|],
    ~operator=Rx.Operators.tap(~next=_value => (), ()),
    ()
  );

  testMarbles("throttle: should immediately emit the first value in each time window", ts => {
    let e1 = ts |> hot("     -a-xy-----b--x--cxxx-|");
    let e1subs = [|"         ^--------------------!"|];
    let e2 = ts |> cold("     ----|                ")
    let e2subs = [|
      "                      -^---!                ",
      "                      ----------^---!       ",
      "                      ----------------^---! "
    |];

    let expected = "         -a--------b-----c----|";
    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.throttle(`Subscribable(_ => e2 |> ColdObservable.asObservable), ());
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
  
  testMarbles("throttleTime: should immediately emit the first value in each time window", ts => {
    let e1 = ts |> hot("-a-x-y----b---x-cx---|");
    let subs = [|"     ^--------------------!"|];
    let expected = "   -a--------b-----c----|";
    let result = e1 
    |> HotObservable.asObservable
    |> Rx.Operators.throttleTime(~duration=5, ~scheduler=ts |> asScheduler, ());

    ts 
    |> expectObservable(result)
    |> toBeObservable(expected);

    ts
    |> expectSubscriptions(e1 |> HotObservable.subscriptions)
    |> toBeSubscriptions(subs);
  });

  testMarbles("timeInterval: should record the time interval between source elements", ts => {
    let e1 = ts |> hot("--a--^b-c-----d--e--|");
    let e1subs =    [|"^--------------!"|];
    let expected =    "-w-x-----y--z--|";
    let values = { "w": 1, "x": 2, "y": 6, "z": 3 };

    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.timeInterval(~scheduler=ts|.TestScheduler.asScheduler, ())
    |> Rx.Operators.map((x, _idx) => x |. Rx.TimeInterval.intervalGet);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs);
  });

  testMarbles("timeout: should timeout after a specified timeout period", ts => {
    let e1 = ts |> cold("-------a--b--|");
    let e1subs =      [|"^----!        "|];
    let expected =      "-----#        ";
    
    let timeoutError = Rx.TimeoutError.create();
    
    let result = e1
    |> ColdObservable.asObservable
    |> Rx.Operators.timeout(`Number(5), ~scheduler=ts|.TestScheduler.asScheduler, ());

    ts |> expectObservable(result) |> toBeObservable(expected, ~errorValue=timeoutError);
    ts |> expectSubscriptions(e1 |> ColdObservable.subscriptions) |> toBeSubscriptions(e1subs);
  });

  testMarbles("timeoutWith: should timeout after a specified period then subscribe to the passed observable", ts => {
    let e1 = ts |> cold("-------a--b--|");
    let e1subs =      [|"^----!        "|];
    let e2 = ts |> cold("x-y-z-|       ")
    let e2subs =      [|"-----^-----!  "|];
    let expected =      "-----x-y-z-|  ";
    
    let timeoutError = Rx.TimeoutError.create();
    
    let result = e1
    |> ColdObservable.asObservable
    |> Rx.Operators.timeoutWith(
      ~due=`Number(5), 
      ~withObservable=`Observable(e2 |> ColdObservable.asObservable),
      ~scheduler=ts|.TestScheduler.asScheduler, 
      ());

    ts |> expectObservable(result) |> toBeObservable(expected, ~errorValue=timeoutError);
    ts |> expectSubscriptions(e1 |> ColdObservable.subscriptions) |> toBeSubscriptions(e1subs);
    ts |> expectSubscriptions(e2 |> ColdObservable.subscriptions) |> toBeSubscriptions(e2subs);
  });

  testMarbles("timestamp: should record the time stamp per each source elements", ts => {
    let e1 = ts |> hot("-b-c-----d--e--|");
    let e1subs =    [|"^--------------!"|];
    let expected =    "-w-x-----y--z--|";
    let values = { "w": 1, "x": 3, "y": 9, "z": 12 };

    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.timestamp(~scheduler=ts|.TestScheduler.asScheduler, ())
    |> Rx.Operators.map((x, _idx) => x |. Rx.Timestamp.timestampGet);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs);
  });

  testMarblesWithHotPreset(
    ~name="toArray: should reduce the values of an observable into an array",
    ~hot=     "---a--b--|",
    ~expected="---------(w|)",
    ~subs=  [|"^--------!"|],
    ~operator=Rx.Operators.toArray(),
    ~values={"w": [|"a", "b"|]},
    ()
  );

  testMarbles("timestamp: should record the time stamp per each source elements", ts => {
    let e1 = ts |> hot("-b-c-----d--e--|");
    let e1subs =    [|"^--------------!"|];
    let expected =    "-w-x-----y--z--|";
    let values = { "w": 1, "x": 3, "y": 9, "z": 12 };

    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.timestamp(~scheduler=ts|.TestScheduler.asScheduler, ())
    |> Rx.Operators.map((x, _idx) => x |. Rx.Timestamp.timestampGet);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs);
  });

  testMarbles("window: should emit windows that close and reopen", ts => {
    let source = ts |> hot("---a---b---c---d---e---f---g---h---i---|    ");
    let sourceSubs = [|"^--------------------------------------!    "|];
    let closing = ts |> hot("-------------w------------w----------------|");
    let closingSubs =[|"^--------------------------------------!    "|];
    let expected =     "x------------y------------z------------|    ";
    let x = ts |> cold(      "---a---b---c-|                              ");
    let y = ts |> cold(                   "--d---e---f--|                 ");
    let z = ts |> cold(                                "-g---h---i---|    ");
    let values = { "x": x, "y": y, "z": z };

    let result = source
    |> HotObservable.asObservable
    |> Rx.Operators.window(closing |> HotObservable.asObservable);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(source |> HotObservable.subscriptions) |> toBeSubscriptions(sourceSubs);
    ts |> expectSubscriptions(closing |> HotObservable.subscriptions) |> toBeSubscriptions(closingSubs);
  });

  testMarbles("windowCount: should emit windows with count 3, no skip specified", ts => {
    let source = ts |>   hot("---a---b---c---d---e---f---g---h---i---|");
    let sourceSubs =       [|"^--------------------------------------!"|];
    let expected =           "x----------y-----------z-----------w---|";
    let x = ts |> cold(      "---a---b---(c|)                         ");
    let y = ts |> cold(                   "----d---e---(f|)             ");
    let z = ts |> cold(                                "----g---h---(i|)");
    let w = ts |> cold(                                            "----|")
    let values = { "x": x, "y": y, "z": z, "w": w };

    let result = source
    |> HotObservable.asObservable
    |> Rx.Operators.windowCount(3, ());

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(source |> HotObservable.subscriptions) |> toBeSubscriptions(sourceSubs);
  });

  testMarbles("windowTime: should emit windows given windowTimeSpan and windowCreationInterval", ts => {
    let source = ts |> hot("--1--2--^-a--b--c--d--e---f--g--h-|");
    let subs =                   [|"^-------------------------!"|];

    let expected =           "x---------y---------z-----|";
    let x = ts |> cold(            "--a--(b|)                  ");
    let y = ts |> cold(                      "-d--e|           ");
    let z = ts |> cold(                                "-g--h| ");
    let values = { "x": x, "y": y, "z": z };

    let result = source
    |> HotObservable.asObservable
    |> Rx.Operators.windowTime(
      ~windowTimeSpan=5,
      ~windowCreationTimeInterval=10,
      ~maxWindowSize=Js.Int.max,
      ~scheduler=ts |> TestScheduler.asScheduler,
      ()
    );

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(source |> HotObservable.subscriptions) |> toBeSubscriptions(subs);
  });

  testMarbles("windowTime: should emit windows given windowTimeSpan and windowCreationInterval", ts => {
    let source = ts |>  hot("--1--2--^-a--b--c--d--e--f--g--h-|");
    let subs =             [|"^------------------------!"|];
    let e2 = ts |> cold(           "----w--------w--------w--|");
    let e2subs =           [|"^------------------------!"|];
    let e3 = ts |> cold(               "-----|                ");

    let expected =           "----x--------y--------z--|";
    let x = ts |> cold(                "-b--c|                ");
    let y = ts |> cold(                         "-e--f|       ");
    let z = ts |> cold(                                  "-h-|");
    let values = { "x": x, "y": y, "z": z };

    let result = source
    |> HotObservable.asObservable
    |> Rx.Operators.windowToggle(
      ~opening=e2 |> ColdObservable.asObservable, 
      ~closingSelector=() => e3 |> ColdObservable.asObservable);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(source |> HotObservable.subscriptions) |> toBeSubscriptions(subs); 
    ts |> expectSubscriptions(e2 |> ColdObservable.subscriptions) |> toBeSubscriptions(e2subs); 
  });

  testMarbles("windowWhen: should emit windows that close and reopen", ts => {
    let e1 = ts |> hot("--a--^--b--c--d--e--f--g--h--i--|");
    let e1subs =          [|"^--------------------------!"|];
    let e2 = ts |> cold(    "-----------|                ");
    let e2subs =          [|"^----------!                ",
                            "-----------^----------!     ",
                            "----------------------^----!"|];
    let a = ts |> cold(     "---b--c--d-|                ");
    let b = ts |> cold(                "-e--f--g--h|     ");
    let c = ts |> cold(                           "--i--|");
    let expected =    "a----------b----------c----|";
    let values = { "a": a, "b": b, "c": c };

    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.windowWhen(() => e2 |> ColdObservable.asObservable);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs);
    ts |> expectSubscriptions(e2 |> ColdObservable.subscriptions) |> toBeSubscriptions(e2subs);
  });

  testMarbles("withLatestFrom: should combine events from cold observables", ts => {
    let e1 = ts |> cold("-a--b-----c-d-e-|");
    let e2 = ts |> cold("--1--2-3-4---|   ");
    let expected =      "----B-----C-D-E-|";
    let values = { "B": [|"b", "1"|], "C": [|"c","4"|], "D": [|"d", "4"|], "E": [|"e", "4"|] };
    let result = e1
    |> ColdObservable.asObservable
    |> Rx.Operators.withLatestFrom([|
      e2 |> ColdObservable.asObservable
    |]);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
  });

  testMarbles("zipAll: should combine paired events from two observables", ts => {
    let x = ts |> cold(               "-a-----b-|");
    let y = ts |> cold(               "--1-2-----");
    let outer = ts |> hot("-x----y--------|         ", ~values={ "x": x, "y": y });
    let expected =  "-----------------A----B-|";

    let result = outer
    |> HotObservable.asObservable
    |> Rx.Operators.zipAllProject(values => values[0] ++ values[1]);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values={"A": "a1", "B": "b2"});
  });

  ()
});