/** FS System API */

// #region FS System API
// -sFORCE_FILESYSTEM=1 -sFILESYSTEM=1

type DeviceId = number;
type Char = number;

/* prettier-ignore */
// https://en.wikipedia.org/wiki/File-system_permissions#Numeric_notation
type EmFsPermission =
| 0o000 | 0o700 | 0o770 | 0o777 | 0o111 | 0o222
| 0o333 | 0o444 | 0o555 | 0o666 | 0o740;

// TODO Check constraints
type EmFsStat = {
  dev: number;
  ino: number;
  mode: number;
  nlink: number;
  uid: number;
  gid: number;
  rdev: number;
  size: number;
  atime: number;
  mtime: number;
  ctime: number;
  blksize: number;
  blocks: number;
};

type EmFsOpenFlag = "r" | "r+" | "w" | "wx" | "w+" | "wx+" | "a" | "ax" | "a+";

type EmFsStream = number;

type EmFsFileEncodingMapping = {
  binary: Uint8Array;
  utf8: string;
};

type EmFsNode = any;

type EmFsPathProp = {
  isRoot: boolean;
  exists: boolean;
  error: Error;
  name: string;
  path: string;
  object: EmFsNode;
  parentExists: boolean;
  parentPath: string | null;
  parentObject: EmFsPathProp | null;
};

type EmFs = {
  // Devices
  makedev: (major: number, minor: number) => DeviceId;
  registerDevice: (dev: DeviceId, opts: any) => void;

  // Standard IO Devices
  init: (
    input: () => Char | null,
    output: (c: Char | null) => void,
    error: (c: Char | null) => void
  ) => void;

  // File system API
  unmount: (mountpoint: string) => void;
  syncfs: (populate: boolean, callback: (err: any) => void) => void;
  mkdir: (path: string, mode: EmFsPermission) => void;
  mkdev: (path: string, mode: EmFsPermission, dev: DeviceId) => void;
  symlink: (oldpath: string, newpath: string) => void;
  rename: (oldpath: string, newpath: string) => void;
  rmdir: (path: string) => void;
  unlink: (path: string) => void;
  readlink: (path: string) => string;
  stat: (path: string) => EmFsStat;
  lstat: (path: string) => EmFsStat;
  chmod: (path: string, mode: EmFsPermission) => void;
  lchmod: (path: string, mode: EmFsPermission) => void;
  fchmod: (fd: number, mode: EmFsPermission) => void;
  chown: (path: string, uid: number, gid: number) => void;
  lchown: (path: string, uid: number, gid: number) => void;
  fchown: (fd: number, uid: number, gid: number) => void;
  truncate: (path: string, len: number) => void;
  ftruncate: (fd: number, len: number) => void;
  utime: (path: string, atime: number, mtime: number) => void;
  open: (path: string, flags: EmFsOpenFlag, mode: EmFsPermission) => EmFsStream;
  close: (stream: EmFsStream) => void;
  llseek: (stream: EmFsStream, offset: number, whence: 0 | 1 | 2) => void;

  read: (
    stream: EmFsStream,
    buffer: ArrayBufferView,
    offset: number,
    length: number,
    position?: number
  ) => void;

  write: (
    stream: EmFsStream,
    buffer: ArrayBufferView,
    offset: number,
    length: number,
    position?: number
  ) => void;

  readFile: <E extends keyof EmFsFileEncodingMapping>(
    path: string,
    opts: { encoding: E; flags: EmFsOpenFlag }
  ) => EmFsFileEncodingMapping[E];

  writeFile: (
    path: string,
    data: Uint8Array | string,
    opts: { flags: EmFsOpenFlag }
  ) => void;

  createLazyFile: (
    parent: string,
    name: string,
    canRead: boolean,
    canWrite: boolean
  ) => File;

  createPreloadedFile: (
    parent: string,
    name: string,
    canRead: boolean,
    canWrite: boolean
  ) => void;

  // File types
  isFile: (mode: number) => boolean;
  isDir: (mode: number) => boolean;
  isLink: (mode: number) => boolean;
  isChrdev: (mode: number) => boolean;
  isBlkdev: (mode: number) => boolean;
  isSocket: (mode: number) => boolean;

  // Paths
  cwd: () => string;
  chdir: (path: string) => void;
  readdir: (path: string) => string[];

  lookupPath: (
    path: string,
    opts: { parent: boolean; follow: boolean }
  ) => { path: string; node: EmFsNode };

  analyzePath: (path: string, dontResolveLastLink: boolean) => EmFsPathProp;

  getPath: (node: EmFsNode) => string;
};

