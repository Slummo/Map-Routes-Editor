const { contextBridge, ipcRenderer } = require("electron")

contextBridge.exposeInMainWorld("electronAPI", {
    requestFrame: () => ipcRenderer.invoke("image_request"),
    resizeFrame: (args) => ipcRenderer.invoke("resize_request", args)
})