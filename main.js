//index.js
const path = require('path')
const { ipcMain, BrowserWindow, app, screen } = require('electron');

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

const createWindow = () => {
    const win = new BrowserWindow({
        autoHideMenuBar: true,
        webPreferences: {
        preload: path.join(__dirname, 'backend/preload.js')
        }, 
        contextIsolation: true
    })
    win.loadFile("frontend/docs/index.html")
    win.maximize()
}

app.whenReady().then(() => {createWindow();})

// var eddu

// testAddon.startEngine(800, 600);
// eddu = testAddon.loadImage();

// async function ed() {
//     await sleep(3000);
//     console.log(testAddon.loadFrame())
// } 

// ed()

// ipcMain.on("debug", function(event, data) {console.log(data)})


// ipcMain.handle("image_request", async (event, arg) => {
//     return eddu;
// })