declare var FS: EmFs;

// #endregion

/** Module object */

// #region Module object

type EmModuleCallback = <M extends EmModuleConfig>(module: M) => void;
type WasmImports = any;
type WasmExports = any;

type EmModuleConfig = {
  /**
   * @deprecated To force the environment, use the ENVIRONMENT compile-time option
   * (for example, -sENVIRONMENT=web or -sENVIRONMENT=node).
   */
  ENVIRONMENT?: string;

  print?: (...text: string[]) => void;
  printErr?: (...text: string[]) => void;

  /**
   * @param path Wasm Binary path
   * @param scriptDir Prepend the script dir
   */
  locateFile?: (path: string, scriptDir: string) => string;

  /** TODO */
  wasmBinary?: any;

  preRun?: EmModuleCallback | EmModuleCallback[];
  postRun?: EmModuleCallback | EmModuleCallback[];

  /** For monitoring progress of wasm loading */
  monitorRunDependencies?: (runDeps: number) => void;

  /**
   * Should throw an error. The default function throws
   * `e = new WebAssembly.RuntimeError(what)`.
   *
   * `printErr(what) |> readyPromiseReject(e) |> throw e`
   */
  onAbort?: (what: any) => never;

  /** TODO */
  instantiateWasm?: (
    imports: any,
    successCallback: (instance: any, module: WebAssembly.Module) => any
  ) => void;

  /** @default false */
  noExitRuntime?: boolean;

  /** TODO */
  onExit?: (code) => void;

  /**
   * @param msg `'Module.statusMessage (done/expected)'` or `''` on done
   */
  setStatus?: (msg: string) => void;

  /** Used by `Module.setStatus` */
  statusMessage?: string;

  /** skips a frame if returns `false` */
  preMainLoop?: () => boolean;

  postMainLoop?: () => void;

  canvas: HTMLCanvasElement;

  /** @default 4/3  */
  forcedAspectRatio?: number;

  /** TODO */
  elementPointerLock?: any;

  onFullScreen?: (isFullscreen: boolean) => void;

  onRuntimeInitialized?: () => void;

  preInit?: EmModuleCallback | EmModuleCallback[];

  noInitialRun?: boolean;
};

type EmModuleOwn = {
  ready: Promise<any>; // sets readyPromiseResolve/Reject
  // use these symbols in .then() callback
  // __main, __memory, __indirect_function_table, __fflush, onRuntimeInitialized

  arguments: string[]; // URL Query params 'key=val'
  thisProgram: string;
  quit: (status: string, toThrow: Error) => never;

  /** TODO Heaps from wasm buffer. And some deprecations Line 379... */
  /** Line 675... */

  /** Line 5533... */
  calledRun: boolean;

  requestFullscreen: (
    lockPointer: boolean,
    resizeCanvas: boolean
  ) => Promise<void>;

  /** @deprecated Use `Module.requestFullscreen` instead. (Without capital S) */
  requestFullScreen: () => never;

  /** @returns unique long integer id */
  requestAnimationFrmae: (func) => number;

  setCanvasSize: (width: number, height: number, noUpdates?: boolean) => void;

  pauseMainLoop: () => void;
  resumeMainLoop: () => void;
  getUserMedia: () => void;

  /** WebGL only */
  createContext: (
    canvas: HTMLCanvasElement,
    useWebGL,
    setInModule,
    webGLContextAttributes
  ) => void;
};

type EmModule = EmModuleConfig &
  EmModuleOwn & {
    getBytes: () => Int8Array;
  };

declare var initEmModule: (config: EmModuleConfig) => Promise<EmModule>;

// #endregion
