#pragma once

#include <QDebug>

namespace Test
{

template<typename T1>
inline bool compare(T1 t1, T1 t2) noexcept
{
    if (t1 != t2)
    {
        qDebug() << "Expected " << t1 << " but got " << t2;
        return false;
    }
    return true;
}

};
