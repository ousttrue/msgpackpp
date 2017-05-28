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

configuration "vs*"
do
    buildoptions {
        "/TP",
    }
end

