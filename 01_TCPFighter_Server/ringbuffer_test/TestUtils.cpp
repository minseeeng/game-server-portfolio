#include "TestUtils.h"

std::random_device rd;
std::mt19937 gen(rd());

int GetRandomNumber(int minInclusive, int maxInclusive)
{
    std::uniform_int_distribution<> dist(minInclusive, maxInclusive);
    return dist(gen);
}

std::string GetRandomString(int length)
{
    auto randchar = []() -> char
        {
            const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
            constexpr size_t max = (sizeof(charset) - 1);
            return charset[GetRandomNumber(0, max)];
        };

    std::string tmp(length, 0);
    std::generate_n(tmp.begin(), length, randchar);

    return tmp;
}