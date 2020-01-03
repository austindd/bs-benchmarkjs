module Impl =
       (
         T: {
           type t;
           type benchmark;
           type suiteOptions;
           type options;
           type stats;
           type times;
           type event;
           type eventHandler;
           type testFn;
           type setupFn;
           type teardownFn;
           type eventType;
           let eventTypeToString: eventType => string;
           let eventTypeFromString: string => eventType;
         },
       ) => {

  module Internal = {

    include Glennsl__BsBenchmarkJs__FFI.Suite.Impl({
      type t = T.t;
      type benchmark = T.benchmark;
      type suiteOptions = T.suiteOptions;
      type options = T.options;
      type event = T.event;
      type eventHandler = T.eventHandler;
      type testFn = T.testFn;
      type setupFn = T.setupFn;
    });

    module Benchmark =
      Glennsl__BsBenchmarkJs__FFI.Benchmark.Impl({
        type t = T.benchmark;
        type testFn = T.testFn;
        type setupFn = T.setupFn;
        type teardownFn = T.teardownFn;
        type stats = T.stats;
        type times = T.times;
        type options = T.options;
      });

    // TODO: needs testing
    let eventListToString: list(T.eventType) => string =
      eventList =>
        Belt.List.reduceU(eventList, "", (. str, evt) => {
          str ++ T.eventTypeToString(evt) ++ " "
        });
    let addBenchmarkToSuite: (. T.t, T.benchmark) => T.t =
      (. suite, bench) =>
        addWithOptions(
          Benchmark.getName(bench),
          Benchmark.getFn(bench),
          Benchmark.getOptions(bench),
          suite,
        );
  };

  let getName: T.t => string = Internal.getName;
  let getAborted: T.t => bool = Internal.getAborted;
  let getLength: T.t => int = Internal.getLength;
  let getRunning: T.t => bool = Internal.getRunning;

  /** Creates a new [ Suite.t ] object. Optionally takes a [ SuiteOptions.t ] object for configuration. */
  let make: (~options: T.suiteOptions=?, string) => T.t =
    (~options=?, name) =>
      switch (options) {
      | None => Internal.make(name)
      | Some(opt) => Internal.makeWithOptions(name, opt)
      };

  let add: (~options: T.options=?, string, T.testFn, T.t) => T.t =
    (~options=?, name, fn, suite) =>
      switch (options) {
      | None => Internal.add(name, fn, suite)
      | Some(opt) => Internal.addWithOptions(name, fn, opt, suite)
      };

  let run: (~options: T.suiteOptions=?, T.t) => T.t =
    (~options=?, suite) =>
      switch (options) {
      | None => Internal.run(suite)
      | Some(opt) => Internal.runWithOptions(opt, suite)
      };

  let clone: (~options: T.suiteOptions=?, T.t) => T.t =
    (~options=?, suite) =>
      switch (options) {
      | None => Internal.clone(suite)
      | Some(opt) => Internal.cloneWithOptions(opt, suite)
      };

  let filter: (T.benchmark => bool, T.t) => T.t = Internal.filter;
  let filterByFastest: T.t => T.t = Internal.filterByFastest;
  let filterBySlowest: T.t => T.t = Internal.filterBySlowest;
  let filterBySuccessful: T.t => T.t = Internal.filterBySuccessful;

  let getListeners: T.t => array(T.eventHandler) = Internal.getListeners;
  let getListenersByEvent: (T.eventType, T.t) => array(T.eventHandler) =
    (eventType, suite) => Internal.getListenersByEvent(T.eventTypeToString(eventType), suite);

  let emit: (T.eventType, T.t) => T.t =
    (eventType, suite) => Internal.emit(T.eventTypeToString(eventType), suite);

  let abort: T.t => T.t = Internal.abort;
  let reset: T.t => T.t = Internal.reset;

  let onAbort: (T.eventHandler, T.t) => T.t =
    (handler, suite) => Internal.on("abort", handler, suite);
  let onComplete: (T.eventHandler, T.t) => T.t =
    (handler, suite) => Internal.on("complete", handler, suite);
  let onCycle: (T.eventHandler, T.t) => T.t =
    (handler, suite) => Internal.on("cycle", handler, suite);
  let onError: (T.eventHandler, T.t) => T.t =
    (handler, suite) => Internal.on("error", handler, suite);
  let onReset: (T.eventHandler, T.t) => T.t =
    (handler, suite) => Internal.on("reset", handler, suite);
  let onStart: (T.eventHandler, T.t) => T.t =
    (handler, suite) => Internal.on("start", handler, suite);

  let on: (list(T.eventType), T.eventHandler, T.t) => T.t =
    (eventList, handler, suite) => Internal.(eventListToString(eventList)->on(handler, suite));

  let removeListener: (list(T.eventType), T.eventHandler, T.t) => T.t =
    (eventList, handler, suite) =>
      Internal.(eventListToString(eventList)->removeListener(handler, suite));

  let removeListenersByEvent: (list(T.eventType), T.t) => T.t =
    (eventList, suite) =>
      Internal.(eventListToString(eventList)->removeListenersByEvent(suite));

  let removeAllListeners: T.t => T.t = Internal.removeAllListeners;

  let toArray: T.t => array(T.benchmark) = Internal.toBenchmarkArray;
  let toList: T.t => list(T.benchmark) =
    suite => toArray(suite)->Belt.List.fromArray->Belt.List.reverse;

  let fromArray: (~options: T.suiteOptions=?, string, array(T.benchmark)) => T.t =
    (~options=?, name, benchArray) => {
      let suite =
        switch (options) {
        | None => make(name)
        | Some(opt) => make(name, ~options=opt)
        };
      Belt.Array.reduceU(benchArray, suite, Internal.addBenchmarkToSuite);
    };

  let fromList: (~options: T.suiteOptions=?, string, list(T.benchmark)) => T.t =
    (~options=?, name, benchList) => {
      let suite =
        switch (options) {
        | None => make(name)
        | Some(opt) => make(name, ~options=opt)
        };
      Belt.List.reverse(benchList)->Belt.List.reduceU(suite, Internal.addBenchmarkToSuite);
    };
};