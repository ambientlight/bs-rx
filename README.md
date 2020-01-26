# bs-rx
Bucklescript bindings for [rxjs v7(alpha)](https://github.com/ReactiveX/rxjs)  
Most functionality is available, while ajax / fetch / websocket apis are not yet done. Refer to [documentation](https://ambientlight.github.io/bs-rx) for existing coverage.

## Installation and Usage

```
npm install @ambientlight/bs-rx
```

Then add `@ambientlight/bs-rx` into `bs-dependencies` in your project `bsconfig.json`.


```reason
Rx.range(~start=1, ~count=200, ())
|> Rx.Operators.filter((x, _idx) => x mod 2 == 1)
|> Rx.Operators.map((x, _idx) => x + x)
|> Rx.Observable.subscribe(
  ~next=x=>Js.log(x)
)
```

## Examples

#### Map and flatten each letter to an Observable ticking every 1 second

```reason
Rx.of_([|"a", "b", "c"|])
|> Rx.Operators.mergeMap(`Observable((x, _idx) => 
  Rx.interval(~period=1000, ())
  |> Rx.Operators.map((i, _idx) => string_of_int(i) ++ x)), 
  ())
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