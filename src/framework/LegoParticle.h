#ifndef LegoParticle_H
#define LegoParticle_H

#include "Particle.h"

#include <vector>
class LegoParticle {
  
public:
  LegoParticle(){}
  ~LegoParticle(){}
  
  void Load( std::string filename );
  std::vector<std::shared_ptr<Particle>> GetParticles();
  void Clear();
  
private:
  
};
#endif
