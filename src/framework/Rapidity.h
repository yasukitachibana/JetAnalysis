#ifndef RAPIDITY_H_
#define RAPIDITY_H_

#include "Particle.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================

class RapidityBase
{

public:
  virtual ~RapidityBase() {}
  virtual std::string Type() { return "None"; }

  virtual double Val(std::shared_ptr<fastjet::PseudoJet> pj) { return 100; }
  virtual double Val(fastjet::PseudoJet pj) { return 100; }

  double Val(std::shared_ptr<Particle> p) { return Val(p->GetPseudoJet()); }

}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// (Simple) Rapidity Class
//===========================================================================================================================
class RapidityY : public RapidityBase
{
public:
  RapidityY() {}
  ~RapidityY() {}
  std::string Type() { return "Y"; }
  double Val(std::shared_ptr<fastjet::PseudoJet> pj) { return pj->rapidity(); }
  double Val(fastjet::PseudoJet pj) { return pj.rapidity(); }
};

class PseudoRapidityEta : public RapidityBase
{
public:
  PseudoRapidityEta() {}
  ~PseudoRapidityEta() {}
  std::string Type() { return "Eta"; }
  double Val(std::shared_ptr<fastjet::PseudoJet> pj) { return pj->eta(); }
  double Val(fastjet::PseudoJet pj) { return pj.eta(); }
};
//===========================================================================================================================

#endif
