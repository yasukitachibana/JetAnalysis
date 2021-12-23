#ifndef NAIVENEGATIVESUB_H
#define NAIVENEGATIVESUB_H

#include "SubtractionModuleBase.h"
#include <iostream>
#include <float.h>

class NaiveNegativeSub : public SubtractionModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterSubtractionModule<NaiveNegativeSub> reg;
  const std::string name;
  
public:
  NaiveNegativeSub(std::string name_in = "NaiveNegativeSub");
  ~NaiveNegativeSub();
  std::string Name(){return name;}
  
  std::vector <fastjet::PseudoJet>
  JetSub(double jetR,
         std::vector <fastjet::PseudoJet> jets,
         std::vector<std::shared_ptr<Particle>> particle_list );
  
  std::vector < std::shared_ptr<fastjet::PseudoJet> >
  JetSub(double jetR,
         std::vector < std::shared_ptr<fastjet::PseudoJet> > jets,
         std::vector<std::shared_ptr<Particle>> particle_list );
  
  
  
  double ptSub( std::shared_ptr<Particle> particle );
  double eSub( std::shared_ptr<Particle> particle );
  double nSub( std::shared_ptr<Particle> particle );
  double pzSub( std::shared_ptr<Particle> particle );
  double chargeSub( std::shared_ptr<Particle> particle );
  double baryonSub( std::shared_ptr<Particle> particle );
  double strangeSub( std::shared_ptr<Particle> particle );
  double charmSub( std::shared_ptr<Particle> particle );

  
};

#endif // NAIVENEGATIVESUB_H
