-- PirateSense make file.

workspace "PirateSense"
    configurations { "Dev", "Release" }
    platforms "Win64"
    cppdialect "C++20"
    toolset "v143"

    filename "PirateSense"

    defines { "WIN32_LEAN_AND_MEAN" }

    filter "platforms:Win64"
        system "Windows"
        architecture "x86_64"

project "PirateSense"
    kind "SharedLib"
    language "C++"

    characterset ("MBCS")

    linkoptions { "/NODEFAULTLIB:LIBCMT", "/LTCG" }

    location "Intermediates/ProjectFiles"

    targetdir "Binaries/%{cfg.architecture}-%{cfg.buildcfg}"
    objdir "Intermediates/%{cfg.architecture}-%{cfg.buildcfg}"

    files { "PirateSense/**.h", "PirateSense/**.cpp" }

    includedirs { "PirateSense", "PirateSense/ThirdParty/kiero", "PirateSense/ThirdParty/imgui", "PirateSense/ThirdParty/minhook/include", "PirateSense/ThirdParty/discord-rpc-stuff/include" }
    libdirs { "PirateSense/ThirdParty/minhook/lib" }

    links { "libMinHook-x64-v141-mt" }

    filter "configurations:Dev"
        defines { "NDEBUG", "DEVBUILD" }

        optimize "On"
        symbols "Off"

    filter "configurations:Release"
        defines "NDEBUG"

        optimize "On"
        symbols "Off"

project "PirateLoader"
    kind "ConsoleApp"
    language "C++"

    characterset ("MBCS")

    defines "_CONSOLE"

    location "Intermediates/ProjectFiles"

    targetdir "Binaries/%{cfg.architecture}-%{cfg.buildcfg}"
    objdir "Intermediates/%{cfg.architecture}-%{cfg.buildcfg}"

    files { "PirateLoader/**.h", "PirateLoader/**.cpp" }

    includedirs { "PirateLoader" }

    filter "configurations:Dev"
        defines "NDEBUG"

        optimize "On"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"

        optimize "On"
        symbols "On"
