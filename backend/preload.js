const { contextBridge, ipcRenderer } = require("electron")

contextBridge.exposeInMainWorld("electronAPI", {
    requestFrame: () => ipcRenderer.invoke("image_request")
})