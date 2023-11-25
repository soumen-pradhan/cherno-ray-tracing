### Issues

❌ Check if mouse coords are shifted. (They are shifted by `1px`, towards top and left)

❌ scrolling on page causes a passive event listener to call `e.preventDefault()`

❌ HTML layout is very finicky. `body` cannot have more than the `#canvas-wrapper`.
`nav` has to lifted out of element flow.

✔ EM_ASM has access to emModule even if -sMODULARIZE=1
