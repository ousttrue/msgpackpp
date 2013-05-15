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
    defines {
        "BOOST_TEST_DYN_LINK",
    }
    buildoptions {
        "-std=c++0x",
        "-Wno-deprecated",
    }
    links {
        "boost_thread-mt",
        "boost_system-mt",
        "boost_unit_test_framework-mt",
    }
end

configuration "vs*"
do
    includedirs {
        "$(BOOST_DIR)",
    }
    buildoptions {
        "/TP",
    }
    libdirs {
        "$(BOOST_DIR)/lib",
    }
end

configuration {}

include "test"
include "sample"
include "msgpack-rpc-asio"
include "msgpack"
