#include <string>
#include <string_view>
#include <type_traits>
#include <concepts>

#include "typestring/typestring.hh"
#define ts(x) typestring_is(x)

using namespace std::literals;

namespace urlparse
{

    template <class T>
    concept exactly_sv = std::same_as<std::string_view, T>;

    template <typename U, typename T>
    struct UrlGetHelper
    {
    };

    template <typename... T>
    struct UrlData
    {
    };

    template <typename T, typename... Rest>
    struct UrlData<T, Rest...>
    {
        UrlData(std::string_view first, const exactly_sv auto &...rest)
            : first(first), rest(rest...)
        {
        }

        std::string_view first;
        UrlData<Rest...> rest;

        template <typename U>
        auto get()
        {
            return UrlGetHelper<U, UrlData<T, Rest...>>::get(*this);
        }
    };

    template <typename U, typename T, typename... Rest>
    struct UrlGetHelper<U, UrlData<T, Rest...>>
    {
        static auto get(UrlData<T, Rest...> &data)
        {
            if constexpr (std::is_same<U, T>::value)
                return data.first;
            else
                return UrlGetHelper<U, UrlData<Rest...>>::get(data.rest);
        }
    };

    template <typename... T>
    struct UrlHelper
    {
    };

    template <typename... T>
    auto urlpoke(UrlHelper<T...>) -> UrlHelper<T...>;

    template <typename... X, typename... Y>
    auto urlpoke(UrlHelper<X...>, UrlHelper<irqus::typestring<':'>>, UrlHelper<Y>...)
        -> UrlHelper<X...>;

    template <typename A, typename... X, typename... Y>
    auto urlpoke(UrlHelper<X...>, UrlHelper<A>, UrlHelper<Y>...)
        -> decltype(urlpoke(UrlHelper<X..., A>(), UrlHelper<Y>()...));

    template <typename... C>
    auto urlpeek(UrlHelper<C...>)
        -> decltype(urlpoke(UrlHelper<C>()...));

    template <typename... T>
    auto urlconvert(UrlHelper<T...>)
        -> UrlData<T...>;

    consteval std::string_view parse_arg(const std::string_view str, const size_t idx)
    {
        constexpr std::string_view output = ""sv;
        size_t prev_pos = 0;
        size_t colon_pos = 0;
        size_t target = 0;

        while ((colon_pos = str.find(':', colon_pos)) != std::string::npos)
        {
            const size_t slash_pos = str.find('/', colon_pos);
            if (target == idx)
                return std::string_view(str.substr(colon_pos + 1, slash_pos - colon_pos - 1));

            target++;
            prev_pos = ++colon_pos;
        }

        return ":"sv;
    }

}

#define args(x) \
    ts(urlparse::parse_arg(x, 0)), ts(urlparse::parse_arg(x, 1)), ts(urlparse::parse_arg(x, 2)), ts(urlparse::parse_arg(x, 3))

#define args_is(x) \
    decltype(urlparse::urlconvert(urlparse::urlpeek(urlparse::UrlHelper<args(x)>())))
