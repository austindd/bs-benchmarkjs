module Impl = (T: {type t;}) => {
  module Internal =
    BenchmarkJs__FFI.Times.Impl({
      type t = T.t;
    });
  let getCycle: T.t => float = Internal.getCycle;
  let getElapsed: T.t => float = Internal.getElapsed;
  let getPeriod: T.t => float = Internal.getPeriod;
  let getTimeStamp: T.t => float = Internal.getTimeStamp;
};