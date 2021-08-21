# msgpack-rpc-asio

`headeronly` msgpack-rpc `<msgpack_rpc.h>`

<https://github.com/msgpack-rpc/msgpack-rpc/blob/master/spec.md>

## dependencies

* `headeronly` [asio standalone](http://think-async.com/Asio/index.html) `<asio.hpp> etc...`
* `headeronly` [msgpackpp](https://github.com/ousttrue/msgpackpp) `<msgpackpp.h>`

## Version 2

* [x] `c++11` std::promise/future
* [ ] `c++20` co_await
* [x] use https://github.com/ousttrue/msgpackpp backend
* [x] fix unittest
* [ ] error handling

## Build

* Open folder by vscode. `CMake: Configure`, `CMake: Build`
