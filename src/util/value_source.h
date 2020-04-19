#pragma once

///
/// Provides values.
///
template<typename T>
class ValueSource
{
public:
  virtual ~ValueSource() {}
  virtual T next() = 0;
};
