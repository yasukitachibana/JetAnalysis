#ifndef ANALYSISMODULETEMPLETE_H
#define ANALYSISMODULETEMPLETE_H

#include "AnalysisModuleBase.h"
#include <iostream>

class AnalysisModuleTemplete : public AnalysisModuleBase {
private:

  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<AnalysisModuleTemplete> reg;

public:
  AnalysisModuleTemplete();
  ~AnalysisModuleTemplete();
};

#endif // AnalysisModuleTemplete_H
