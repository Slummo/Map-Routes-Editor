const { contextBridge, ipcRenderer } = require("electron")

contextBridge.exposeInMainWorld("electronAPI", {
    debug: (message) => {ipcRenderer.send("debug", message)},
    doAction: () => ipcRenderer.invoke("image_request")
})