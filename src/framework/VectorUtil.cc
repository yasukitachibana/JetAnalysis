#include "VectorUtil.h"

namespace VectorUtil
{

  std::vector<double> VectorArange(double start, double stop, double step)
  {
    std::vector<double> vec(1, start);
    for (double val = start + step; val < stop; val += step)
    {
      vec.push_back(val);
    }
    return vec;
  }

}
