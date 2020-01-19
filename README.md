# bs-rx

## [Operators In Alphabetical Order](./AlphabetizedList.md)

## [Categories of operators](https://github.com/ReactiveX/rxjs/blob/master/docs_app/content/guide/operators.md#categories-of-operators)

There are operators for different purposes, and they may be categorized as: creation, transformation, filtering, joining, multicasting, error handling, utility, etc. In the following list you will find all the operators organized in categories.

## Creation Operators

ajax
bindCallback
bindNodeCallback
defer
empty
from
fromEvent
fromEventPattern
generate
interval
of
range
throwError
timer
iif

## Join Creation Operators

These are Observable creation operators that also have join functionality -- emitting values of multiple source Observables.

combineLatest
concat
forkJoin
merge
race
zip

## Transformation Operators

buffer
bufferCount
bufferTime
bufferToggle
bufferWhen
concatMap
concatMapTo
exhaust
exhaustMap
expand
groupBy
map
mapTo
mergeMap
mergeMapTo
mergeScan
pairwise
partition
pluck
scan
switchMap
switchMapTo
window
windowCount
windowTime
windowToggle
windowWhen

## Filtering Operators

audit
auditTime
debounce
debounceTime
distinct
distinctKey
distinctUntilChanged
distinctUntilKeyChanged
elementAt
filter
first
ignoreElements
last
sample
sampleTime
single
skip
skipLast
skipUntil
skipWhile
take
takeLast
takeUntil
takeWhile
throttle
throttleTime

## Join Operators

## Also see the Join Creation Operators section above.

combineAll
concatAll
exhaust
mergeAll
startWith
withLatestFrom

## Multicasting Operators

multicast
publish
publishBehavior
publishLast
publishReplay
share

## Error Handling Operators

catchError
retry
retryWhen

## Utility Operators

tap
delay
delayWhen
dematerialize
materialize
observeOn
subscribeOn
timeInterval
timestamp
timeout
timeoutWith
toArray

## Conditional and Boolean Operators

defaultIfEmpty
every
find
findIndex
isEmpty

## Mathematical and Aggregate Operators

count
max
min
reduce
