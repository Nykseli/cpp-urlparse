#include <iostream>
#include <functional>

#include "urlparse.hh"

template <typename ret, typename par>
using url_fn = std::function<ret(par)>;

template <typename T>
void register_endpoint(const std::string_view endpoint, url_fn<void, T> f)
{
}

int main()
{
    constexpr std::string_view url = "/user/:username";
    register_endpoint<args_is(url)>(url, [](args_is(url) type) {
        std::cout << type.get<ts("username")>() << std::endl; // Ok
        // std::cout << type.get<ts("foo")>() << std::endl; // Compile error
    });
    return 0;
}
