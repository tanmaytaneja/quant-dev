#ifndef CRTP_HPP
#define CRTP_HPP

#include "macros.hpp"

template<typename DerivedStrategy>
class BaseStrategy
{
public:
    FORCE_INLINE void execute()
    {
        static_cast<DerivedStrategy*>(this)->execute_derived();
    }
};

class DerivedStrategy : BaseStrategy<DerivedStrategy>
{
public:
    FORCE_INLINE void execute_derived()
    {
        int a = 1, b = 2;
        int c = a + b;
    }
};

#endif