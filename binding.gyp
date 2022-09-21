{
  "targets": [
    {
      "target_name": "snap7",
      "type": "static_library",
      "sources": [
        "./snap7/src/sys/snap_msgsock.cpp",
        "./snap7/src/sys/snap_sysutils.cpp",
        "./snap7/src/sys/snap_tcpsrvr.cpp",
        "./snap7/src/sys/snap_threads.cpp",
        "./snap7/src/core/s7_client.cpp",
        "./snap7/src/core/s7_isotcp.cpp",
        "./snap7/src/core/s7_partner.cpp",
        "./snap7/src/core/s7_peer.cpp",
        "./snap7/src/core/s7_server.cpp",
        "./snap7/src/core/s7_text.cpp",
        "./snap7/src/core/s7_micro_client.cpp",
        "./snap7/src/lib/snap7_libmain.cpp"
       ],
      "include_dirs": [
        "./snap7/src/sys",
        "./snap7/src/core",
        "./snap7/src/lib"
      ],
      "cflags_cc!": ["-fno-exceptions"],
    },
    {
      "target_name": "node_snap7_client",
      "sources": [
        "src/addon.cc",
        "src/v3/snap7.cpp",
        "src/v3/snap7_client.cpp",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")", "snap7"],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS"],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
    }
  ]
}