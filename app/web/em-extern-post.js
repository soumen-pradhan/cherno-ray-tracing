// @ts-check
console.log("EM_EXTERN_POST_JS");

/**
 * Early exit
 * @param {string} msg
 * @returns {never}
 */
function exit(msg = "unreachable") {
  throw new Error(msg);
}

/**  @param {string} id  */
const refId = (id) => document.getElementById(id);

let em_loading = refId("em-loading") ?? exit();
const em_status = refId("em-status") ?? exit();

const em_resize = refId("em-resize") ?? exit();
const em_pointer_lock = refId("em-pointer-lock") ?? exit();

const em_fullscreen = refId("em-fullscreen") ?? exit();
const em_reset_window = refId("em-reset-window") ?? exit();

const em_canvas = refId("canvas") ?? exit();

const em_autoscroll = refId("em-autoscroll") ?? exit();
const em_stdout = refId("em-stdout") ?? exit();

// assert HTML* types

if (
  !(em_resize instanceof HTMLInputElement) ||
  !(em_pointer_lock instanceof HTMLInputElement) ||
  !(em_canvas instanceof HTMLCanvasElement) ||
  !(em_autoscroll instanceof HTMLInputElement) ||
  !(em_stdout instanceof HTMLTextAreaElement)
) {
  exit();
}

em_canvas.addEventListener("contextmenu", (e) => e.preventDefault());

const statusLog = [];

/** @type {EmModuleConfig} */
const emConfig = {
  canvas: em_canvas,

  print: (...text) => {
    const msg = text.join(" ");
    em_stdout.value += msg + "\n";

    if (em_autoscroll.checked) {
      em_stdout.scrollTop = em_stdout.scrollHeight; // focus on bottom
    }
  },

  setStatus(msg) {
    statusLog.push(msg);
    em_status.replaceChildren(msg);

    if (!msg || msg.length === 0) {
      console.log("Status: ", statusLog);
      const spinners = document.querySelectorAll("#em-info .spinner");
      for (let s of spinners) s.replaceWith(); // remove spinner
    }
  },

  monitorRunDependencies: (deps) => {
    emConfig.setStatus?.(`Deps remaining: ${deps}`);
  },
};

window.addEventListener("error", (e) => {
  em_status.classList.add("text-red-500", "font-bold");
  emConfig.setStatus?.("Exception thrown, see JS Console");
});

(async function main() {
  em_stdout.value = "";
  emConfig.setStatus?.("Downloading...");

  const module = await initEmModule(emConfig);

  const canvasObs = new ResizeObserver(
    debounce((entries) => {
      for (let e of entries) {
        if (e.target.id !== em_canvas.id) continue;

        em_canvas.width = e.contentRect.width;
        em_canvas.height = e.contentRect.height;

        module.setCanvasSize?.(em_canvas.width, em_canvas.height);
      }
    }, 200)
  );

  canvasObs.observe(em_canvas);

  em_fullscreen.addEventListener("click", () => {
    module.requestFullscreen?.(em_pointer_lock.checked, em_resize.checked);
  });

  em_reset_window.addEventListener("click", () => {
    module.setCanvasSize?.(em_canvas.width, em_canvas.height);
  });
})();

/*
initEmModule(emConfig).then((module) => {
  canvasObs = new ResizeObserver(
    debounce((entries) => {
      for (let e of entries) {
        if (e.target.id !== em_canvas.id) continue;

        em_canvas.width = e.contentRect.width;
        em_canvas.height = e.contentRect.height;

        module.setCanvasSize(em_canvas.width, em_canvas.height);
      }
    }, 200)
  );

  canvasObs.observe(em_canvas);

  em_canvas.addEventListener("contextmenu", (e) => e.preventDefault());

  em_fullscreen.addEventListener("click", () => {
    module.requestFullscreen(em_pointer_lock.checked, em_resize.checked);
  });

  em_reset_window.addEventListener("click", () => {
    module.setCanvasSize(em_canvas.width, em_canvas.height);
  });
});
*/

/**
 * Creates a debounced function that delays invoking `func` until after `wait`
 * milliseconds have elapsed since the last time the debounced function was
 * invoked, or until the next browser frame is drawn. The debounced function
 * comes with a `cancel` method to cancel delayed `func` invocations and a
 * `flush` method to immediately invoke them. Provide `options` to indicate
 * whether `func` should be invoked on the leading and/or trailing edge of the
 * `wait` timeout. The `func` is invoked with the last arguments provided to the
 * debounced function. Subsequent calls to the debounced function return the
 * result of the last `func` invocation.
 *
 * **Note:** If `leading` and `trailing` options are `true`, `func` is
 * invoked on the trailing edge of the timeout only if the debounced function
 * is invoked more than once during the `wait` timeout.
 *
 * If `wait` is `0` and `leading` is `false`, `func` invocation is deferred
 * until the next tick, similar to `setTimeout` with a timeout of `0`.
 *
 * If `wait` is omitted in an environment with `requestAnimationFrame`, `func`
 * invocation will be deferred until the next frame is drawn (typically about
 * 16ms).
 *
 * See [David Corbacho's article](https://css-tricks.com/debouncing-throttling-explained-examples/)
 * for details over the differences between `debounce` and `throttle`.
 *
 * @since 0.1.0
 * @category Function
 * @function
 * @template {Function} F
 * @param {F} func The function to debounce.
 * @param {number} [waitMs=0]
 *  The number of milliseconds to delay; if omitted, `requestAnimationFrame` is
 *  used (if available).
 * @param {Object} [options={}] The options object.
 * @param {boolean} [options.leading=false]
 *  Specify invoking on the leading edge of the timeout.
 * @param {number} [options.maxWait]
 *  The maximum time `func` is allowed to be delayed before it's invoked.
 * @param {boolean} [options.trailing=true]
 *  Specify invoking on the trailing edge of the timeout.
 * @returns {F} Returns the new debounced function.
 * @example
 *
 * // Avoid costly calculations while the window size is in flux.
 * jQuery(window).on('resize', debounce(calculateLayout, 150))
 *
 * // Invoke `sendMail` when clicked, debouncing subsequent calls.
 * jQuery(element).on('click', debounce(sendMail, 300, {
 *   'leading': true,
 *   'trailing': false
 * }))
 *
 * // Ensure `batchLog` is invoked once after 1 second of debounced calls.
 * const debounced = debounce(batchLog, 250, { 'maxWait': 1000 })
 * const source = new EventSource('/stream')
 * jQuery(source).on('message', debounced)
 *
 * // Cancel the trailing debounced invocation.
 * jQuery(window).on('popstate', debounced.cancel)
 *
 * // Check for pending invocations.
 * const status = debounced.pending() ? "Pending..." : "Ready"
 */

