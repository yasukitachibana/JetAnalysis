#include "Smearing.h"
#include "SetXML.h"
#include <random>

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
  // std::cout << "smearing factor " << factor << std::endl;
  // exit(-1);
  return factor * pt;
}

// One Jet or Particle------------------------------
std::shared_ptr<Particle>
CMSxJGammaSmearing::Smear(std::shared_ptr<Particle> particle)
{
  double pt = particle->perp();
  double factor = SampleSmearingFactor(pt);

  double e = factor * particle->e();
  double px = factor * particle->px();
  double py = factor * particle->py();
  double pz = factor * particle->pz();

  particle->reset_momentum(px, py, pz, e);
  return particle;
}

fastjet::PseudoJet
CMSxJGammaSmearing::Smear(fastjet::PseudoJet jet)
{
  double pt = jet.perp();
  double factor = SampleSmearingFactor(pt);

  double e = factor * jet.e();
  double px = factor * jet.px();
  double py = factor * jet.py();
  double pz = factor * jet.pz();

  jet.reset_momentum(px, py, pz, e);

  return jet;
}

std::shared_ptr<fastjet::PseudoJet>
CMSxJGammaSmearing::Smear(std::shared_ptr<fastjet::PseudoJet> jet)
{
  double pt = jet->perp();
  double factor = SampleSmearingFactor(pt);

  double e = factor * jet->e();
  double px = factor * jet->px();
  double py = factor * jet->py();
  double pz = factor * jet->pz();

  jet->reset_momentum(px, py, pz, e);

  return jet;
}

// List of Jets or Particles------------------------------
std::vector<std::shared_ptr<Particle>>
CMSxJGammaSmearing::Smear(std::vector<std::shared_ptr<Particle>> particles)
{
  std::vector<std::shared_ptr<Particle>> particles_scaled;
  for (auto &p : particles)
  {
    particles_scaled.push_back(Smear(p));
  }
  return particles_scaled;
}

std::vector<fastjet::PseudoJet>
CMSxJGammaSmearing::Smear(std::vector<fastjet::PseudoJet> jets)
{
  std::vector<fastjet::PseudoJet> jets_scaled;
  for (auto &j : jets)
  {
    jets_scaled.push_back(Smear(j));
  }
  return jets_scaled;
}

std::vector<std::shared_ptr<fastjet::PseudoJet>>
CMSxJGammaSmearing::Smear(std::vector<std::shared_ptr<fastjet::PseudoJet>> jets)
{
  std::vector<std::shared_ptr<fastjet::PseudoJet>> jets_scaled;
  for (auto &j : jets)
  {
    jets_scaled.push_back(Smear(j));
  }
  return jets_scaled;
}

double CMSxJGammaSmearing::SampleSmearingFactor(double pt)
{
  double sigma = GetSigma(pt);
  //----------------------------------------------------------------
  std::random_device rd;                                // Generate a random seed
  std::mt19937 gen(rd());                               // Mersenne Twister random number generator
  std::uniform_real_distribution<double> dis(0.0, 1.0); // Uniform distribution of real numbers between 0.0 and 1.0

  bool sampled = false;
  double x_sampled = -1.0;

  while (sampled == false)
  {
    x_sampled = 2.0 * dis(gen) - 1.0; // Generate a random number
    double y_sampled = dis(gen);      // Generate a random number
    double fx_sampled = exp(-0.5 * x_sampled * x_sampled / sigma / sigma);
    if (y_sampled < fx_sampled)
    {
      sampled = true;
    }
  }
  return x_sampled + 1.0;
}

double CMSxJGammaSmearing::GetSigma(double pt)
{
  return sqrt(C * C + S * S / pt + N * N / pt / pt);
}