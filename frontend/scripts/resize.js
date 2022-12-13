const panel = document.getElementById("info")
const resizer = document.getElementById("resizer")
let width = parseInt(getComputedStyle(panel).width) 
let x, dx

function resize(eMove) {
    dx = eMove.screenX - x
    x = eMove.screenX
    width -= dx
    panel.style.width = width + "px"
}

resizer.addEventListener("mousedown", eDown => {
    x = eDown.screenX
    document.addEventListener("mousemove", resize)
    document.addEventListener("mouseup", () => {
        document.removeEventListener("mousemove", resize)
    })
})