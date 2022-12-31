const img = document.getElementById("map")
const canvas = document.getElementById("canvas")
const ctx = canvas.getContext("2d")

const socket = new WebSocket("ws://127.0.0.1:1010")


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

var oldstart;

socket.onmessage = async function (event) {
    // start = performance.now()
    console.log(event.data)
    data = new Uint8ClampedArray(event.data);
    if (data.length != screenWidth * screenHeight * 4) {
        socket.send("g")
    }

    var imgData = new ImageData(data, screenWidth, screenHeight)
    ctx.putImageData(imgData, 0, 0)
    socket.send("g")
    // stop = performance.now()
    // console.log('it took ', stop - start, ' ms')
}

const ratio = async () => {
    ctx.canvas.width = screenWidth;
    ctx.canvas.height = screenHeight;

    await sleep(1000)
    
    window.electronAPI.startEngine(screenWidth, screenHeight)

    socket.send("start")
    // while (true) {

    //     var dat = await window.electronAPI.requestFrame();

    //     data = new Uint8ClampedArray(dat);
    //     if (data.length != screenWidth * screenHeight * 4) {
    //         continue;
    //     }
        
        
    //     var imgData = new ImageData(data, screenWidth, screenHeight)
    //     ctx.putImageData(imgData, 0, 0)
    // }
}

socket.onopen = function(args) {
    socket.binaryType = "arraybuffer";
    ratio()
}
