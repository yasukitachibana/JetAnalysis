#include "Particle.h"

// Initialize static MakeUniqueHelper.here
Pythia8::Pythia ParticleBase::InternalHelperPythia("IntentionallyEmpty", false);

//===========================================================================================================================
// BASE
//===========================================================================================================================
ParticleBase::ParticleBase(const ParticleBase &srp)
    : PseudoJet(srp)
{
  pid_ = srp.pid_;
  plabel_ = srp.plabel_;
  pstat_ = srp.pstat_;
  mass_ = srp.mass_;
  n_particle_ = srp.n_particle_;
  property_ = srp.property_;
}

ParticleBase::ParticleBase(int label, int id, int stat, const FourVector &p, double n)
{

  set_label(label);
  set_id(id);
  set_stat(stat);
  reset_momentum(p);
  set_n_particle(n);

  assert(InternalHelperPythia.particleData.isParticle(id));
  set_restmass(InternalHelperPythia.particleData.m0(id));
  set_property(id);
}

ParticleBase::ParticleBase(int label, int id, int stat, int fjui, const FourVector &p, double n)
{

  set_label(label);
  set_id(id);
  set_stat(stat);
  set_fjui(fjui);
  reset_momentum(p);
  set_n_particle(n);

  assert(InternalHelperPythia.particleData.isParticle(id));
  set_restmass(InternalHelperPythia.particleData.m0(id));
  set_property(id);
}

ParticleBase::ParticleBase(int label, int id, int stat,
                           double pt, double eta, double phi, double e, double n)
{
  set_label(label);
  set_id(id);
  set_stat(stat);
  reset_momentum(pt * cos(phi), pt * sin(phi), pt * sinh(eta), e);
  set_n_particle(n);

  assert(InternalHelperPythia.particleData.isParticle(id));
  set_restmass(InternalHelperPythia.particleData.m0(id));
  set_property(id);
}

ParticleBase::ParticleBase(int label, int id, int stat, int fjui,
                           double pt, double eta, double phi, double e, double n)
{
  set_label(label);
  set_id(id);
  set_stat(stat);
  set_fjui(fjui);
  reset_momentum(pt * cos(phi), pt * sin(phi), pt * sinh(eta), e);
  set_n_particle(n);

  assert(InternalHelperPythia.particleData.isParticle(id));
  set_restmass(InternalHelperPythia.particleData.m0(id));
  set_property(id);
}
//---------------------------------------------------------------
void ParticleBase::clear()
{
  plabel_ = 0;
  pid_ = 0;
  pstat_ = 0;
  mass_ = -1;
  n_particle_ = 1.0;
  clear_property();
}
//---------------------------------------------------------------
// Properties (clear)
void ParticleBase::clear_property()
{
  property_.chargeType = 0;
  property_.baryonType = 0;
  property_.strangeType = 0;
  property_.charmType = 0;
}
//---------------------------------------------------------------

