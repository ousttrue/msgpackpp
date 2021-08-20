# msgpack-rpc-asio

msgpack-rpc for windows.

## Version 2

* [x] `c++11` std::promise/future
* [ ] `c++20` co_await
* [x] use https://github.com/ousttrue/msgpackpp backend

## ChangeLog
* 20210816: replace premake to cmake(vscode settings).
* 20170528: remove boost dependencies. use [asio standalone](https://github.com/chriskohlhoff/asio), [Catch](https://github.com/philsquared/Catch).
* 20170528: fix vs2017 build.

## Build

* Open folder by vscode. `CMake: Configure`, `CMake: Build`
