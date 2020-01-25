

module TimeoutError {
  type t;

  [@bs.module "rxjs"][@bs.new]
  external create: unit => t = "TimeoutError";
}
