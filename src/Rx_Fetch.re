
/**
  Uses [the Fetch API](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API) to make an HTTP request.
    
    @param input The resource you would like to fetch. Can be a url or a request object.
    @param init A configuration object for the fetch.
    @returns An Observable, that when subscribed to performs an HTTP request using the native `fetch` function.
 */
[@bs.module "rxjs/fetch"][@bs.val]
external fromFetch: (
    [@bs.unwrap][`String(string) | `Request(Fetch.Request.t)], 
    ~init: Fetch.RequestInit.t=?, 
    unit
) => Rx_Observable.Observable.t(Fetch.Response.t) = "fromFetch";