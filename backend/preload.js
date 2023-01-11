const { contextBridge, ipcRenderer } = require("electron")


contextBridge.exposeInMainWorld("electronAPI", {
    loadFrame: () => ipcRenderer.invoke("frame_request"),
    resizeFrame: (width, height) => ipcRenderer.invoke("resize_request", [width, height]),
    startEngine: (width, height) => ipcRenderer.invoke("start_engine_request", [width, height]),
    moveCamera: (camX, camZ, camTheta) => ipcRenderer.invoke("camera_move_request", [camX, camZ, camTheta])
})