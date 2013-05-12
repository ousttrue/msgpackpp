------------------------------------------------------------------------------
-- Project
------------------------------------------------------------------------------
project "sample"
--language "C"
language "C++"
--kind "StaticLib"
--kind "SharedLib"
--kind "ConsoleApp"
kind "WindowedApp"

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
    "../msgpack-asiorpc/include",
}
defines {
}
buildoptions {
}

-- link
libdirs {
}
links {
    "boost_thread-mt",
    "boost_system-mt",
    "msgpack",
}
linkoptions {
}

