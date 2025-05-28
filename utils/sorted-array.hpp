#ifndef SORTED_ARRAY_HPP
#define SORTED_ARRAY_HPP

#include <cstddef>

template <typename T, size_t Capacity>
class SortedArray
{
public:
    SortedArray() noexcept : len{0} {}

    size_t size() const noexcept { return len; }
    bool empty() const noexcept { return len == 0; }

    T &at(size_t i) noexcept
    {
        return data[i];
    }

    const T &at(size_t i) const noexcept
    {
        return data[i];
    }

    const T &back() const noexcept
    {
        return data[len - 1];
    }

    T *begin() noexcept { return data; }
    T *end() noexcept { return data + len; }
    const T *begin() const noexcept { return data; }
    const T *end() const noexcept { return data + len; }

    void insert(const T &value) noexcept
    {
        size_t pos = lower_bound(value);
        for (size_t i = len; i > pos; --i)
        {
            data[i] = data[i - 1];
        }
        data[pos] = value;
        ++len;
    }

    void erase(const T &value) noexcept
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

    T *find(const T &value) noexcept
    {
        size_t pos = lower_bound(value);
        if (pos < len && value == data[pos])
        {
            return &data[pos];
        }
        return end();
    }

    const T *find(const T &value) const noexcept
    {
        size_t pos = lower_bound(value);
        if (pos < len && value == data[pos])
        {
            return &data[pos];
        }
        return end();
    }

    T extract(T *it) noexcept
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

    T pop_back() noexcept
    {
        return data[--len];
    }

    T pop_front() noexcept
    {
        T val = data[0];
        for (size_t i = 1; i < len; ++i)
        {
            data[i - 1] = data[i];
        }
        --len;
        return val;
    }

private:
    T data[Capacity];
    size_t len;

    size_t lower_bound(const T &value) const noexcept
    {
        size_t low = 0, high = len;
        while (low < high)
        {
            size_t mid = (low + high) >> 1;
            if (data[mid] < value)
            {
                low = mid + 1;
            }
            else
            {
                high = mid;
            }
        }
        return low;
    }

    void print_set() const noexcept
    {
        std::cout << "{ ";
        for(size_t i = 0; i < len; ++i)
        {
            std::cout << data[i] << ' ';
        }
        std::cout << "}" << std::endl;
    }
};

#endif