#include "DeltaPhiCut.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================
double DeltaPhiCutBase::PhiFormat(double phi_in)
{
  if (phi_in < -0.0)
  {
    return pi * fabs(phi_in);
  }
  else
  {
    return phi_in;
  }
}

double DeltaPhiCutBase::DeltaPhi(double phi, double phi_b) // in range -pi .. pi
{
  double dphi = phi - phi_b;
  if (dphi > pi)
  {
    dphi -= twopi;
  }
  if (dphi < -pi)
  {
    dphi += twopi;
  }
  return dphi;
}

double DeltaPhiCutBase::DeltaPhiAbs(double phi, double phi_b) // |DeltaPhi|
{
  return fabs(DeltaPhi(phi, phi_b));
}

//---------------------------------------------------------------------------------------------------------------------------
void DeltaPhiCutBase::ShowDeltaPhiCutSetting()
{
  if (initialized)
  {
    PrintDeltaPhiCutSetting();
  }
  else
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[DeltaPhiCutBase] DeltaPhiCut is NOT initialized." << std::endl;
    std::cout << "[DeltaPhiCutBase] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
}
//===========================================================================================================================

//===========================================================================================================================
// (Simple) DeltaPhiCut Class
//===========================================================================================================================
DeltaPhiCut::DeltaPhiCut(double delta_phi_min_in)
{
  delta_phi_min = PhiFormat(delta_phi_min_in);
  initialized = 1;
}
//--------------------------------------------------------------
void DeltaPhiCut::DeltaPhiMin(double delta_phi_min_in)
{
  delta_phi_min = PhiFormat(delta_phi_min_in);
  initialized = 1;
}

void DeltaPhiCut::PhiBasis(double phi_basis_in)
{
  phi_basis = phi_basis_in;
}
//--------------------------------------------------------------
void DeltaPhiCut::PrintDeltaPhiCutSetting()
{
  std::cout << "[    DeltaPhiCut   ] ***-------------------------------------------" << std::endl;
  std::cout << "[    DeltaPhiCut   ] *** [Back-to-Back]" << std::endl;
  std::cout << "[    DeltaPhiCut   ] *** Delta Phi > " << delta_phi_min << " = " << delta_phi_min / pi << "*pi " << std::endl;
  // std::cout << "[    DeltaPhiCut   ] *** TEST " << DeltaPhiCutBase::DeltaPhi(2*pi,-pi) / pi << std::endl;
}

//-----------------------------------------------------------------
double DeltaPhiCut::DeltaPhi(double phi) // in range -pi .. pi
{
  return DeltaPhiCutBase::DeltaPhi(phi, phi_basis);
}

double DeltaPhiCut::DeltaPhiAbs(double phi) // |DeltaPhi|
{
  return DeltaPhiCutBase::DeltaPhiAbs(phi, phi_basis);
}
//-----------------------------------------------------------------
bool DeltaPhiCut::Trigger(std::shared_ptr<Particle> p)
{
  return Trigger(p->GetPseudoJet());
}

bool DeltaPhiCut::Trigger(fastjet::PseudoJet jet)
{
  // std::cout << "pTjet = " << jet.perp() << std::endl;
  // std::cout << "-> Phi = " << jet.phi() << std::endl;
  // std::cout << "-> Phi_tag = " << phi_basis << std::endl;
  // std::cout << "-> DeltaPhiAbs(phi) = " << DeltaPhiAbs(jet.phi()) << std::endl;
  // std::cout << "-> delta_phi_min = " << delta_phi_min << std::endl;
  return Trigger(jet.phi());
}

bool DeltaPhiCut::Trigger(double phi)
{
  return DeltaPhiAbs(phi) >= delta_phi_min;
}
//===========================================================================================================================
NoPhiCut::NoPhiCut()
{
  initialized = 1;
}
//===========================================================================================================================
