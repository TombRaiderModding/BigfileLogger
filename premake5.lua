workspace "BigfileLogger"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64" }

project "BigfileLogger"
    kind "SharedLib"

    language "C++"
    cppdialect "C++17"

    files "src/**"

    files { "vendor/patterns/*.cpp", "vendor/minhook/src/**" }
    includedirs { "vendor/patterns", "vendor/minhook/include", "vendor/jitasm" }

    symbols "On"

    filter "platforms:Win32"
        architecture "x86"

    filter "platforms:Win64"
        architecture "x86_64"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"