//---------------------------------------------------------------
void ParticleBase::set_label(int label) { plabel_ = label; }
void ParticleBase::set_id(int id) { pid_ = id; }
void ParticleBase::set_stat(int stat) { pstat_ = stat; }
void ParticleBase::set_restmass(double mass_input) { mass_ = mass_input; }
void ParticleBase::set_n_particle(double n) { n_particle_ = n; }
//---------------------------------------------------------------
// Properties (setters)
void ParticleBase::set_property(int id)
{
  set_chargeType(id);
  set_baryonType(id);
  set_strangeType(id);
}
void ParticleBase::set_chargeType(int id)
{
  property_.chargeType = InternalHelperPythia.particleData.chargeType(id);
}
void ParticleBase::set_baryonType(int id)
{
  property_.baryonType = InternalHelperPythia.particleData.baryonNumberType(id);
}
void ParticleBase::set_strangeType(int id)
{
  property_.strangeType = -net_quark_number(id, 3); // 3 is pid of strange quark
  if (property_.strangeType > 0)
  {
    std::cout << "[ParticleBase] id=" << id << ", strangeness=" << property_.strangeType << std::endl;
    exit(-1);
  }
}
//--
int ParticleBase::net_quark_number(const int id, const int quark) const
{
  int antiparticle = (id < 0 ? -1 : +1); //+1 for particle, -1 for anti particle
  int absid = antiparticle * id;

  absid = 3303;

  // Quarks
  if (InternalHelperPythia.particleData.isQuark(absid))
  {
    return (absid == quark) ? antiparticle : 0;
  }

  // Diquarks
  if (InternalHelperPythia.particleData.isDiquark(absid))
  {
    std::array<int, 2> quarks_in = quarks_in_diquark(absid);
    //
    // std::cout << "[ParticleBase] diquark absid=" << absid
    //           << ", quarks=" << quarks_in[0] << "_" << quarks_in[1] << std::endl;

    //exit(-1);
    return antiparticle*((quarks_in[0] == quark) + (quarks_in[1] == quark));
  }

  return 0;
}
std::array<int, 3> ParticleBase::quarks_in_baryon(const int id) const
{
  return {0, 0, 0};
}
std::array<int, 2> ParticleBase::quarks_in_meson(const int id) const
{
  return {0, 0};
}
std::array<int, 2> ParticleBase::quarks_in_diquark(const int id) const
{
  int quark1 = (id / 1000) % 10;
  int quark2 = (id / 100) % 10;
  return {quark1, quark2};
}
//---------------------------------------------------------------

//  start getters
const int ParticleBase::pid() const { return (pid_); }
const int ParticleBase::pstat() const { return (pstat_); }
const int ParticleBase::plabel() const { return (plabel_); }
const double ParticleBase::n_particle() const { return (n_particle_); }
const FourVector ParticleBase::p_in() const
{
  return (FourVector(px(), py(), pz(), e()));
}
const double ParticleBase::restmass() { return (mass_); }
const double ParticleBase::p(int i)
{
  /// Deprecated. Prefer explicit component access
  // cerr << " DON'T USE ME VERY OFTEN!!" << endl;
  switch (i)
  {
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
//---------------------------------------------------------------
// Properties (getters)

//---------------------------------------------------------------

ParticleBase &ParticleBase::operator=(ParticleBase &c)
{

  fastjet::PseudoJet::operator=(c);

  pid_ = c.pid();
  pstat_ = c.pstat();
  plabel_ = c.plabel();
  mass_ = c.mass_;
  property_ = c.property_;

  return *this;
}

ParticleBase &ParticleBase::operator=(const ParticleBase &c)
{

  fastjet::PseudoJet::operator=(c);

  pid_ = c.pid_;
  pstat_ = c.pstat_;
  plabel_ = c.plabel_;
  mass_ = c.mass_;
  property_ = c.property_;

  return *this;
}
//===========================================================================================================================

//===========================================================================================================================
// (Simple) Particle Class
//===========================================================================================================================
Particle::Particle(const Particle &srh) : ParticleBase::ParticleBase(srh) {}

Particle::Particle(int label, int id, int stat, const FourVector &p, double n)
    : ParticleBase::ParticleBase(label, id, stat, p, n) {}

Particle::Particle(int label, int id, int stat, int fjui, const FourVector &p, double n)
    : ParticleBase::ParticleBase(label, id, stat, fjui, p, n) {}

Particle::Particle(int label, int id, int stat, double pt, double eta, double phi, double e, double n)
    : ParticleBase::ParticleBase(label, id, stat, pt, eta, phi, e, n) {}

Particle::Particle(int label, int id, int stat, int fjui, double pt, double eta, double phi, double e, double n)
    : ParticleBase::ParticleBase(label, id, stat, fjui, pt, eta, phi, e, n) {}

Particle &Particle::operator=(Particle &c)
{
  ParticleBase::operator=(c);
  return *this;
}

Particle &Particle::operator=(const Particle &c)
{
  ParticleBase::operator=(c);
  return *this;
}
//===========================================================================================================================
