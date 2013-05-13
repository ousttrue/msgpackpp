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
    "msgpack",
}
linkoptions {
}

configuration "gmake"
do
    links {
        "boost_thread-mt",
        "boost_system-mt",
    }
end

