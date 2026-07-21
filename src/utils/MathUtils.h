#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QtMath>
#include <algorithm>

namespace MathUtils {

template<typename T>
inline T clamp(T value, T lo, T hi)
{
    return std::clamp(value, lo, hi);
}

inline qreal lerp(qreal a, qreal b, qreal t)
{
    return a + (b - a) * t;
}

inline qreal distance(qreal x1, qreal y1, qreal x2, qreal y2)
{
    qreal dx = x2 - x1;
    qreal dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

} // namespace MathUtils

#endif // MATHUTILS_H
