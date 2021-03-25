#ifndef SUBTRACTIONMODULETEMPLETE_H
#define SUBTRACTIONMODULETEMPLETE_H

#include "SubtractionModuleBase.h"
#include <iostream>

class SubtractionModuleTemplete : public SubtractionModuleBase {
private:

  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterSubtractionModule<SubtractionModuleTemplete> reg;

public:
  SubtractionModuleTemplete();
  ~SubtractionModuleTemplete();
  std::string Name(){return "SubtractionModuleTemplete";}
};

#endif // SubtractionModuleTemplete_H
