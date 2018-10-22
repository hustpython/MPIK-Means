#ifndef NULL_H
#define NULL_H
#include "types.hpp"
#include<limits>

template <class Type>
class Null;

template <>
class Null<Size> {
public:
    Null() {}
    operator Size() const {
        return Size(NULL_SIZE);
    }
};

template<>
class Null<Real> {
public:
    Null() {}
    operator Real() const {
        return Real(NULL_REAL);
    }
};
#endif