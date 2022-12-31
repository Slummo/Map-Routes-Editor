{
    "targets": [{
        "target_name": "addon",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "sources": [
            "src/addon.cpp", "src/glad.c", "src/Graphics.cpp", "src/Framebuffer.cpp", "src/Event.cpp", "src/Program.cpp", "src/Texture.cpp", "src/Camera.cpp"
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "include",
        ],
        'libraries': [ "../lib/node.lib",
            "../lib/OpenGL32.Lib",
            "../lib/glfw3_mt.lib",
            "../lib/Gdi32.Lib"
        ],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }]
}