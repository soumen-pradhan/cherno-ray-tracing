/**
 *
 * PRESERVE POST_JS COMMENTS
 *
 */

console.log("EM_POST_JS");

addRunDependency(200);
FS.mkdir("/offline");
FS.mount(IDBFS, {}, "/offline");

FS.syncfs(true, (err) => {
  console.log("[Module] FS.syncfs", err ?? "");
  removeRunDependency(200);
});

window.addEventListener("beforeunload", () => {
  FS.syncfs((err) => console.log("FS.syncFs: Synced beforeunload.", err));
});
