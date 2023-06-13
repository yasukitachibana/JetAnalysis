#ifndef MASSADJUST_H_
#define MASSADJUST_H_

#include "Particle.h"
// #include <vector>
// #include <memory>
// #include <iostream>
// #include <sstream>
// #include <iomanip>

namespace MassAdjHelper
{
  bool AdjustEnergy(std::string adj_quant_in);

  static const std::vector<std::string> eNames =
      {"",
       "ENE", "Ene", "ene",
       "ENERGY", "Energy", "energy",
       "E", "e", "p0", "P0"};

  static const std::vector<std::string> pNames =
      {"MOM", "Mom", "mom",
       "MOMENTUM", "Momentum", "momentum",
       "P", "p", "pvec", "vecp", "Pvec", "vecP"};

}

//===========================================================================================================================
// BASE
//===========================================================================================================================

class MassAdjustBase
{

public:
  MassAdjustBase();
  virtual ~MassAdjustBase() {}
  virtual std::string Type() { return "None"; }
  virtual double Mass() { return -1; }
  virtual void Init() {}
  virtual void Init(double mass_in) {}

  bool Initialized() const { return initialized; }
  void ShowMassAdjustSetting();
  virtual void PrintMassAdjustSetting() {}

  virtual std::shared_ptr<Particle> Adjust(std::shared_ptr<Particle> p) { return p; }

protected:
  bool initialized;
}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// (Simple) NoMassAdjust Class
//===========================================================================================================================
class NoMassAdjust : public MassAdjustBase
{

public:
  NoMassAdjust();
  ~NoMassAdjust() {}
  std::string Type() { return "NoMassAdjust"; }
  double Mass() { return -1; } // return negative value no mass adjust is set
  void Init();
  void Init(double mass_in);
  void PrintMassAdjustSetting() {}
  std::shared_ptr<Particle> Adjust(std::shared_ptr<Particle> p) { return p; }
};

//===========================================================================================================================
// (Simple) MassAdjust Class
//===========================================================================================================================
class AdjustByMass : public MassAdjustBase
{
public:
  AdjustByMass();
  ~AdjustByMass() {}
  virtual std::string Type() { return "None [AdjustByMass]"; }
  double GetMass() { return sqrt(mass2); } // return mass used for the adjustment
  void Init(double mass_in);
  double SetMass(double mass_in);
  void PrintMassAdjustSetting();
  std::shared_ptr<Particle> Adjust(std::shared_ptr<Particle> p) { return p; }

protected:
  double mass2;

private:
  virtual std::string AdjustedQunat() { return ""; }
};

class AdjustEnergyByMass : public AdjustByMass
{
public:
  AdjustEnergyByMass();
  ~AdjustEnergyByMass() {}
  std::string Type() { return "AdjustEnergyByMass"; }
  std::shared_ptr<Particle> Adjust(std::shared_ptr<Particle> p);

private:
  std::string AdjustedQunat() { return "Energy"; }
};

class AdjustMomentumByMass : public AdjustByMass
{
public:
  AdjustMomentumByMass();
  ~AdjustMomentumByMass() {}
  std::string Type() { return "AdjustMomentumByMass"; }
  std::shared_ptr<Particle> Adjust(std::shared_ptr<Particle> p);

private:
  std::string AdjustedQunat() { return "Momentum"; }
};

//===========================================================================================================================

#endif
