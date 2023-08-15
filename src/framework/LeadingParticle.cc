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
    if (constituent.perp() > leading_pt_min)
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

bool LeadingInCone::Trigger(fastjet::PseudoJet jet, double r_cone)
{
  std::cout << "[LeadingInCone] Under Construction. Exit." << std::endl;
  std::cout << "[LeadingInCone] jet cone size R = " << r_cone << std::endl;
  exit(-1);
  return false;
}
