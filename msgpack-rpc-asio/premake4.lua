------------------------------------------------------------------------------
-- Project
------------------------------------------------------------------------------
project "msgpack-rpc-asio"
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
    "include/**.h",
}
prebuildcommands {
}

-- compile
includedirs {
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

