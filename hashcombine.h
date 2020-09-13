
#ifndef HASHCOMBINE_H
#define HASHCOMBINE_H

#include <algorithm>
#include <functional>

template<class T>
inline void hash_combine(std::size_t &seed, const T &v)
{
    std::hash<T> hash_func;
    seed ^= hash_func(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// hash function for any std::pair of pointers
template<typename T1, typename T2>
struct hash<std::pair<T1 *, T2 *>>
{
    size_t operator()(const std::pair<T1 *, T2 *> &p) const
    {
        size_t seed = 0;
        hash_combine(seed, p.first);
        hash_combine(seed, p.second);
        return seed;
    }
};

// hash function for any std::array of pointers
template<typename T, std::size_t N>
struct hash<std::array<T *, N>>
{
    size_t operator()(const std::array<T *, N> &a) const
    {
        size_t seed = 0;
        for (size_t i = 0; i < N; ++i)
            hash_combine(seed, a[i]);
        return seed;
    }
};

#endif // HASHCOMBINE_H
