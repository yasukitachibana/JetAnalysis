#include "SubtractionModuleTemplete.h"


// Register the module with the base class
RegisterSubtractionModule<SubtractionModuleTemplete> SubtractionModuleTemplete::reg("SubtractionModuleTemplete");

SubtractionModuleTemplete::SubtractionModuleTemplete(){
  std::cout << "-@-Creating SubtractionModule Templete" << std::endl;
}

SubtractionModuleTemplete::~SubtractionModuleTemplete(){
  std::cout << "-$-Deleting SubtractionModule Templete"<<std::endl;
}
