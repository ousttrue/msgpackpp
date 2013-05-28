#pragma once

namespace msgpack {

// char*
template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const char *str)
{
    int size=strlen(str);
    o.pack_raw(size);
    o.pack_raw_body(str, size);
    return o;
}

/*
// wchar_t*
template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const wchar_t *str)
{
    int size=wcslen(str) * sizeof(str);
    o.pack_raw(size);
    o.pack_raw_body(str, size);
    return o;
}
*/

}
