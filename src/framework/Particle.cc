#include "Particle.h"

// Initialize static MakeUniqueHelper.here
Pythia8::Pythia ParticleBase::InternalHelperPythia("IntentionallyEmpty", false);
//
int ParticleBase::strange_k0s = 0;

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
  set_charmType(id);
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
  property_.strangeType = -net_quark_number(id, 3); // 3 is pid of strange quark (S=-1)
}
void ParticleBase::set_charmType(int id)
{
  property_.charmType = net_quark_number(id, 4); // 4 is pid of charm quark (C=1)
}
//--
int ParticleBase::net_quark_number(const int id, const int quark) const
{
  int antiparticle = (id < 0 ? -1 : +1); //+1 for particle, -1 for anti particle
  int absid = antiparticle * id;

  // Quarks
  if (InternalHelperPythia.particleData.isQuark(absid))
  {
    return (absid == quark) ? antiparticle : 0;
  }

  // Diquarks (Just count quarks in diquark)
  if (InternalHelperPythia.particleData.isDiquark(absid))
  {
    std::array<int, 2> quarks_in = quarks_in_diquark(absid);
    // std::cout << "[ParticleBase] diquark absid=" << absid
    //           << ", quarks=" << quarks_in[0] << "_" << quarks_in[1] << std::endl;
    // exit(-1);
    return antiparticle * ((quarks_in[0] == quark) + (quarks_in[1] == quark));
  }

  // Baryons (Just count quarks in baryon)
  if (InternalHelperPythia.particleData.isBaryon(absid))
  {
    std::array<int, 3> quarks_in = quarks_in_baryon(absid);
    return antiparticle * ((quarks_in[0] == quark) + (quarks_in[1] == quark) + (quarks_in[2] == quark));
  }

  // Mesons
  if (InternalHelperPythia.particleData.isMeson(absid))
  {

    // K0L
    if (absid == 130)
    {
      if (abs(ParticleBase::strange_k0s) <= 1)
      {
        return ParticleBase::strange_k0s;
      }
      else
      {
        return (rand() % 2) * 2 - 1;
      }
    }

    // K0S
    if (absid == 310)
    {
      if (abs(ParticleBase::strange_k0s) <= 1)
      {
        return -ParticleBase::strange_k0s;
      }
      else
      {
        return (rand() % 2) * 2 - 1;
      }
    }

    std::array<int, 2> quarks_in = quarks_in_meson(absid);

    // quarkonium state? Not open net_quark_number.
    if (quarks_in[0] == quarks_in[1] ||
        (quarks_in[0] != quark && quarks_in[1] != quark))
    {
      return 0;
    }

    // Mesons with single (our) quark
    /* this has covered all the easy stuff
     * get the "other" quark. (We know this must exist, since they are
     * not both the right one and one of them is the right one). */
    int otherquark = (quarks_in[0] == quark) ? quarks_in[1] : quarks_in[0];
    /* "our" quark is the heavier one: 1 for u,c,t; -1 for d,s,b (and of
     * course the antiparticle sign) */
    if (quark > otherquark)
    {
      return antiparticle * ((quark % 2 == 0) ? 1 : -1);
    }
    /* ours is the lighter: If the heavier particle is u,c,t, the lighter
     * one (ours) is an antiquark. */
    return antiparticle * ((otherquark % 2 == 0) ? -1 : 1);
  }

  // Rest (leptons, gauge bosons,...)
  return 0;
}
std::array<int, 3> ParticleBase::quarks_in_baryon(const int id) const
{
  int quark1 = (id / 1000) % 10;
  int quark2 = (id / 100) % 10;
  int quark3 = (id / 10) % 10;
  return {quark1, quark2, quark3};
}
std::array<int, 2> ParticleBase::quarks_in_meson(const int id) const
{
  int quark1 = (id / 100) % 10;
  int quark2 = (id / 10) % 10;
  return {quark1, quark2};
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
const double ParticleBase::charge() const
{
  return (double)property_.chargeType / 3.0;
}
const int ParticleBase::chargeType() const
{
  return property_.chargeType;
}
const double ParticleBase::baryon() const
{
  return (double)property_.baryonType / 3.0;
}
const int ParticleBase::baryonType() const
{
  return property_.baryonType;
}
const int ParticleBase::strange() const
{
  return property_.strangeType;
}
const int ParticleBase::strangeType() const
{
  return property_.strangeType;
}
const int ParticleBase::charm() const
{
  return property_.charmType;
}
const int ParticleBase::charmType() const
{
  return property_.charmType;
}
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

void ParticleBase::PrintInfo() const
{
  std::cout << "\n[ParticleBase]---------------------------------------------------"
            << "\n[ParticleBase] id=" << pid_
            << " stat=" << pstat_
            << " plabel=" << plabel_
            << "\n[ParticleBase] pt=" << perp()
            << " eta=" << eta()
            << " y=" << rap()
            << " phi=" << phi()
            << "\n[ParticleBase]---------------------------------------------------"
            << std::endl;
}

void ParticleBase::PrintInfo(int i) const
{
  std::cout << "#" << i
            << ": id=" << pid_
            << " stat=" << pstat_
            << " plabel=" << plabel_
            << " pt=" << perp()
            << " eta=" << eta()
            << " y=" << rap()
            << " phi=" << phi() << std::endl;
}

//===========================================================================================================================

//===========================================================================================================================
// (Simple) Particle Class
//===========================================================================================================================
Particle::Particle() : ParticleBase::ParticleBase() {}

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

void PrintParticleInfoList(const std::vector<std::shared_ptr<ParticleBase>> particles)
{
  std::cout << "\n----------------------------------------------------------" << std::endl;
  for (size_t i = 0; i < particles.size(); i++)
  {
    particles[i]->PrintInfo(i);
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;
}

void PrintParticleInfoList(const std::vector<std::shared_ptr<Particle>> particles)
{
  std::cout << "\n----------------------------------------------------------" << std::endl;
  for (size_t i = 0; i < particles.size(); i++)
  {
    particles[i]->PrintInfo(i);
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;
}

void PrintParticleInfoList(const std::vector<fastjet::PseudoJet> particles)
{
  std::cout << "\n----------------------------------------------------------" << std::endl;
  for (size_t i = 0; i < particles.size(); i++)
  {
    PrintPseudoJetInfo(particles[i],i);
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;
}

void PrintPseudoJetInfo(const fastjet::PseudoJet pseudojet, int i)
{
    std::cout << "PseudoJet #" << i
              << " user_index=" << pseudojet.user_index()
              << " pt=" << pseudojet.perp()
              << " eta=" << pseudojet.eta()
              << " y=" << pseudojet.rap()
              << " phi=" << pseudojet.phi() << std::endl;
}

//------------------------------------------------------------------------------------------

std::vector<Particle> sorted_by_pt(const std::vector<Particle> &particles)
{
  std::vector<double> minus_kt2(particles.size());
  for (size_t i = 0; i < particles.size(); i++)
  {
    minus_kt2[i] = -particles[i].kt2();
  }
  return objects_sorted_by_values(particles, minus_kt2);
}
std::vector<ParticleBase> sorted_by_pt(const std::vector<ParticleBase> &particles)
{
  std::vector<double> minus_kt2(particles.size());
  for (size_t i = 0; i < particles.size(); i++)
  {
    minus_kt2[i] = -particles[i].kt2();
  }
  return objects_sorted_by_values(particles, minus_kt2);
}
std::vector<std::shared_ptr<Particle>>
sorted_by_pt(const std::vector<std::shared_ptr<Particle>> &particles)
{
  std::vector<double> minus_kt2(particles.size());
  for (size_t i = 0; i < particles.size(); i++)
  {
    minus_kt2[i] = -particles[i]->kt2();
  }
  return objects_sorted_by_values(particles, minus_kt2);
}
std::vector<std::shared_ptr<ParticleBase>>
sorted_by_pt(const std::vector<std::shared_ptr<ParticleBase>> &particles)
{
  std::vector<double> minus_kt2(particles.size());
  for (size_t i = 0; i < particles.size(); i++)
  {
    minus_kt2[i] = -particles[i]->kt2();
  }
  return objects_sorted_by_values(particles, minus_kt2);
}
