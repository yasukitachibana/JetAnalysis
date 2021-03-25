#include "ParticleSpec.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<ParticleSpec> ParticleSpec::reg("ParticleSpec");

ParticleSpec::ParticleSpec(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

ParticleSpec::~ParticleSpec(){
  std::cout << "-$-Deleting " << name << std::endl;
}
