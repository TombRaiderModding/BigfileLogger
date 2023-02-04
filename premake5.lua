workspace "BigfileLogger"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64" }

project "BigfileLogger"
    kind "SharedLib"

    language "C++"
    cppdialect "C++17"

    files "src/**"

    files { "vendor/patterns/*.cpp" }
    includedirs { "vendor/patterns" }

    symbols "On"

    filter "platforms:Win32"
        architecture "x86"
        links "MinHook.x86.lib"

    filter "platforms:Win64"
        architecture "x86_64"
        links "MinHook.x64.lib"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"