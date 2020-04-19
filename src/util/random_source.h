#pragma once

///
/// Provides random quantities.
///
template<typename T>
class RandomSource
{
public:
  virtual ~RandomSource() {}
  virtual T next() = 0;
};
