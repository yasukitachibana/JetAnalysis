#ifndef JETSPEC_H
#define JETSPEC_H

#include "AnalysisModuleBase.h"
#include <iostream>

class JetSpec : public AnalysisModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<JetSpec> reg;
  const std::string name;
  
public:
  JetSpec(std::string name_in = "JetSpec");
  ~JetSpec();
  std::string Name(){return name;}
  

};

#endif // JetSpec_H
