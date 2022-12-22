const img = document.getElementById("map")
const canvas = document.getElementById("canvas")
const ctx = canvas.getContext("2d")

function sleep (time) {
    return new Promise((resolve) => setTimeout(resolve, time));
}

var screenWidth = window.outerWidth
var screenHeight = window.outerHeight

window.onresize = () => {
    screenWidth = window.outerWidth
    screenHeight = window.outerHeight

    ctx.canvas.width = 800;
    ctx.canvas.height = 600;
    //window.electronAPI.resizeFrame(screenWidth, screenHeight)
}

const ratio = async () => {
    await sleep(1000)
    ctx.canvas.width = screenWidth;
    ctx.canvas.height = screenHeight;

    window.electronAPI.startEngine(screenWidth, screenHeight)

    while (true) {
        var data = await window.electronAPI.requestFrame()
        data = new Uint8ClampedArray(data)
        if (data.length != screenWidth * screenHeight * 4) {
            ;
        }
        var imgData = new ImageData(data, screenWidth, screenHeight)
        ctx.putImageData(imgData, 0, 0)
    }
}

ratio()
