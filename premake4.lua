------------------------------------------------------------------------------
-- Solution
------------------------------------------------------------------------------
solution "msgpack-rpc-asio"
configurations { "Debug", "Release" }

configuration "Debug"
do
	defines { "DEBUG" }
	symbols "On"
	targetdir "debug"
end

configuration "Release"
do
	defines { "NDEBUG" }
	flags { "Optimize", }
	targetdir "release"
end

configuration {}
do
    characterset "MBCS"

    -- asio standalone
    includedirs {
        "asio/asio/include",
        "catch",
    }
    defines {
        "ASIO_STANDALONE",
    }
end

configuration "gmake"
do
    buildoptions {
        "-std=c++0x",
        --"-Wno-deprecated",
    }
end

configuration {"gmake", "linux" }
do
    defines {
    }
    links {
    }
end

configuration { "gmake", "windows", "debug" }
do
    links {
        "ws2_32",
        "Mswsock",
    }
end

configuration { "gmake", "windows", "release" }
do
    links {
        "ws2_32",
        "Mswsock",
    }
end

configuration "Windows"
do
    defines {
        "_WIN32_WINNT=0x0501",
    }
    libdirs {
    }
end

configuration "vs*"
do
    buildoptions {
        "/wd4800",
        "/wd4996",
    }
end

configuration {}

include "client"
include "test"
include "sample"
include "msgpack-rpc-asio"
include "msgpack"

