#include "StandardReco.h"


// Register the module with the base class
RegisterReconstructionModule<StandardReco> StandardReco::reg("StandardReco");

StandardReco::StandardReco(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating StandardReco" << std::endl;
}

StandardReco::~StandardReco(){
  std::cout << "-$-Deleting StandardReco"<<std::endl;
}
