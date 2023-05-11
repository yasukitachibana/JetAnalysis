#ifndef SMEARING_H
#define SMEARING_H

#include "SmearingModuleBase.h"
#include <iostream>
#include <float.h>

class NoSmearing : public SmearingModuleBase
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterSmearingModule<NoSmearing> reg;
  const std::string name;

public:
  NoSmearing(std::string name_in = "NoSmearing");
  ~NoSmearing();
  std::string Name() { return name; }
  void Init() { initialized = 1; }
  int NSampling() { return 1; }

  // Just pT------------------------------
  double Smear(double pt) { return pt; }

  // One Jet or Particle------------------------------
  std::shared_ptr<Particle>
  Smear(std::shared_ptr<Particle> particle) { return particle; }

  fastjet::PseudoJet
  Smear(fastjet::PseudoJet jet) { return jet; }

  std::shared_ptr<fastjet::PseudoJet>
  Smear(std::shared_ptr<fastjet::PseudoJet> jet) { return jet; }

  // List of Jets or Particles------------------------------
  std::vector<std::shared_ptr<Particle>>
  Smear(std::vector<std::shared_ptr<Particle>> particles) { return particles; }

  std::vector<fastjet::PseudoJet>
  Smear(std::vector<fastjet::PseudoJet> jets) { return jets; }

  std::vector<std::shared_ptr<fastjet::PseudoJet>>
  Smear(std::vector<std::shared_ptr<fastjet::PseudoJet>> jets) { return jets; }
};

class CMSxJGammaSmearing : public SmearingModuleBase
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterSmearingModule<CMSxJGammaSmearing> reg;
  const std::string name;

public:
  CMSxJGammaSmearing(std::string name_in = "CMSxJGammaSmearing");
  ~CMSxJGammaSmearing();
  std::string Name() { return name; }
  void Init();
  void ShowSmearingSetting();
  void PrintSmearingSetting();

  // Just pT------------------------------
  double Smear(double pt);

  // One Jet or Particle------------------------------
  std::shared_ptr<Particle>
  Smear(std::shared_ptr<Particle> particle);

  fastjet::PseudoJet
  Smear(fastjet::PseudoJet jet);

  std::shared_ptr<fastjet::PseudoJet>
  Smear(std::shared_ptr<fastjet::PseudoJet> jet);

  // List of Jets or Particles------------------------------
  std::vector<std::shared_ptr<Particle>>
  Smear(std::vector<std::shared_ptr<Particle>> jets);

  std::vector<fastjet::PseudoJet>
  Smear(std::vector<fastjet::PseudoJet> jets);

  std::vector<std::shared_ptr<fastjet::PseudoJet>>
  Smear(std::vector<std::shared_ptr<fastjet::PseudoJet>> jets);

private:
  void ReadParametersFromXML();
  double GetSigma(double pt);
  double SampleSmearingFactor(double pt);
  double C, S, N;
};

#endif // SMEARING_H
