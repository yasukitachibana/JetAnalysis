#ifndef LegoParticle_H
#define LegoParticle_H

#include "Particle.h"


class LegoParticle {
  
public:
  LegoParticle(){}
  ~LegoParticle(){}
  
  void Load( std::string filename );
  std::vector<std::shared_ptr<Particle>> GetParticles();
  void Clear();
  
  static Pythia8::Pythia InternalHelperPythia;
  
private:
  std::stringstream input_str_stream;
  
};
#endif
