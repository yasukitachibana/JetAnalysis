#include "TaggedJetPair.h"
#include "TMath.h"

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
void TaggedJetPair::ShowParamsSetting()
{

  std::cout << "[   TaggedJetPair  ] ***-------------------------------------------" << std::endl;
  std::cout << "[   TaggedJetPair  ] *** [TaggedJetPair]" << std::endl;

  std::cout << "[   TaggedJetPair  ] *** Delta RJJ: ";
  for (int i = 0; i < rBinEdges.size() - 1; i++)
  {
    std::cout << rBinEdges[i]
              << "-"
              << rBinEdges[i + 1] << ", ";
  }
  std::cout << "\b\b  " << std::endl;

  std::cout << "[   TaggedJetPair  ] *** Delta AJJTag: ";
  for (int i = 0; i < aBinEdges.size() - 1; i++)
  {
    std::cout << aBinEdges[i]
              << "-"
              << aBinEdges[i + 1] << ", ";
  }
  std::cout << "\b\b  " << std::endl;
}

std::string TaggedJetPair::VariableSuffix(int i)
{
  if (i == 1)
  {
    return "";
  }
  else
  {
    return std::to_string(i);
  }
}

int TaggedJetPair::ReadOptionParametersFromXML()
{

  particles_str = "Jet Pairs";
  rBinEdges = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "rBinEdges", "Item"});
  aBinEdges = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "aBinEdges", "Item"});

  for (int i = 1; i < 30; i++)
  {
    std::string i_str = VariableSuffix(i);
    // std::cout << i_str << std::endl;
    if (ReadVariablesFromXML(i_str) == 0)
    {
      break;
    }
  }

  for (int i = 0; i < n_var; i++)
  {
    std::cout
        << varNames[i] << ": ";
    for (auto iv : i_var[i])
    {
      std::cout << iv << " ";
    }
    std::cout << std::endl;
  }

  return (rBinEdges.size() - 1) * (aBinEdges.size() - 1);
}

int TaggedJetPair::ReadVariablesFromXML(std::string tag)
{
  int exist = 0;

  for (int i = 0; i < n_var; i++)
  {
    // tag specifies sets of parameters in the anlysis (e.g. beta and zcut)
    std::string var = varNames[i] + tag;
    auto ite = std::find(variables.begin(), variables.end(), var);
    if (ite != variables.end())
    {
      int index = distance(variables.begin(), ite);
      // std::cout << var << " " << index << " in "<< varNames[i] << std::endl;
      i_var[i].push_back(index);
      exist++;
    }
  }

  return exist;
}

