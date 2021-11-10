#ifndef LeadingParticle_H_
#define LeadingParticle_H_

#include "fastjet/PseudoJet.hh"
#include "Pythia8/Pythia.h"

#include "Particle.h"
// #include <vector>
// #include <memory>
// #include <iostream>
// #include <sstream>
// #include <iomanip>

//===========================================================================================================================
// BASE
//===========================================================================================================================

class LeadingParticleBase
{

public:
  virtual ~LeadingParticleBase() {}
  // virtual bool Trigger(std::shared_ptr<Particle> p){return false;}
  // bool LeadingParticleCheck(std::shared_ptr<Particle> p);
  virtual std::string Type() { return "None"; }

protected:
  static Pythia8::Pythia InternalHelperPythia;
}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// (Simple) LeadingParticle Class
//===========================================================================================================================
// class LeadingParticleOnly : public LeadingParticleBase
// {

// public:
//   LeadingParticleOnly(){}
//   ~LeadingParticleOnly(){}
//   bool Trigger(std::shared_ptr<Particle> p);
//   std::string Type(){return "LeadingParticle";}
// };

class LeadingInclusive : public LeadingParticleBase
{
public:
  LeadingInclusive() {}
  ~LeadingInclusive() {}
  //   bool Trigger(std::shared_ptr<Particle> p){return true;}
  std::string Type() { return "Full"; }
};
//===========================================================================================================================

#endif
