#include "Smearing.h"
#include "SetXML.h"

//-----------------------------------------------------------------
// Register the module with the base class
RegisterSmearingModule<NoSmearing> NoSmearing::reg("NoSmearing");

NoSmearing::NoSmearing(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating NoSmearing" << std::endl;
}

NoSmearing::~NoSmearing()
{
  std::cout << "-$-Deleting NoSmearing" << std::endl;
}

//-----------------------------------------------------------------
// Register the module with the base class
RegisterSmearingModule<CMSxJGammaSmearing> CMSxJGammaSmearing::reg("CMSxJGamma");

CMSxJGammaSmearing::CMSxJGammaSmearing(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating CMSxJGammaSmearing" << std::endl;
}

CMSxJGammaSmearing::~CMSxJGammaSmearing()
{
  std::cout << "-$-Deleting CMSxJGammaSmearing" << std::endl;
}

void CMSxJGammaSmearing::Init()
{
  ReadParametersFromXML();
  initialized = 1;
}

void CMSxJGammaSmearing::ReadParametersFromXML()
{
  C = SetXML::Instance()->GetElementDouble({"smearing", "C"});
  S = SetXML::Instance()->GetElementDouble({"smearing", "S"});
  N = SetXML::Instance()->GetElementDouble({"smearing", "N"});
}

void CMSxJGammaSmearing::ShowSmearingSetting()
{
  if (initialized == 0)
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[CMSxJGammaSmearing] CMSxJGammaSmearing is NOT initialized." << std::endl;
    std::cout << "[CMSxJGammaSmearing] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
  else
  {
    PrintSmearingSetting();
  }
}

void CMSxJGammaSmearing::PrintSmearingSetting()
{
  //==============================================================================
  std::cout << "[     Smearing     ] *** Smearing ON: CMSxJGammaSmearing" << std::endl;
  std::cout << "[     Smearing     ] *** Smearing Parameters: " << std::endl;
  std::cout << "[     Smearing     ] *** C=" << C << ", S=" << S << ", N=" << N << std::endl;
  //==============================================================================
}

// Just pT------------------------------
double CMSxJGammaSmearing::Smear(double pt)
{
  double factor = SampleSmearingFactor(pt);
  return factor * pt;
}

// One Jet or Particle------------------------------
std::shared_ptr<Particle>
CMSxJGammaSmearing::Smear(std::shared_ptr<Particle> particle)
{
  exit(-1);
  return particle;
}

fastjet::PseudoJet
CMSxJGammaSmearing::Smear(fastjet::PseudoJet jet)
{
  exit(-1);  
  return jet;
}

std::shared_ptr<fastjet::PseudoJet>
CMSxJGammaSmearing::Smear(std::shared_ptr<fastjet::PseudoJet> jet)
{
  exit(-1);  
  return jet;
}

// List of Jets or Particles------------------------------
std::vector<std::shared_ptr<Particle>>
CMSxJGammaSmearing::Smear(std::vector<std::shared_ptr<Particle>> particles)
{
  exit(-1);  
  return particles;
}

std::vector<fastjet::PseudoJet>
CMSxJGammaSmearing::Smear(std::vector<fastjet::PseudoJet> jets)
{
  exit(-1);  
  return jets;
}

std::vector<std::shared_ptr<fastjet::PseudoJet>>
CMSxJGammaSmearing::Smear(std::vector<std::shared_ptr<fastjet::PseudoJet>> jets)
{
  exit(-1);   
  return jets;
}

double CMSxJGammaSmearing::SampleSmearingFactor(double pt){
  exit(-1);   
  return 1.0;
}