bool TaggedJetPair::SubJetTrigger(fastjet::PseudoJet subjet, int ipp, int ipr)
{
  double pt = subjet.perp();
  double rapidity = particle_rap_ptr->Val(subjet);

  if (pt >= particlePtMin[ipp] && pt < particlePtMax[ipp] &&
      fabs(rapidity) >= particleRapMin[ipr] && fabs(rapidity) < particleRapMax[ipr])
  {
    return true;
  }
  else
  {
    return false;
  }
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
    // For Loop for pTjet and Rapidity Cuts
    for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
    {
      for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
      {
        for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
        {
          for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
          {
            //==========================================================
            // Trigger Taged particle
            // nParams for sets of parameters in the anlysis (e.g. rJJ, aJJTag)
            for (int ip = 0; ip < nParams; ip++)
            {
              // n_var is for 0:"xJJTag", 1:"rJJ", 2:"aJJTag"
              std::array<std::vector<int>, n_var> index;
              for (int i = 0; i < n_var; i++)
              {
                // i_var[i] is vector. Each element of the vector is for each of multiple bin settings.
                // index[i] stores the indices for all bin settings for this parameter set.
                index[i] = GetHistIndex(i_var[i], ir, ijp, ijr, ipp, ipr, ip);
                for (auto ii : index[i])
                {
                  // std::cout << ii << " ";
                  hist_list[ii]->JetTriggered();
                }
              }
            }
            //==========================================================
            // For Loop for Jets
            for (int i_j1 = 0; i_j1 < jets.size(); i_j1++)
            {
              fastjet::PseudoJet jet1 = jets[i_j1];
              if (SubJetTrigger(jet1, ipp, ipr))
              {
                for (int i_j2 = i_j1 + 1; i_j2 < jets.size(); i_j2++)
                {
                  fastjet::PseudoJet jet2 = jets[i_j2];
                  if (SubJetTrigger(jet2, ipp, ipr))
                  {
                    fastjet::PseudoJet paired_jet = jet1 + jet2;
                    if (JetTrigger(paired_jet, ir, ijp, ijr))
                    {

                      double xJJTag = paired_jet.perp() / pt_tag;
                      double delta_eta12 = jet1.eta() - jet2.eta();
                      double delta_phi12 = jet1.delta_phi_to(jet2);
                      double rJJ = TMath::Sqrt(delta_eta12 * delta_eta12 + delta_phi12 * delta_phi12);
                      double aJJTag = fabs(jet1.perp() - jet2.perp()) / pt_tag;

                      // nParams for sets of parameters in the anlysis (e.g. rJJ, aJJTag)
                      for (int ip = 0; ip < nParams; ip++)
                      {

                        auto ip_array = GetParamIndex(ip);
                        double r_l = rBinEdges[ip_array[0]];
                        double r_u = rBinEdges[ip_array[0] + 1];
                        double a_l = aBinEdges[ip_array[1]];
                        double a_u = aBinEdges[ip_array[1] + 1];

                        if (rJJ >= r_l && rJJ < r_u &&
                            aJJTag >= a_l && aJJTag < a_u)
                        {

                          // n_var is for 0:"xJJTag", 1:"rJJ", 2:"aJJTag"
                          std::array<std::vector<int>, n_var> index;
                          for (int i = 0; i < n_var; i++)
                          {
                            // i_var[i] is vector. Each element of the vector is for each of multiple bin settings.
                            // index[i] stores the indices for all bin settings for this parameter set.
                            index[i] = GetHistIndex(i_var[i], ir, ijp, ijr, ipp, ipr, ip);
                            // std::cout << varNames[i] << ": ";
                            // for (auto ii : index[i])
                            // {
                            //   // std::cout << ii << " ";
                            //   hist_list[ii]->JetTriggered();
                            // }
                            // std::cout << endl;
                          }

                          // 0:"xJJTag"
                          for (auto i : index[0])
                          {
                            // std::cout << i << " ";
                            hist_list[i]->Fill(xJJTag, 1.0);
                          }
                          // 1:"rJJ"
                          for (auto i : index[1])
                          {
                            // std::cout << i << " ";
                            hist_list[i]->Fill(rJJ, 1.0);
                          }
                          // 2:"aJJTag"
                          for (auto i : index[2])
                          {
                            // std::cout << i << " ";
                            hist_list[i]->Fill(aJJTag, 1.0);
                          }

                        } // params trigger
                      }   // params
                    }     // paired jet trigger
                  }       // jet 2 trigger
                }         // jet 2
              }           // jet 1 trigger
            }             // jet 1
            //==========================================================
          } // ipr
        }   // ipp
      }     // ijr
    }       // ijp
  }         // ir
}

//------------------------------------------------------------
// Get Tags for Parameters
std::string TaggedJetPair::GetParamsTag(int i)
{
  return GetParamsTag(GetParamIndex(i));
}

std::string TaggedJetPair::GetParamsTag(std::array<int, 2> i)
{
  return GetParamsTag(i[0], i[1]);
}

std::string TaggedJetPair::GetParamsTag(int i_r, int i_a)
{
  return GetParamsTag(rBinEdges[i_r], rBinEdges[i_r + 1], aBinEdges[i_a], aBinEdges[i_a + 1]);
}

std::string TaggedJetPair::GetParamsTag(double r_l, double r_u, double a_l, double a_u)
{
  std::ostringstream oss;

  oss << std::fixed
      << "rJJ" << std::setprecision(3) << (r_l) << "-"
      << std::setprecision(3) << (r_u) << "_"
      << "aJJTag" << std::setprecision(3) << (a_l) << "-"
      << std::setprecision(3) << (a_u);

  return oss.str();
}
//------------------------------------------------------------
// Get Index of Tags for Parameters
int TaggedJetPair::GetParamIndex(std::array<int, 2> i)
{
  return GetParamIndex(i[0], i[1]);
}

int TaggedJetPair::GetParamIndex(int i_r, int i_a)
{
  return (aBinEdges.size() - 1) * (i_r) + i_a;
}

std::array<int, 2> TaggedJetPair::GetParamIndex(int i)
{
  int i_r = i / (aBinEdges.size() - 1);
  int i_a = i % (aBinEdges.size() - 1);
  return std::array<int, 2>{i_r, i_a};
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
