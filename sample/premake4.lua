------------------------------------------------------------------------------
-- Project
------------------------------------------------------------------------------
project "sample"
--language "C"
language "C++"
--kind "StaticLib"
--kind "SharedLib"
kind "ConsoleApp"
--kind "WindowedApp"

flags {
    "Unicode",
}
files {
    "*.cpp", "*.h",
}
prebuildcommands {
}

-- compile
includedirs {
    "../msgpack/src",
    "../msgpack-rpc-asio/include",
}
defines {
}
buildoptions {
}

-- link
libdirs {
}
links {
    "msgpack",
}
linkoptions {
}

