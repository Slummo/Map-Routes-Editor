const img = document.getElementById("map")
const canvas = document.getElementById("canvas")
const ctx = canvas.getContext("2d")

function sleep (time) {
    return new Promise((resolve) => setTimeout(resolve, time));
}

async function idk() {
    await sleep(3000)
    const data = await window.electronAPI.requestFrame()
    console.log(data[2])
    for(let i=0; i < 100; i++) {
        for(let j=0; j < 100; j++) {
            r = data[i * 100 + j + 0]
            g = data[i * 100 + j + 1]
            b = data[i * 100 + j + 2]
            ctx.fillStyle = "rgba("+r+","+g+","+b+", 1)"
            ctx.fillRect((i/3)%100, Math.floor((i/3)/100), 1, 1)
        } 
    }
}

idk()