function debounce(func, waitMs, options) {
  let lastArgs;
  let lastThis;
  let maxWait;
  let result;
  let timerId;
  let lastCallTime;
  let lastInvokeTime = 0;
  let leading = false;
  let maxing = false;
  let trailing = true;

  // Bypass `requestAnimationFrame` by explicitly setting `wait=0`.
  const useRAF =
    !waitMs &&
    waitMs !== 0 &&
    typeof window.requestAnimationFrame === "function";

  if (typeof func !== "function") {
    throw new TypeError("Expected a function");
  }
  let wait = +(waitMs ?? 0);
  if (typeof options === "object") {
    leading = !!options.leading;
    maxing = "maxWait" in options;
    maxWait = maxing ? Math.max(+(options.maxWait ?? 0), wait) : maxWait;
    trailing = "trailing" in options ? !!options.trailing : trailing;
  }

  function invokeFunc(time) {
    const args = lastArgs;
    const thisArg = lastThis;

    lastArgs = lastThis = undefined;
    lastInvokeTime = time;
    result = func.apply(thisArg, args);
    return result;
  }

  function startTimer(pendingFunc, milliseconds) {
    if (useRAF) {
      window.cancelAnimationFrame(timerId);
      return window.requestAnimationFrame(pendingFunc);
    }
    // eslint-disable-next-line @typescript-eslint/no-implied-eval
    return setTimeout(pendingFunc, milliseconds);
  }

  function cancelTimer(id) {
    if (useRAF) {
      window.cancelAnimationFrame(id);
      return;
    }
    clearTimeout(id);
  }

  function leadingEdge(time) {
    // Reset any `maxWait` timer.
    lastInvokeTime = time;
    // Start the timer for the trailing edge.
    timerId = startTimer(timerExpired, wait);
    // Invoke the leading edge.
    return leading ? invokeFunc(time) : result;
  }

  function remainingWait(time) {
    const timeSinceLastCall = time - lastCallTime;
    const timeSinceLastInvoke = time - lastInvokeTime;
    const timeWaiting = wait - timeSinceLastCall;

    return maxing
      ? Math.min(timeWaiting, maxWait - timeSinceLastInvoke)
      : timeWaiting;
  }

  function shouldInvoke(time) {
    const timeSinceLastCall = time - lastCallTime;
    const timeSinceLastInvoke = time - lastInvokeTime;

    // Either this is the first call, activity has stopped and we're at the
    // trailing edge, the system time has gone backwards and we're treating
    // it as the trailing edge, or we've hit the `maxWait` limit.
    return (
      lastCallTime === undefined ||
      timeSinceLastCall >= wait ||
      timeSinceLastCall < 0 ||
      (maxing && timeSinceLastInvoke >= maxWait)
    );
  }

  function timerExpired() {
    const time = Date.now();
    if (shouldInvoke(time)) {
      return trailingEdge(time);
    }
    // Restart the timer.
    timerId = startTimer(timerExpired, remainingWait(time));
    return undefined;
  }

  function trailingEdge(time) {
    timerId = undefined;

    // Only invoke if we have `lastArgs` which means `func` has been
    // debounced at least once.
    if (trailing && lastArgs) {
      return invokeFunc(time);
    }
    lastArgs = lastThis = undefined;
    return result;
  }

  function cancel() {
    if (timerId !== undefined) {
      cancelTimer(timerId);
    }
    lastInvokeTime = 0;
    lastArgs = lastCallTime = lastThis = timerId = undefined;
  }

  function flush() {
    return timerId === undefined ? result : trailingEdge(Date.now());
  }

  function pending() {
    return timerId !== undefined;
  }

  function debounced(...args) {
    const time = Date.now();
    const isInvoking = shouldInvoke(time);

    lastArgs = args;
    lastThis = this;
    lastCallTime = time;

    if (isInvoking) {
      if (timerId === undefined) {
        return leadingEdge(lastCallTime);
      }
      if (maxing) {
        // Handle invocations in a tight loop.
        timerId = startTimer(timerExpired, wait);
        return invokeFunc(lastCallTime);
      }
    }
    if (timerId === undefined) {
      timerId = startTimer(timerExpired, wait);
    }
    return result;
  }
  debounced.cancel = cancel;
  debounced.flush = flush;
  debounced.pending = pending;

  // @ts-ignore
  return debounced;
}
