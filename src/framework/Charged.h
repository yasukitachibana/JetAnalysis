#ifndef CHARGED_H_
#define CHARGED_H_

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

class ChargedBase
{

public:
  virtual ~ChargedBase(){}
  virtual bool Trigger(std::shared_ptr<Particle> p){return false;}
  bool ChargedCheck(std::shared_ptr<Particle> p);
  virtual std::string Type(){return "None";}
protected:
  static Pythia8::Pythia InternalHelperPythia;
}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// (Simple) Charged Class
//===========================================================================================================================
class ChargedOnly : public ChargedBase
{

public:
  ChargedOnly(){}
  ~ChargedOnly(){}
  bool Trigger(std::shared_ptr<Particle> p);
  std::string Type(){return "Charged";}
};

class IncludeNeutral : public ChargedBase
{
public:
  IncludeNeutral(){}
  ~IncludeNeutral(){}
  bool Trigger(std::shared_ptr<Particle> p){return true;}
  std::string Type(){return "Full";}  
};
//===========================================================================================================================

#endif
