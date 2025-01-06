#include "LeadingParticle.h"
#include "SetXML.h"

bool LeadingParticleHelper::LeadingConst(std::string method_in)
{

  //--------------------------------------------------------
  for (auto name : cNames)
  {
    if (method_in == name)
    {
      return true;
    }
  }

  //--------------------------------------------------------
  for (auto name : aNames)
  {
    if (method_in == name)
    {
      return false;
    }
  }

  //--------------------------------------------------------
  std::cout << std::endl;
  std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
  std::cout << "[LeadingParticle] Bad Input for leadingMethod" << std::endl;
  std::cout << "[LeadingParticle] Exit. " << std::endl;
  std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
  std::cout << std::endl;
  exit(-1);
}

//===========================================================================================================================
// BASE
//===========================================================================================================================

LeadingParticleBase::LeadingParticleBase()
{
  initialized = false;
}

void LeadingParticleBase::ShowLeadingParticleSetting()
{
  if (initialized)
  {
    PrintLeadingParticleSetting();
  }
  else
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[LeadingParticleBase] LeadingParticle is NOT initialized." << std::endl;
    std::cout << "[LeadingParticleBase] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
}
//===========================================================================================================================

//===========================================================================================================================
// NoLeading
NoLeading::NoLeading() : LeadingParticleBase() {}
void NoLeading::Init()
{
  initialized = true;
}
void NoLeading::Init(double leading_pt_min_in)
{
  Init();
}

//===========================================================================================================================
// LeadingRequirement
LeadingRequirement::LeadingRequirement() : LeadingParticleBase() {}

//===========================================================================================================================
// LeadingConstituent
LeadingConstituent::LeadingConstituent() : LeadingRequirement() {}

void LeadingConstituent::Init(double leading_pt_min_in)
{
  leading_pt_min = leading_pt_min_in;
  initialized = true;
}

void LeadingConstituent::PrintLeadingParticleSetting()
{
  std::cout << "[LeadingConstituent] ***-------------------------------------------" << std::endl;
  std::cout << "[LeadingConstituent] *** [Leading Parcitle Requirement]" << std::endl;
  std::cout << "[LeadingConstituent] *** pT > " << leading_pt_min << " GeV" << std::endl;
  std::cout << "[LeadingConstituent] *** in Reconstructed Jet Constitunets" << std::endl;
}

bool LeadingConstituent::Trigger(fastjet::PseudoJet jet, double r_cone)
{
  // std::cout << "\n[LeadingConstituent] *** jet pt = " << jet.perp() << " GeV" << std::endl;
  for (auto constituent : jet.constituents())
  {
    if ((!(constituent.user_index() < 0)) &&
        constituent.perp() > leading_pt_min)
    {
      // std::cout << "[LeadingConstituent] *** pass ! pt = " << constituent.perp() << " GeV" << std::endl;
      return true;
    }
    // std::cout << "[LeadingConstituent] *** fail ! = " << constituent.perp() << " GeV" << std::endl;
  }
  return false;
}

//===========================================================================================================================
// LeadingInCone
LeadingInCone::LeadingInCone() : LeadingRequirement() {}

void LeadingInCone::Init(double leading_pt_min_in)
{
  leading_pt_min = leading_pt_min_in;
  int leading_particle = SetXML::Instance()->GetElementInt({"jetReco", "leadCharged"});
  if (leading_particle)
  {
    lead_charged_ptr = std::unique_ptr<ChargedOnly>(new ChargedOnly());
  }
  else
  {
    lead_charged_ptr = std::unique_ptr<IncludeNeutral>(new IncludeNeutral());
  }
  initialized = true;
}

void LeadingInCone::PrintLeadingParticleSetting()
{
  std::cout << "[LeadingConstituent] ***-------------------------------------------" << std::endl;
  std::cout << "[LeadingConstituent] *** [Leading Parcitle Requirement]" << std::endl;
  std::cout << "[LeadingConstituent] *** pT > " << leading_pt_min << " GeV, " << lead_charged_ptr->Type() << " Particle" << std::endl;
  std::cout << "[LeadingConstituent] *** in Jet Cone Area" << std::endl;
}

void LeadingInCone::SetEvent(std::vector<std::shared_ptr<Particle>> particle_list_in)
{
  event_particle_list.clear();
  event_particle_list.shrink_to_fit();
  event_particle_list = particle_list_in;
}

bool LeadingInCone::Trigger(fastjet::PseudoJet jet, double r_cone)
{

  // std::cout << "In Leading:" << std::endl;
  // std::cout << "n_particle = " << event_particle_list.size() << std::endl;
  // std::cout << "#0 pstat=" << event_particle_list[0]->pstat()
  //           << ", pid=" << event_particle_list[0]->pid()
  //           << ", pt=" << event_particle_list[0]->perp()
  //           << ", eta=" << event_particle_list[0]->eta() << std::endl;

  for (auto p : event_particle_list)
  {

    double delta_eta = p->eta() - jet.eta();
    double delta_phi = jet.delta_phi_to(p->GetPseudoJet());
    double delta_r2 = delta_eta * delta_eta + delta_phi * delta_phi;

    if (delta_r2 < r_cone * r_cone &&
        p->perp() > leading_pt_min &&
        lead_charged_ptr->Trigger(p) &&
        p->pstat() != -1)
    {
      // std::cout << "Pass !: pstat=" << p->pstat()
      //           << ", pid=" << p->pid()
      //           << ", pt=" << p->perp()
      //           << ", eta=" << p->eta() << std::endl;
      return true;
    }
    // else
    // {
    //   std::cout << "fail !: pstat=" << p->pstat()
    //             << ", pid=" << p->pid()
    //             << ", pt=" << p->perp()
    //             << ", eta=" << p->eta() << std::endl;
    // }
  }
  return false;
}
