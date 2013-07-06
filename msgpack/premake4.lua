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
    "src",
}
defines {
}
buildoptions {
}

-- link
libdirs {
}
links {
}
linkoptions {
}

