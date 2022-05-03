[![STATUS](https://github.com/ambientlight/bs-rx/workflows/Deploy%20Docs/badge.svg)](https://github.com/ambientlight/bs-rx/actions)
[![VERSION](https://img.shields.io/npm/v/@ambientlight/bs-rx)](https://www.npmjs.com/package/@ambientlight/bs-rx)
[![LICENSE](https://img.shields.io/github/license/ambientlight/bs-rx?t)](https://github.com/ambientlight/bs-rx/blob/master/LICENSE)
[![ISSUES](https://img.shields.io/github/issues/ambientlight/bs-rx)](https://github.com/ambientlight/bs-rx/issues)

# bs-rx [NO LONGER MAINTAINED]
Bucklescript bindings for [rxjs v7(alpha)](https://github.com/ReactiveX/rxjs)  
Most functionality is available, while ajax / websocket apis are not yet done. Refer to [documentation](https://ambientlight.github.io/bs-rx) for existing coverage.

## Installation and Usage

```
npm install @ambientlight/bs-rx reason-promise bs-fetch
```

[reason-promise](https://github.com/aantron/promise) and [bs-fetch](https://github.com/reasonml-community/bs-fetch) are peer dependencies

Then add `@ambientlight/bs-rx` (and `reason-promise`, `bs-fetch`) into `bs-dependencies` in your project `bsconfig.json`.


```reason
Rx.range(~start=1, ~count=200, ())
|> Rx.Operators.filter((x, _idx) => x mod 2 == 1)
|> Rx.Operators.map((x, _idx) => x + x)
|> Rx.Observable.subscribe(
  ~next=x=>Js.log(x)
)
```

You may also utilize **-n**(no idx) suffixed versions of those operators that errase **idx** argument:

```reason
Rx.range(~start=1, ~count=200, ())
|> Rx.Operators.filtern(x => x mod 2 == 1)
|> Rx.Operators.mapn(x => x + x)
|> Rx.Observable.subscribe(
  ~next=x=>Js.log(x)
);
```

## Examples

#### Map and flatten each letter to an Observable ticking every 1 second

```reason
Rx.of_([|"a", "b", "c"|])
|> Rx.Operators.mergeMap(`Observable((x, _idx) => 
  Rx.interval(~period=1000)
  |> Rx.Operators.map((i, _idx) => string_of_int(i) ++ x)))
|> Rx.Observable.subscribe(~next=x=>Js.log(x));
```

#### Custom operator

Create an observable that never completes and repeats when browser is back online.

```reason
let repeatWhenOnline = source => 
  source
  |> Rx.Operators.takeUntil(Rx.fromEvent(~target=Webapi.Dom.window, ~eventName="offline"))
  |> Rx.Operators.repeatWhen(_notifier => Rx.fromEvent(~target=Webapi.Dom.window, ~eventName="online"));

let obs = Rx.of1("I'm online")
|> repeatWhenOnline
|> Rx.Observable.subscribe(~next=x=>Js.log(x));
```

#### Fetch

```reason
Rx.Fetch.fromFetch(`String("https://api.github.com/users?per_page=5"), ())
|> Rx.Operators.mergeMap(`Promise((response, _idx) => response |> Fetch.Response.json))
|> Rx.Observable.subscribe(
  ~next=value => Js.log(value),
  ~error=error=>Js.log(error),
  ~complete=() => Js.log("done")
);
```

Also, have a look at [OperatorTests](https://github.com/ambientlight/bs-rx/blob/master/__tests__/OperatorTests.re) for more usage examples.

## Promises

All bindings accepting promises come in two flavours: as `Js.Promise.t` (shipped with bucklescript) and `Promise.t` from [reason-promise](https://github.com/aantron/promise). For the later, use **\`Repromise** variant constructor.

```reason
[@bs.module "rxjs/operators"]
external mergeMap: (
  [@bs.unwrap] [
    | `Observable(('a, int) => t('b))
    | `Promise(('a, int) => Js.Promise.t('b))
    | `Repromise(('a, int) => Promise.t('b))
    | `Array(('a, int) => array('b))
  ],
  ~concurrent: int=?
) => operator('a, 'b) = "mergeMap";
```

## Testing

You may find marble testing handy to test your rxjs logic. Marble string syntax allows you to specify rxjs events(such as emissions, subscription points) over virtual time that progresses by frames(denoted by `-`). You can use it to express the expected behavior of your observable sequences as strings and compare them with `Rx.Observable.t('a)` instances you are testing. You need to initialize `TestScheduler.t` with a function that can perform deep comparison (such as `BsMocha.Assert.deep_equal`), then put your marble tests inside `ts |> TestScheduler.run(_r => ...)`. Asynchronous operators usually take `~scheduler` parameter, pass `TestScheduler.t` instance to them. The next example illustrates it, also you may want to refer to rxjs [marble diagrams documentation](https://rxjs-dev.firebaseapp.com/guide/testing/marble-testing).

```reason
open Jest;
open Rx.Testing;
open TestScheduler;

//...

test("timeInterval: should record the time interval between source elements", () => {
  let ts = TestScheduler.create(~assertDeepEqual=BsMocha.Assert.deep_equal);
  ts |> run(_r => {
    // subscribe in 6th frame, 4 emissions: b, c, d, e
    let e1 = ts |> hot("--a--^b-c-----d--e--|");
    let e1subs =    [|"^--------------!"|];
    let expected =    "-w-x-----y--z--|";
    // expected values in w, x, y, z emissions
    let values = { "w": 1, "x": 2, "y": 6, "z": 3 };

    let result = e1
    |> HotObservable.asObservable
    |> Rx.Operators.timeInterval(~scheduler=ts|.TestScheduler.asScheduler)
    |> Rx.Operators.map((x, _idx) => x |. Rx.TimeInterval.intervalGet);

    ts |> expectObservable(result) |> toBeObservable(expected, ~values);
    ts |> expectSubscriptions(e1 |> HotObservable.subscriptions) |> toBeSubscriptions(e1subs);
    Expect.expect(true) 
  })
  |> Expect.toBe(true)
});
```

## Contributing

Any contribution is greatly appreciated. Feel free to reach out in issues for any questions or problem you ran into. Implementational inheritance is used to model inheritance used in rxjs, you may want to refer to [Implementation Inheritance](https://github.com/reasonml-community/bs-webapi-incubator#implementation-inheritance). 

```
git clone https://github.com/ambientlight/bs-rx.git
cd bs-rx
npm install
npm run build
npm run test
```

You can also generate docs via [bsdoc](https://github.com/reuniverse/bsdoc). If you have forked this repo, the pushes to master should spin the github actions workflow that rebuild the github pages docs with workflow available at [deploy_docs.yml](https://github.com/ambientlight/bs-rx/blob/master/.github/workflows/deploy_docs.yml). (You will need to set `GH_PAGES_TOKEN` for github pages deployment to work).

If you want to generate docs in local make sure you have [bsdoc](https://github.com/reuniverse/bsdoc) built against ocaml version matching the ocaml version used in your `bs-platform` (`4.02.3+buckle-master` for bs-platform@5.2.1).

```
opam switch 4.02.3+buckle-master
```

For osx, you can use the npm installation of bsdoc(corresponds to bs-platform 6), but for linux-based distros, you would need to build bsdoc from source for now.

## See Also

[reductive-observable](https://github.com/ambientlight/reductive-observable): Centalized rx side-effects for reductive.
