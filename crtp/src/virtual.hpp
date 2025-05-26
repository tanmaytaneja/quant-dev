#ifndef VIRTUAL_HPP
#define VIRTUAL_HPP

#include "macros.hpp"

class BaseStrategy
{
public:
    FORCE_INLINE virtual void execute();
};

class DerivedStrategy : BaseStrategy
{
public:
    FORCE_INLINE void execute() override
    {
        int a = 1, b = 2;
        int c = a + b;
    }
};

#endif
