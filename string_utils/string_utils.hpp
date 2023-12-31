#include <string>
#include <string_view>
#include <sstream>

namespace strutils {

template<typename CharT>
using tstring = std::basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT>>;
template<typename CharT>
using tstringstream = std::basic_stringstream<CharT, std::char_traits<CharT>, std::allocator<CharT>>;

template<typename CharT>
inline tstring<CharT>
to_upper(tstring<CharT> text)
{
    std::transform(std::begin(text), std::end(text), std::begin(text), ::toupper);
    return text;
}

template<typename CharT>
inline tstring<CharT>
to_lower(tstring<CharT> text)
{
    std::transform(std::begin(text), std::end(text), std::begin(text), ::tolower);
    return text;
}

template<typename CharT>
inline tstring<CharT>
reverse(tstring<CharT> text)
{
    std::reverse(std::begin(text), std::end(text));
    return text;
}

template<typename CharT>
inline tstring<CharT>
trim(tstring<CharT> const& text)
{
    auto first{text.find_first_not_of(' ')};
    auto last{text.find_last_not_of(' ')};
    return text.substr(first, (last - first + 1));
}

template<typename CharT>
inline tstring<CharT>
trim(tstring<CharT> const& text, tstring<CharT> const& chars)
{
    auto first{text.find_first_not_of(chars)};
    auto last{text.find_last_not_of(chars)};
    return text.substr(first, (last - first + 1));
}

template<typename CharT>
inline tstring<CharT>
trimleft(tstring<CharT> const& text)
{
    auto first{text.find_first_not_of(' ')};
    return text.substr(first, text.size() - first);
}

template<typename CharT>
inline tstring<CharT>
trimleft(tstring<CharT> const& text, tstring<CharT> const& chars)
{
    auto first{text.find_first_not_of(chars)};
    return text.substr(first, text.size() - first);
}

template<typename CharT>
inline tstring<CharT>
trimright(tstring<CharT> const& text)
{
    auto last{text.find_last_not_of(' ')};
    return text.substr(0, last + 1);
}

template<typename CharT>
inline tstring<CharT>
trimright(tstring<CharT> const& text, tstring<CharT> const& chars)
{
    auto last{text.find_last_not_of(chars)};
    return text.substr(0, last + 1);
}

template<typename CharT>
inline tstring<CharT> remove(tstring<CharT> text, CharT const ch)
{
    auto start = std::remove_if(std::begin(text), std::end(text),
        [ch](CharT const c) {
            return c == ch;
        }
    );
    text.erase(start, std::end(text));
    return text;
}

template<typename CharT>
inline std::vector<tstring<CharT>>
split(tstring<CharT> text, CharT const delimiter)
{
    auto sstr = tstringstream<CharT>{text};
    auto tokens = std::vector<tstring<CharT>>{};
    auto token = tstring<CharT>{};
    while (std::getline(sstr, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::string_view trim_view(std::string_view text)
{
    auto first{text.find_first_not_of(" ")};
    auto last{text.find_last_not_of(" ")};
    text.remove_suffix(text.length() - last - 1);
    text.remove_prefix(first);
    return text;
}

}