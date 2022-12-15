//index.js
const testAddon = require('./build/Release/addon')
const path = require('path')
const { ipcMain, BrowserWindow, app, screen } = require('electron');

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

const createWindow = () => {
    const win = new BrowserWindow({width: 800, height: 600, autoHideMenuBar: true, webPreferences: {
        preload: path.join(__dirname, 'preload.js')
    }, contextIsolation: true})

    win.loadFile("test.html")
}

app.whenReady().then(() => {createWindow();})

var eddu

testAddon.startEngine(800, 600);
eddu = testAddon.loadImage();

async function ed() {
    await sleep(3000);
    console.log(testAddon.loadFrame())
} 

ed()

ipcMain.on("debug", function(event, data) {console.log(data)})


ipcMain.handle("image_request", async (event, arg) => {
    return eddu;
})

