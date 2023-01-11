const img = document.getElementById("map")
const canvas = document.getElementById("canvas")
const ctx = canvas.getContext("2d")


function sleep (time) {
    return new Promise((resolve) => setTimeout(resolve, time));
}

var screenWidth = window.outerWidth
var screenHeight = window.outerHeight
var lastClick = 0;
function newAtan(y, x) {
    return (2 * Math.atan2(y, x) + Math.PI);
}

document.getElementById("canvas").addEventListener("mousemove", async (e) => { 
    if (e.buttons != 1)
    {
        return;
    }
    var speed = newAtan((-window.innerHeight/2 + e.y), (-window.innerWidth/2 + e.x + 1)) - lastClick; 
    if (speed > 1.1 || speed < -1.1) speed = speed/Math.abs(speed) * 0.5;
    window.electronAPI.moveCamera((-window.innerWidth/2 + e.x), (-window.innerHeight/2 + e.y), speed);
    var dat = await window.electronAPI.loadFrame();

    data = new Uint8ClampedArray(dat);
    if (data.length != screenWidth * screenHeight * 4) {
        return;
    }
    
    var imgData = new ImageData(data, screenWidth, screenHeight)
    ctx.putImageData(imgData, 0, 0)
    lastClick = newAtan((-window.innerHeight/2 + e.y), (-window.innerWidth/2 + e.x) + 1);
});

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

    await sleep(3000)
    
    window.electronAPI.startEngine(screenWidth, screenHeight)

    while (true) {
        var start = performance.now();
        data = await window.electronAPI.loadFrame();
        var stop = performance.now();

        data = new Uint8ClampedArray(data);
        if (data.length != screenWidth * screenHeight * 4) {
            continue;
        }
        
        var imgData = new ImageData(data, screenWidth, screenHeight)
        ctx.putImageData(imgData, 0, 0)
    }
}

ratio()

