#ifndef SORTED_ARRAY_HPP
#define SORTED_ARRAY_HPP

#define SIMD_MEMMOVE 0 //! Taking more time.

#include "macros.hpp"

#include <cstddef>
#include <cstring>

namespace TT
{
    template <typename T, size_t Capacity>
    class SortedArray
    {
    public:
        SortedArray() noexcept : len{0} {}

        FORCE_INLINE size_t size() const noexcept { return len; }
        FORCE_INLINE bool empty() const noexcept { return len == 0; }

        FORCE_INLINE T &at(size_t i) noexcept
        {
            return data[i];
        }

        FORCE_INLINE const T &at(size_t i) const noexcept
        {
            return data[i];
        }

        FORCE_INLINE const T &back() const noexcept
        {
            return data[len - 1];
        }

        FORCE_INLINE T *begin() noexcept { return data; }
        FORCE_INLINE T *end() noexcept { return data + len; }
        FORCE_INLINE const T *begin() const noexcept { return data; }
        FORCE_INLINE const T *end() const noexcept { return data + len; }

        FORCE_INLINE void insert(const T &value) noexcept
        {
            size_t pos = lower_bound(value);
            for (size_t i = len; i > pos; --i)
            {
                data[i] = data[i - 1];
            }
            data[pos] = value;
            ++len;
        }

        FORCE_INLINE void erase(const T &value) noexcept
        {
            size_t pos = lower_bound(value);
            if (pos < len && value == data[pos])
            {
                for (size_t i = pos; i < len - 1; ++i)
                {
                    data[i] = data[i + 1];
                }
                --len;
            }
        }

        FORCE_INLINE T *find(const T &value) noexcept
        {
            size_t pos = lower_bound(value);
            if (pos < len && value == data[pos])
            {
                return &data[pos];
            }
            return end();
        }

        FORCE_INLINE const T *find(const T &value) const noexcept
        {
            size_t pos = lower_bound(value);
            if (pos < len && value == data[pos])
            {
                return &data[pos];
            }
            return end();
        }

        FORCE_INLINE T extract(T *it) noexcept
        {
            size_t pos = it - data;
            T value = data[pos];
            for (size_t i = pos; i < len - 1; ++i)
            {
                data[i] = data[i + 1];
            }
            --len;
            return value;
        }

        FORCE_INLINE T pop_back() noexcept
        {
            return data[--len];
        }

        FORCE_INLINE T pop_front() noexcept
        {
            T val = data[0];
            for (size_t i = 1; i < len; ++i)
            {
                data[i - 1] = data[i];
            }
            --len;
            return val;
        }

        FORCE_INLINE void clear() noexcept
        {
            len = 0;
        }

    private:
        T data[Capacity];
        size_t len;

        FORCE_INLINE size_t lower_bound(const T &value) const noexcept
        {
            size_t low = 0, high = len;
            while (low < high)
            {
                size_t mid = (low + high) >> 1;
                bool cond = data[mid] < value;
                low = cond ? mid + 1 : low;
                high = cond ? high : mid;
            }
            return low;
        }

        FORCE_INLINE void print_set() const noexcept
        {
            std::cout << "{ ";
            for (size_t i = 0; i < len; ++i)
            {
                std::cout << data[i] << ' ';
            }
            std::cout << "}" << std::endl;
        }
    };
}

#endif