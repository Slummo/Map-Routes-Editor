//index.js
const path = require('path')
const addon = require("./backend/build/Release/addon")
const { ipcMain, BrowserWindow, app, screen } = require('electron');

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

const createWindow = () => {
    const win = new BrowserWindow({
        width: 900,
        height: 700,
        autoHideMenuBar: false,
        webPreferences: {
        preload: path.join(__dirname, 'backend/preload.js')
        }, 
    })
    win.loadFile("frontend/docs/index.html")
    win.setBackgroundColor('rgb(0, 0, 0)')
}

app.commandLine.appendSwitch('js-flags', '--max-old-space-size=2048');

app.whenReady().then(() => {createWindow();})

app.on('window-all-closed', () => {
    addon.stopEngine()
    app.quit()
  })


ipcMain.handleOnce("start_engine_request", async (event, arg) => {
    addon.startEngine(arg[0], arg[1])
})

ipcMain.handle("frame_request", async (event, arg) => {
    var start = performance.now();
    var frame = addon.loadFrame();
    var stop = performance.now();

    return frame;
})

ipcMain.handle("resize_request", async(event, arg) => {
    addon.resizeFrame(arg[0], arg[1]);
})

ipcMain.handle("camera_move_request", async(event, arg) => {
    addon.moveCamera(arg[0], arg[1], arg[2]);
})
