//index.js
const path = require('path')
const addon = require("./backend/build/Release/addon")
const { ipcMain, BrowserWindow, app, screen } = require('electron');

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

const createWindow = () => {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        autoHideMenuBar: false,
        webPreferences: {
        preload: path.join(__dirname, 'backend/preload.js')
        }, 
        contextIsolation: true
    })
    win.loadFile("frontend/docs/index.html")
    //win.maximize()
}

app.whenReady().then(() => {createWindow();})

ipcMain.handleOnce("start_engine_request", async (event, arg) => {
    addon.startEngine(arg[0], arg[1])
})

ipcMain.handle("frame_request", async (event, arg) => {
    return new Uint8Array(addon.loadFrame());
})

ipcMain.handle("resize_request", async(event, arg) => {
    addon.resizeFrame(arg[0], arg[1]);
})
