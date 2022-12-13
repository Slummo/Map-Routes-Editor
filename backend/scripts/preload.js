const {contextBridge, ipcRenderer} = require("electron")

contextBridge.exposeInMainWorld(
    "electronAPI", {
        //nomi funzioni o variabili
    }
)