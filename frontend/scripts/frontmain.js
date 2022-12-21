const img = document.getElementById("map")
const canvas = document.getElementById("canvas")
const ctx = canvas.getContext("2d")

function sleep (time) {
    return new Promise((resolve) => setTimeout(resolve, time));
}

async function idk() {
    await sleep(3000)
    while (true) {
        console.log("a")
        var data = await window.electronAPI.requestFrame()
        data = new Uint8ClampedArray(data)
        var imgData = new ImageData(data, 400, 400)
        ctx.putImageData(imgData, 0, 0)
    }
    // base_image = new Image();
    // base_image.src = 'ics.PNG';
    // base_image.onload = function(){
    // ctx.drawImage(base_image, 0, 0);
  
}

idk()
