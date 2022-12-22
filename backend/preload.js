const { contextBridge, ipcRenderer } = require("electron")

contextBridge.exposeInMainWorld("electronAPI", {
    requestFrame: () => ipcRenderer.invoke("frame_request"),
    resizeFrame: (width, height) => ipcRenderer.invoke("resize_request", [width, height]),
    startEngine: (width, height) => ipcRenderer.invoke("start_engine_request", [width, height])
})