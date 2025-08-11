#ifndef LeadingParticle_H_
#define LeadingParticle_H_

#include "fastjet/PseudoJet.hh"
#include "Pythia8/Pythia.h"

#include "Particle.h"
#include "Charged.h"
// #include <vector>
// #include <memory>
// #include <iostream>
// #include <sstream>
// #include <iomanip>

namespace LeadingParticleHelper
{
  bool LeadingConst(std::string method_in);

  static const std::vector<std::string> cNames =
      {"",
       "Const", "const",
       "Con", "con",
       "C", "c",
       "Constituent", "constituent",
       "Constituents", "constituens"};

  static const std::vector<std::string> aNames =
      {"Area", "area", "Are", "are",
       "A", "a", "R", "r",
       "InCone", "inCone", "Incone", "incone"};
}

//===========================================================================================================================
// BASE
//===========================================================================================================================

class LeadingParticleBase
{

public:
  LeadingParticleBase();
  virtual ~LeadingParticleBase() {}
  virtual void Init() {}
  virtual void Init(double leading_pt_min_in) {}
  virtual void SetEvent(std::vector<std::shared_ptr<Particle>> particle_list_in) {}
  virtual bool Trigger(fastjet::PseudoJet jet, double r_cone) { return false; }
  virtual std::string Type() { return "None"; }

  bool Initialized() const { return initialized; }
  void ShowLeadingParticleSetting();
  virtual void PrintLeadingParticleSetting() {}

protected:
  bool initialized;

}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// NoLeading Class
//===========================================================================================================================

class NoLeading : public LeadingParticleBase
{
public:
  NoLeading();
  ~NoLeading() {}
  std::string Type() { return "No Leading Particle Requirement"; }
  void Init();
  void Init(double leading_pt_min_in);
  void SetEvent(std::vector<std::shared_ptr<Particle>> particle_list_in) {}  
  bool Trigger(fastjet::PseudoJet jet, double r_cone) { return true; }
  void PrintLeadingParticleSetting() {}
};

//===========================================================================================================================
// LeadingRequirement Class
//===========================================================================================================================

class LeadingRequirement : public LeadingParticleBase
{
public:
  LeadingRequirement();
  ~LeadingRequirement() {}
  virtual std::string Type() { return "None [LeadingRequirement]"; }
  virtual bool Trigger(fastjet::PseudoJet jet, double r_cone) { return false; }
  virtual void Init(double leading_pt_min_in) {}
  virtual void SetEvent(std::vector<std::shared_ptr<Particle>> particle_list_in) {}  
  virtual void PrintLeadingParticleSetting() {}

protected:
  double leading_pt_min;
};

class LeadingConstituent : public LeadingRequirement
{
public:
  LeadingConstituent();
  ~LeadingConstituent() {}
  std::string Type() { return "Leading Jet Constituent"; }
  bool Trigger(fastjet::PseudoJet jet, double r_cone);
  void Init(double leading_pt_min_in);
  void SetEvent(std::vector<std::shared_ptr<Particle>> particle_list_in) {}  
  void PrintLeadingParticleSetting();
};

class LeadingInCone : public LeadingRequirement
{
public:
  LeadingInCone();
  ~LeadingInCone() {}
  std::string Type() { return "Leading Particle In Jet Cone Area"; }
  bool Trigger(fastjet::PseudoJet jet, double r_cone);
  void Init(double leading_pt_min_in);
  void SetEvent(std::vector<std::shared_ptr<Particle>> particle_list_in);  
  void PrintLeadingParticleSetting();

private:
  std::unique_ptr<ChargedBase> lead_charged_ptr;
  std::vector<std::shared_ptr<Particle>> event_particle_list;
};

//===========================================================================================================================

#endif
