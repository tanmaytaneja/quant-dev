#ifndef SORTED_ARRAY_HPP
#define SORTED_ARRAY_HPP

#define EXTRACT 1

#include <cstddef>

template <typename T, size_t Capacity>
class SortedArray
{
public:
    SortedArray() : len{0} {}

    size_t size() const { return len; }
    bool empty() const { return len == 0; }

    T &at(size_t i)
    {
        return data[i];
    }

    const T &at(size_t i) const
    {
        return data[i];
    }

    const T &back() const
    {
        return data[len - 1];
    }

    T *begin() { return data; }
    T *end() { return data + len; }
    const T *begin() const { return data; }
    const T *end() const { return data + len; }

    void insert(const T &value)
    {
        size_t pos = lower_bound(value);
        for (size_t i = len; i > pos; --i)
        {
            data[i] = data[i - 1];
        }
        data[pos] = value;
        ++len;
    }

    void erase(const T &value)
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

    T *find(const T &value)
    {
        size_t pos = lower_bound(value);
        if (pos < len && value == data[pos])
        {
            return &data[pos];
        }
        return end();
    }

    const T *find(const T &value) const
    {
        size_t pos = lower_bound(value);
        if (pos < len && value == data[pos])
        {
            return &data[pos];
        }
        return end();
    }

#if EXTRACT == 1
    T extract(T *it)
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
#endif

private:
    T data[Capacity];
    size_t len;

    size_t lower_bound(const T &value) const
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

    void print_set() const
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