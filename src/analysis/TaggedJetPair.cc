#include "TaggedJetPair.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<TaggedJetPair> TaggedJetPair::reg("TaggedJetPair");

TaggedJetPair::TaggedJetPair(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

TaggedJetPair::~TaggedJetPair()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

//--------------------------------------------------------------------------------------------------
int TaggedJetPair::ReadOptionParametersFromXML()
{

  return rMin.size()*aMin.size();
}

int TaggedJetPair::ReadVariablesFromXML(std::string tag)
{
  int exist = 0;

  for (int i = 0; i < n_var; i++)
  {
  //   // tag specifies sets of parameters in the anlysis (e.g. beta and zcut)
  //   std::string var = varNames[i] + tag;
  //   auto ite = std::find(variables.begin(), variables.end(), var);
  //   if (ite != variables.end())
  //   {
  //     int index = distance(variables.begin(), ite);
  //     // std::cout << val << " " << index << std::endl;
  //     i_var[i].push_back(index);
  //     exist++;
  //   }
  }

  return exist;
}

void TaggedJetPair::
    OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list,
                     int i_tag_particle)
{

  //==============================================================================
  // Set pT of the Tag Particle
  double pt_tag = jet_tag_ptr->GetPtTag(i_tag_particle);
  // std::cout << "i=" << i_tag_particle << ", pt = " << pt_tag << " GeV" << std::endl;
  //==============================================================================

  //==============================================================================
  // Set Particle List to be Passed to Jet-finding Algorithm
  std::vector<fastjet::PseudoJet> fj_inputs;
  for (auto p : particle_list)
  {
    fj_inputs.push_back(p->GetPseudoJet());
  }
  //==============================================================================

  // For Loop for Different Jet Cone Sizes
  for (int ir = 0; ir < jetR.size(); ir++)
  {

    //==============================================================================
    // Jet Reconstruction
    //------------------------------------------------------------------------------
    // Jet Cone Size
    double r_cone = jetR[ir];
    // Get Reconstructed Jet List
    std::vector<fastjet::PseudoJet> jets = reco_ptr->JetReco(r_cone, particle_list);
    // PrintParticleInfoList(jets);
    //==============================================================================

    // // For Loop for pTjet and Rapidity Cuts
    // for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
    // {
    //   for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
    //   {
    //     // Dummy Loop for Particle Cuts------------
    //     for (int iv = 0; iv < variables.size(); iv++)
    //     {
    //       for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
    //       {
    //         for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
    //         {

    //           // Get Index for Histogram
    //           int index = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, 0);

    //           // Trigger Tag (Not Jet!)
    //           hist_list[index]->JetTriggered();

    //           // Counter for Number of Triggered Jets per Tag
    //           int n_jet = 0;

    //           // For Loop for Jets
    //           for (auto j : jets)
    //           {

    //             //====================================
    //             // Trigger Jets
    //             if (JetTrigger(j, ir, ijp, ijr))
    //             {
    //               // PrintPseudoJetInfo(j);

    //               // X_j-tag
    //               double xJTag = j.perp() / pt_tag;

    //               // Fill Histogram
    //               hist_list[index]->Fill(xJTag, 1.0);

    //               // Count Triggered Jet
    //               n_jet++;
    //             }
    //             //====================================

    //             //====================================
    //             // Reach Maximum Triggered Jet Number per Tag
    //             if (n_jet == nJetEv)
    //             {
    //               break;
    //             }
    //             //====================================

    //           } // jet

    //         } // ipr
    //       }   // ipp
    //     }     // iv
    //     // End Dummy Loop--------------------------
    //   } // ijr
    // }   // ijp
  }     // ir
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TaggedJetPair::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[TaggedJetPair] hist_name = " << hist_name << std::endl;

  auto total_hist = CreateHist(hist_name, iv);
  total_hist->Init();

  double nTagTotal = 0.0;

  for (auto hist : hist_list)
  {
    double n_ev = hist->Nev();
    if (n_ev != 0)
    {
      nTagTotal += hist->GetNjetSigmaOverEev();
      double sigma = hist->Sigma();
      total_hist->Add(hist, sigma / n_ev);
    }
  }

  total_hist->Print("count_");

  if (nTagTotal != 0)
  {
    total_hist->Scale(1.0 / nTagTotal, "width");
    total_hist->Print("taggedjetpair_");
  }
  else
  {
    std::cout << "[TaggedJetPair] 0-total Tag Particle" << std::endl;
    std::cout << "[TaggedJetPair] Skip. " << std::endl;
  }

  total_hist->DeleteTH();
}
