# msgpackpp

`c++17` header only [msgpack](https://github.com/msgpack/msgpack/blob/master/spec.md) implementation.

# header
## msgpackpp/msgpackpp.h

* `c++17` header only msgpack implementation.
* no dependency

## msgpackpp/rpc.h

* `c++17` header only msgpack-rpc implementation.
* depends on [asio header only](http://think-async.com/Asio/index.html)

## msgpackpp/windows_pipe_transport.h

* Windows PIPE transport for `nvim --embed`
* depends on Windows.h

# ToDo

## Version 3
* [x] merge `msgpackpp` submodule. rename `msgpack-rpc-asio` to `msgpackpp`
* [ ] `c++20` co_await
* [ ] error handling
* [ ] no throw API
* [ ] use `std::span`. (using std::string_view)
* [ ] merge body_index & body_size

## Version 2

* [x] `c++11` std::promise/future
* [x] use https://github.com/ousttrue/msgpackpp backend
* [x] fix unittest

# unittest framework
- https://github.com/philsquared/Catch

# usage
[samples/basic/basic.cpp](./samples/basic/basic.cpp)

```c++
#include <msgpackpp.h>
#include <iostream>

int main(int argc, char **argv)
{
    // pack
    //auto packer = msgpackpp::packer(); // delete copy constructor
    msgpackpp::packer packer;
    packer.pack_array(4);
    packer.pack_integer(0);
    packer.pack_integer(256);
    packer.pack_str("method");
    packer.pack_map(3);
    packer.pack_str("x"); packer.pack_float(1.0f);
    packer.pack_str("y"); packer.pack_float(2.0f);
    packer.pack_str("z"); packer.pack_float(3.0f);

    // std::vector<std::uint8_t>
    auto p = packer.get_payload();

    // unpack
    auto u = msgpackpp::parser(p);
    std::cout << u.to_json() << std::endl; // json style for debug

    std::cout << u.is_array() << std::endl;
    std::cout << u[0].get_number<int>() << std::endl;
    std::cout << u[1].get_number<int>() << std::endl;
    std::cout << u[2].get_string() << std::endl;
    std::cout << u[3].is_map() << std::endl;
    std::cout << u[3]["x"].get_number<float>() << std::endl;
    std::cout << u[3]["y"].get_number<float>() << std::endl;
    std::cout << u[3]["z"].get_number<float>() << std::endl;

    return 0;
}
```

```
[0,256,"method",{"x":1,"y":2,"z":3}]
1
0
256
method
1
1
2
3
```

see [tests](tests/tests.cpp).

# implemented types

| c++ type                      | packer                     | parser                                      |
|-------------------------------|----------------------------|---------------------------------------------|
| ``(void)``                    | ``packer.pack_nil()``      | ``(parser.is_nil())``                       |
| ``bool``                      | ``packer.pack_bool(b)``    | ``parser.get_bool()``                       |
| ``char``                      | ``packer.pack_integer(n)`` | ``parser.get_number<char>()``               |
| ``short``                     | ``packer.pack_integer(n)`` | ``parser.get_number<short>()``              |
| ``int``                       | ``packer.pack_integer(n)`` | ``parser.get_number<int>()``                |
| ``long long``                 | ``packer.pack_integer(n)`` | ``parser.get_number<long long>()``          |
| ``unsigned char``             | ``packer.pack_integer(n)`` | ``parser.get_number<unsigned char>()``      |
| ``unsigned short``            | ``packer.pack_integer(n)`` | ``parser.get_number<unsigned short>()``     |
| ``unsigned int``              | ``packer.pack_integer(n)`` | ``parser.get_number<unsigned int>()``       |
| ``unsigned long long``        | ``packer.pack_integer(n)`` | ``parser.get_number<unsigned long long>()`` |
| ``float``                     | ``packer.pack_float(n)``   | ``parser.get_number<float>()``              |
| ``double``                    | ``packer.pack_double(n)``  | ``parser.get_number<double>()``             |
| ``const char *``              | ``packer.pack_str(str)``   |                                             |
| ``std::string``               | ``packer.pack_str(str)``   | ``parser.get_str()``                        |
| ``std::vector<std::uint8_t>`` | ``packer.pack_bin(bin)``   | ``parser.get_bin()``                        |
| ``std::tuple<A...>``          | ``packer << t``            | `` parser >> t``                            |

# usertype serializer & deserializer definition

## map type

depends on element name.

```cpp
struct Person
{
    std::string name;
    int age;

    bool operator==(const Person &rhs)const
    {
        return name == rhs.name && age == rhs.age;
    }
};
```

json representation.

```json
{
    "name": "hoge",
    "age": 100,
}
```

```cpp
namespace msgpackpp
{
    // Person p;
    // msgpackpp::packer packer;
    // packer << p;
    void serialize(packer &packer, const Person &p)
    {
        packer.pack_map(2) 
            << "name" << p.name
            << "age" << p.age
            ;
    }

    //  auto parser=msgpackpp::parser(msgpack_bytes);
    //  Person p;
    //  parser >> p;
    parser deserialize(const parser &u, Person &p)
    {
        auto uu = u[0];
        auto count = u.count();
        for (int i = 0; i<count; ++i)
        {
            auto key = uu.get_string();
            uu = uu.next();

            if (key == "name") {
                uu >> p.name;
            }
            else if (key == "age") {
                uu >> p.age;
            }
            else {
                // unknown key
                assert(false);
            }
            uu = uu.next();
        }
        return uu;
    }
}
```

use helper macro.

```
MPPP_MAP_SERIALIZER(Person, 2, name, age)
```

## array type

depends on element order.

```cpp
struct Point
{
    float x;
    float y;

    bool operator==(const Point &rhs)const
    {
        return x == rhs.x && y == rhs.y;
    }
};
```

json representation.

```json
[1.0, 2.0] // [x, y]
```

```cpp
namespace msgpackpp
{
    // Point p;
    // msgpackpp::packer packer;
    // packer << p;
    void serialize(packer &packer, const Point &p)
    {
        packer.pack_array(2) 
            << p.x
            << p.y
            ;
    }

    //  auto parser=msgpackpp::parser(msgpack_bytes);
    //  Point p;
    //  parser >> p;
    parser deserialize(const parser &u, Point &p)
    {
        assert(u.count() == 2);

        auto uu = u[0];

        uu >> p.x;
        uu = uu.next();

        uu >> p.y;
        uu = uu.next();

        return uu;
    }
}
```

use helper macro.
```
MPPP_ARRAY_SERIALIZER(Point, 2, x, y);
```

# procedure call

for rpc implementation.

```cpp
auto callback=[](int a, int b){ return a+b; }
auto proc = msgpackpp::make_procedurecall(callback);
msgpackpp::packer packer;
packer << std::make_tuple(1, 2);
auto result = proc(packer.get_payload());

R value;
msgpackpp::parser(result) >> value; // value=3
```

- https://github.com/msgpack-rpc/msgpack-rpc/blob/master/spec.md

```cpp
int id=1;
auto p = msgpackpp::make_rpc_request(id, "method", 1, 2, 3);
// [0, 1, "method", [1, 2, 3]]
```
