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

    ctx.canvas.width = screenWidth;
    ctx.canvas.height = screenHeight;
    window.electronAPI.resizeFrame(screenWidth, screenHeight)
}

const ratio = async () => {
    ctx.canvas.width = screenWidth;
    ctx.canvas.height = screenHeight;

    await sleep(1000)
    
    window.electronAPI.startEngine(screenWidth, screenHeight)

    while (true) {
        var dat = await window.electronAPI.requestFrame();

        data = new Uint8ClampedArray(dat);
        if (data.length != screenWidth * screenHeight * 4) {
            continue;
        }
        
        var imgData = new ImageData(data, screenWidth, screenHeight)
        ctx.putImageData(imgData, 0, 0)
    }
}

ratio()
