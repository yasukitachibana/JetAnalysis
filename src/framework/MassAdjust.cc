#include "MassAdjust.h"
#include <float.h>

bool MassAdjHelper::AdjustEnergy(std::string adj_quant_in)
{

  //--------------------------------------------------------
  // Check if the adjusted quantity is energy
  for (auto name : eNames)
  {
    if (adj_quant_in == name)
    {
      return true;
    }
  }

  //--------------------------------------------------------
  // Check if the adjusted quantity is momentum
  for (auto name : pNames)
  {
    if (adj_quant_in == name)
    {
      return false;
    }
  }

  //--------------------------------------------------------
  // If the adjusted quantity is neither energy nor momentum, show error message and exit.
  std::cout << std::endl;
  std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
  std::cout << "[MassAdjust] Bad Input for adjustedQuant" << std::endl;
  std::cout << "[MassAdjust] Exit. " << std::endl;
  std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
  std::cout << std::endl;
  exit(-1);
}

//===========================================================================================================================
// BASE
//===========================================================================================================================
MassAdjustBase::MassAdjustBase()
{
  initialized = false;
}

void MassAdjustBase::ShowMassAdjustSetting()
{
  if (initialized)
  {
    PrintMassAdjustSetting();
  }
  else
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[MassAdjustBase] MassAdjust is NOT initialized." << std::endl;
    std::cout << "[MassAdjustBase] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
}
//===========================================================================================================================

//===========================================================================================================================
// NoMassAdjust
NoMassAdjust::NoMassAdjust() : MassAdjustBase() {}
void NoMassAdjust::Init()
{
  initialized = true;
}
void NoMassAdjust::Init(double mass_in)
{
  Init();
}

//===========================================================================================================================
// AdjustByMass
AdjustByMass::AdjustByMass() : MassAdjustBase() {}
void AdjustByMass::Init(double mass_in)
{
  mass2 = SetMass(mass_in);
  mass2 = mass2 * mass2;
  initialized = true;
}

double AdjustByMass::SetMass(double mass_in)
{
  // If mass_in is negative, mass_in is the particle id with mass for the adjustment
  if (mass_in < -DBL_MIN)
  {
    return Particle::InternalHelperPythia.particleData.m0(mass_in);
  }
  else
  {
    return mass_in;
  }
}

void AdjustByMass::PrintMassAdjustSetting()
{
  std::cout << "[    MassAdjust    ] ***-------------------------------------------" << std::endl;
  std::cout << "[    MassAdjust    ] *** [Particles' Masses]" << std::endl;
  std::cout << "[    MassAdjust    ] *** Mass of all particles are set to " << GetMass() << " GeV" << std::endl;
  std::cout << "[    MassAdjust    ] *** by adjusting " << AdjustedQunat() << std::endl;
}

//===========================================================================================================================
// AdjustEnergyByMass
AdjustEnergyByMass::AdjustEnergyByMass() : AdjustByMass() {}
std::shared_ptr<Particle> AdjustEnergyByMass::Adjust(std::shared_ptr<Particle> p)
{
  //p->PrintInfo();
  double e_adjusted = sqrt(mass2 + p->modp2()); //e after adjustment
  p->reset_momentum(p->px(), p->py(), p->pz(), e_adjusted);
  //p->PrintInfo();
  return p;
}
//===========================================================================================================================
// AdjustMomentumByMass
AdjustMomentumByMass::AdjustMomentumByMass() : AdjustByMass() {}
std::shared_ptr<Particle> AdjustMomentumByMass::Adjust(std::shared_ptr<Particle> p)
{
  //p->PrintInfo();
  double modp2_original = p->modp2();
  double e = p->e();
  double modp2_adjusted = e * e - mass2;//|\vec{p}| after adjustment

  if (modp2_adjusted > 0.0 && modp2_original > 0.0)
  {
    //Time- or light-like particle
    double f_adj = sqrt(modp2_adjusted / modp2_original);
    p->reset_momentum(f_adj * p->px(), f_adj * p->py(), f_adj * p->pz(), e);
  }
  else 
  {
    //Space-like particle
    p->reset_momentum(0.0, 0.0, 0.0, GetMass());
  }
  //p->PrintInfo();
  return p;
}