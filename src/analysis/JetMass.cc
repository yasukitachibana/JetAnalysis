#include "JetMass.h"
#include "TMath.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetMass> JetMass::reg("JetMass");

JetMass::JetMass(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetMass::~JetMass()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

int JetMass::ReadOptionParametersFromXML()
{
  return n_p;
}

std::string JetMass::GetParamsTag(int i)
{

  if (i == 0)
  {
    return "";
  }
  else
  {
    return "Error!";
  }
}

void JetMass::SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
                            std::vector<std::shared_ptr<Particle>> particle_list,
                            int ir, int ijp, int ijr)
{
  //--------------------------------------------------------------------------------------------------
  for (int iv = 0; iv < variables.size(); iv++)
  {
    for (int ip = 0; ip < n_p; ip++)
    {
      int ii = GetHistIndex(iv, ir, ijp, ijr, 0, 0, ip);
      // index[ip] = ii;
      double jet_mass = jet.m();
      hist_list[ii]->JetTriggered();
      hist_list[ii]->Fill(jet_mass, 1.0);
    }
  } // iv
  //--------------------------------------------------------------------------------------------------
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void JetMass::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[JetMass] hist_name = " << hist_name << std::endl;

  auto total_hist = CreateHist(hist_name, iv);
  total_hist->Init();

  double nJetTotal = 0.0;

  for (auto hist : hist_list)
  {
    double n_ev = hist->Nev();
    if (n_ev != 0)
    {
      nJetTotal += hist->GetNjetSigmaOverEev();
      double sigma = hist->Sigma();
      total_hist->Add(hist, sigma / n_ev);
    }
  }

  total_hist->JetTriggered(nJetTotal);
  total_hist->Print("count_");

  if (nJetTotal != 0)
  {
    total_hist->Scale(1.0 / nJetTotal, "width");
    total_hist->Print("jetmass_");
    total_hist->Normalize("width");
    total_hist->Print("normalized_jetmass_");
  }
  else
  {
    std::cout << "[JetMass] 0-total Jet" << std::endl;
    std::cout << "[JetMass] Skip. " << std::endl;
  }

  total_hist->DeleteTH();
}
