------------------------------------------------------------------------------
-- Solution
------------------------------------------------------------------------------
solution "msgpack-rpc-asio"
configurations { "Debug", "Release" }

configuration "Debug"
do
	defines { "DEBUG" }
	flags { "Symbols", }
	targetdir "debug"
end

configuration "Release"
do
	defines { "NDEBUG" }
	flags { "Optimize", }
	targetdir "release"
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
        "BOOST_TEST_DYN_LINK",
    }
    links {
        "boost_thread-mt",
        "boost_system-mt",
        "boost_unit_test_framework-mt",
    }
end

configuration { "gmake", "windows", "debug" }
do
    links {
        "boost_thread-mgw47-mt-d-1_54",
        "boost_system-mgw47-mt-d-1_54",
        "boost_unit_test_framework-mgw47-mt-d-1_54",
        "ws2_32",
        "Mswsock",
    }
end

configuration { "gmake", "windows", "release" }
do
    links {
        "boost_thread-mgw47-mt-1_54",
        "boost_system-mgw47-mt-1_54",
        "boost_unit_test_framework-mgw47-mt-1_54",
        "ws2_32",
        "Mswsock",
    }
end

configuration "Windows"
do
    includedirs {
        "$(BOOST_DIR)",
    }
    defines {
        "_WIN32_WINNT=0x0500",
    }
    libdirs {
        "$(BOOST_DIR)/lib",
        "$(BOOST_DIR)/stage/lib",
    }
end

configuration "vs*"
do
    buildoptions {
        "/TP",
        "/wd4800",
    }
end

configuration {}

include "test"
include "sample"
include "msgpack-rpc-asio"
include "msgpack"

