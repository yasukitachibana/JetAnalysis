#ifndef DELTAPHICUT_H_
#define DELTAPHICUT_H_

#include "Particle.h"
// #include <vector>
// #include <memory>
// #include <iostream>
// #include <sstream>
// #include <iomanip>

//===========================================================================================================================
// BASE
//===========================================================================================================================

class DeltaPhiCutBase
{

public:
  virtual ~DeltaPhiCutBase() {}
  virtual double DeltaPhiMin() { return 0.0; }
  virtual void DeltaPhiMin(double delta_phi_min_in) {}
  virtual double PhiBasis() { return 0.0; }
  virtual void PhiBasis(double phi_basis_in) {}

  void ShowDeltaPhiCutSetting();
  virtual bool Trigger(fastjet::PseudoJet jet) { return false; }  
  virtual bool Trigger(std::shared_ptr<Particle> p) { return false; }
  virtual bool Trigger(double phi) { return false; }
  virtual std::string Type() { return ""; }
  //------------------------------------------------------------------
  static double PhiFormat(double phi_in);
  static double DeltaPhi(double phi, double phi_b);    // in range -pi .. pi
  static double DeltaPhi02Pi(double phi, double phi_b); // in range 0 .. 2pi

  static double DeltaPhiAbs(double phi, double phi_b); // |DeltaPhi|
  //------------------------------------------------------------------

protected:
  int initialized = 0;

private:
  virtual void PrintDeltaPhiCutSetting() {}

}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// (Simple) DeltaPhiCut Class
//===========================================================================================================================
class DeltaPhiCut : public DeltaPhiCutBase
{

public:
  DeltaPhiCut() {}
  DeltaPhiCut(double delta_phi_min_in);
  ~DeltaPhiCut() {}
  double DeltaPhiMin() { return delta_phi_min; }
  void DeltaPhiMin(double delta_phi_min_in);
  double PhiBasis() { return phi_basis; }
  void PhiBasis(double phi_basis_in);

  bool Trigger(fastjet::PseudoJet jet);
  bool Trigger(std::shared_ptr<Particle> p);
  bool Trigger(double phi);

  std::string Type() { return "DeltaPhiCut"; }

private:
  double delta_phi_min;
  double phi_basis = 0.0;

  double DeltaPhi(double phi);    // in range -pi .. pi
  double DeltaPhi02Pi(double phi);    // in range 0 .. 2pi
  double DeltaPhiAbs(double phi); // |DeltaPhi|

  void PrintDeltaPhiCutSetting();

};

class NoPhiCut : public DeltaPhiCutBase
{
public:
  NoPhiCut();
  ~NoPhiCut() {}
  double DeltaPhiMin() { return 0.0; }
  void DeltaPhiMin(double delta_phi_min_in) {}
  double PhiBasis() { return 0.0; }
  void PhiBasis(double phi_basis_in) {}

  bool Trigger(fastjet::PseudoJet jet) { return true; }
  bool Trigger(std::shared_ptr<Particle> p) { return true; }
  bool Trigger(double phi) { return true; }
  std::string Type() { return "NoPhiCut"; }

private:
  void PrintDeltaPhiCutSetting() {}
};
//===========================================================================================================================

#endif
