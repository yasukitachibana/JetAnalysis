#ifndef VECTORUTIL_H_
#define VECTORUTIL_H_

#include <vector>

namespace VectorUtil
{

  std::vector<double> VectorArange(double start, double stop, double step = 1.0);

  std::vector<int> VectorArange(int start, int stop, int step = 1);  

}

#endif