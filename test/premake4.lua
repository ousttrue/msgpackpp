------------------------------------------------------------------------------
-- Project
------------------------------------------------------------------------------
project "test"
--language "C"
language "C++"
--kind "StaticLib"
--kind "SharedLib"
kind "ConsoleApp"
--kind "WindowedApp"

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

