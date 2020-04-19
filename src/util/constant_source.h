#pragma once

#include "value_source.h"

template<typename T>
class ConstantSource final: public ValueSource<T>
{
private:
  const T value;
public:
  ConstantSource(T &&value) : value(value) {}
  T next() override final { return value; }
};
