------------------------------------------------------------------------------
-- Project
------------------------------------------------------------------------------
project "msgpack"
--language "C"
language "C++"
kind "StaticLib"
--kind "SharedLib"
--kind "ConsoleApp"
--kind "WindowedApp"

flags {
    "Unicode",
}
files {
    "src/**.cpp", 
    "src/**.hpp",
    "src/**.c",
    "src/**.h",
}
prebuildcommands {
}

-- compile
includedirs {
}
defines {
}
buildoptions {
    "/TP",
}

-- link
libdirs {
}
links {
}
linkoptions {
}

