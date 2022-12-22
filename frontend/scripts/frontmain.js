const img = document.getElementById("map")
const canvas = document.getElementById("canvas")
const ctx = canvas.getContext("2d")

var viewport_width = window.innerWidth;
var viewport_height = window.innerHeight;

function sleep (time) {
    return new Promise((resolve) => setTimeout(resolve, time));
}

var screenWidth
var screenHeight

window.onresize = () => {
    screenWidth = window.innerWidth
    screenHeight = window.innerHeight

    var arg = {
        width: screenWidth,
        height: screenHeight
    }

    ctx.canvas.width = screenWidth;
    ctx.canvas.height = screenHeight;
    window.electronAPI.resizeFrame(arg)
}

const ratio = async () => {
    await sleep(3000)

    while (true) {
        var data = await window.electronAPI.requestFrame()
        data = new Uint8ClampedArray(data)
        var imgData = new ImageData(data, screenWidth, screenHeight)
        ctx.putImageData(imgData, 0, 0)
    }
}

ratio()
