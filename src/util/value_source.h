#pragma once

///
/// Provides values.
///
template<typename T>
class ValueSource
{
public:
  // TODO have a conversion constructor for constants?
  virtual ~ValueSource() {}
  virtual T next() = 0;
};
