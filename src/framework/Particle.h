#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "FourVector.h"

#include "fastjet/PseudoJet.hh"
#include "Pythia8/Pythia.h"

#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>

using std::ostream;
using std::weak_ptr;


//===========================================================================================================================
// BASE
//===========================================================================================================================

class ParticleBase : protected fastjet::PseudoJet {
  friend class fastjet::PseudoJet;
  
public:
  
  
  inline void reset_momentum(const double px, const double py, const double pz,
                             const double e) {
    fastjet::PseudoJet::reset_momentum(px, py, pz, e);
  }
  
  inline void reset_momentum(const FourVector &p) {
    fastjet::PseudoJet::reset_momentum(p.x(), p.y(), p.z(), p.t());
  }

  inline void set_fjui(const int fjui) {
    fastjet::PseudoJet::set_user_index(fjui);
  }

  
  /// No implicit cast to PseudoJet is allowed, provide a conversion
  fastjet::PseudoJet GetPseudoJet() const { return PseudoJet(*this); }
  
  // import safe functions
  using fastjet::PseudoJet::px;
  using fastjet::PseudoJet::py;
  using fastjet::PseudoJet::pz;
  using fastjet::PseudoJet::e;
  using fastjet::PseudoJet::E;
  
  using fastjet::PseudoJet::phi;
  using fastjet::PseudoJet::phi_std;
  using fastjet::PseudoJet::phi_02pi;
  using fastjet::PseudoJet::rap;
  using fastjet::PseudoJet::rapidity;
  using fastjet::PseudoJet::pseudorapidity;
  using fastjet::PseudoJet::eta;
  using fastjet::PseudoJet::pt2;
  using fastjet::PseudoJet::pt;
  using fastjet::PseudoJet::perp2;
  using fastjet::PseudoJet::perp;
  using fastjet::PseudoJet::kt2;
  
  using fastjet::PseudoJet::modp2;
  using fastjet::PseudoJet::modp;
  using fastjet::PseudoJet::Et;
  using fastjet::PseudoJet::Et2;
  
  using fastjet::PseudoJet::kt_distance;
  using fastjet::PseudoJet::plain_distance;
  using fastjet::PseudoJet::squared_distance;
  using fastjet::PseudoJet::delta_R;
  using fastjet::PseudoJet::delta_phi_to;
  using fastjet::PseudoJet::beam_distance;
  
  using fastjet::PseudoJet::operator*=;
  using fastjet::PseudoJet::operator/=;
  using fastjet::PseudoJet::operator+=;
  using fastjet::PseudoJet::operator-=;
  
  using fastjet::PseudoJet::user_index;
  using fastjet::PseudoJet::set_user_index;
  using fastjet::PseudoJet::UserInfoBase;
  using fastjet::PseudoJet::InexistentUserInfo;
  
  using fastjet::PseudoJet::user_info;
  using fastjet::PseudoJet::set_user_info;
  using fastjet::PseudoJet::has_user_info;
  using fastjet::PseudoJet::user_info_ptr;
  using fastjet::PseudoJet::user_info_shared_ptr;
  
  using fastjet::PseudoJet::description;
  
  
public:
  ParticleBase() : PseudoJet(){};
  ParticleBase(const ParticleBase &srp);
  
  ParticleBase(int label, int id, int stat, const FourVector &p, double n=1.0 );
  ParticleBase(int label, int id, int stat, int fjui, //fjui: fastjet user index
               const FourVector &p, double n=1.0  );
  ParticleBase(int label, int id, int stat,
               double pt, double eta, double phi, double e, double n=1.0 );
  ParticleBase(int label, int id, int stat, int fjui, //fjui: fastjet user index
               double pt, double eta, double phi, double e, double n=1.0 );

  
  virtual ~ParticleBase(){};
  
  void clear();
  
  // Setters
  void set_label(int label);
  void set_id(int id);
  void set_stat(int stat);
  void set_n_particle(double n);
  
  //  Getters
  const int pid() const;
  const int pstat() const;
  const int plabel() const;
  const double n_particle() const;
  const FourVector p_in() const;
  const double restmass();
  const double p(int i);

  virtual ParticleBase &operator=(ParticleBase &c);
  virtual ParticleBase &operator=(const ParticleBase &c);
  
  static Pythia8::Pythia InternalHelperPythia;
  

  
protected:
  void set_restmass(double mass_input); ///< shouldn't be called from the outside, needs to be consistent with PID
  
  int pid_;    ///< particle id
  int pstat_;  ///< status of particle
  int plabel_; ///< the line number in the event record
  double mass_; ///< rest mass of the particle \todo Only maintain PID, look up mass from PDG
  double n_particle_;    ///< particle number
  
};// END BASE CLASS

//===========================================================================================================================


//===========================================================================================================================
// (Simple) Particle Class
//===========================================================================================================================
class Particle : public ParticleBase {
  
public:
  Particle(const Particle &srp);
  Particle(int label, int id, int stat, const FourVector &p, double n=1.0  );
  Particle(int label, int id, int stat, int fjui, const FourVector &p, double n=1.0  );
  Particle(int label, int id, int stat, double pt, double eta, double phi, double e, double n=1.0 );
  Particle(int label, int id, int stat, int fjui, double pt, double eta, double phi, double e, double n=1.0 );

  Particle &operator=(Particle &c);
  Particle &operator=(const Particle &c);

};
//===========================================================================================================================


#endif 
