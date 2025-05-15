#include <string_view>
#include <iostream>

namespace util {
    constexpr uint64_t hashString(std::string_view str) {
        uint64_t hash = 0;
        for (char c : str) {
            hash = (hash * 131) + c;
        }
        return hash;
    }
}

constexpr uint64_t operator"" _hash(const char* str, size_t len) {
    return util::hashString(std::string_view(str, len));
}