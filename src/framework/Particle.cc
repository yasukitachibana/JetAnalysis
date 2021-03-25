#include "Particle.h"


// Initialize static MakeUniqueHelper.here
Pythia8::Pythia ParticleBase::InternalHelperPythia("IntentionallyEmpty", false);


//===========================================================================================================================
// BASE
//===========================================================================================================================
ParticleBase::ParticleBase(const ParticleBase &srp)
: PseudoJet(srp) {
  pid_ = srp.pid_;
  plabel_ = srp.plabel_;
  pstat_ = srp.pstat_;
}

ParticleBase::ParticleBase(int label, int id, int stat, const FourVector &p) {
  
  set_label(label);
  set_id(id);
  set_stat(stat);
  reset_momentum(p);
  
  assert(InternalHelperPythia.particleData.isParticle(id));
  set_restmass(InternalHelperPythia.particleData.m0(id));
  
}

ParticleBase::ParticleBase(int label, int id, int stat, int fjui, const FourVector &p) {
  
  set_label(label);
  set_id(id);
  set_stat(stat);
  reset_momentum(p);
  set_fjui(fjui);
  
  assert(InternalHelperPythia.particleData.isParticle(id));
  set_restmass(InternalHelperPythia.particleData.m0(id));
  
}

ParticleBase::ParticleBase(int label, int id, int stat,
                           double pt, double eta, double phi, double e) {
  set_label(label);
  set_id(id);
  set_stat(stat);
  reset_momentum(pt * cos(phi), pt * sin(phi), pt * sinh(eta), e);
  
  assert(InternalHelperPythia.particleData.isParticle(id));
  set_restmass(InternalHelperPythia.particleData.m0(id));
  
}

ParticleBase::ParticleBase(int label, int id, int stat, int fjui,
                           double pt, double eta, double phi, double e) {
  set_label(label);
  set_id(id);
  set_stat(stat);
  reset_momentum(pt * cos(phi), pt * sin(phi), pt * sinh(eta), e);
  set_fjui(fjui);
  
  assert(InternalHelperPythia.particleData.isParticle(id));
  set_restmass(InternalHelperPythia.particleData.m0(id));
  
}

void ParticleBase::clear() {
  plabel_ = 0;
  pid_ = 0;
  pstat_ = 0;
  mass_ = -1;
}

void ParticleBase::set_label(int label) { plabel_ = label; }
void ParticleBase::set_id(int id) { pid_ = id; }
void ParticleBase::set_stat(int stat) { pstat_ = stat; }
void ParticleBase::set_restmass(double mass_input) { mass_ = mass_input;}

//  start getters
const int ParticleBase::pid() const { return (pid_); }
const int ParticleBase::pstat() const { return (pstat_); }
const int ParticleBase::plabel() const { return (plabel_); }
const FourVector ParticleBase::p_in() const {
  return (FourVector(px(), py(), pz(), e()));
}
const double ParticleBase::restmass() { return (mass_); }
const double ParticleBase::p(int i) {
  /// Deprecated. Prefer explicit component access
  // cerr << " DON'T USE ME VERY OFTEN!!" << endl;
  switch (i) {
  case 0:
    return e();
  case 1:
    return px();
  case 2:
    return py();
  case 3:
    return pz();
  default:
    throw std::runtime_error(
        "ParticleBase::p(int i) : i is out of bounds.");
  }
}

ParticleBase &ParticleBase::operator=(ParticleBase &c) {
  
  fastjet::PseudoJet::operator=(c);

  pid_ = c.pid();
  pstat_ = c.pstat();
  plabel_ = c.plabel();
  mass_ = c.mass_;

  return *this;
}

ParticleBase &ParticleBase::operator=(const ParticleBase &c) {
  
  fastjet::PseudoJet::operator=(c);

  pid_ = c.pid_;
  pstat_ = c.pstat_;
  plabel_ = c.plabel_;

  mass_ = c.mass_;

  return *this;
  
}
//===========================================================================================================================





//===========================================================================================================================
// (Simple) Particle Class
//===========================================================================================================================
Particle::Particle(const Particle &srh): ParticleBase::ParticleBase(srh) {}

Particle::Particle(int label, int id, int stat, const FourVector &p)
: ParticleBase::ParticleBase(label, id, stat, p){}
Particle::Particle(int label, int id, int stat, int fjui, const FourVector &p)
: ParticleBase::ParticleBase(label, id, stat, fjui, p){}

Particle::Particle(int label, int id, int stat, double pt, double eta, double phi, double e)
: ParticleBase::ParticleBase(label, id, stat, pt, eta, phi, e){}

Particle::Particle(int label, int id, int stat, int fjui, double pt, double eta, double phi, double e)
: ParticleBase::ParticleBase(label, id, stat, fjui, pt, eta, phi, e){}

Particle &Particle::operator=(Particle &c) {
  ParticleBase::operator=(c);
  return *this;
}

Particle &Particle::operator=(const Particle &c) {
  ParticleBase::operator=(c);
  return *this;
}
//===========================================================================================================================
