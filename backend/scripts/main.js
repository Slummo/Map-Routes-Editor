const {app, BrowserWindow, ipcMain} = require("electron")
const path = require("path")

function createWindow () {
  const mainWindow = new BrowserWindow({
      webPreferences: {
          preload: path.join(__dirname, "preload.js")
      },
      //fullscreen: true
  })

  mainWindow.loadFile(path.join(__dirname, "../../frontend/docs/index.html"))
  mainWindow.maximize()
}

app.whenReady().then(() => {
  createWindow()
  
  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on("window-all-closed", () => {
  if (process.platform !== 'darwin') app.quit()
})