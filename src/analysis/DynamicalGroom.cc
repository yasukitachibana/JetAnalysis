#include "DynamicalGroom.h"
#include "TMath.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<DynamicalGroom> DynamicalGroom::reg("DynamicalGroom");

DynamicalGroom::DynamicalGroom(std::string name_in) : name(name_in), ui(-123456)
{
  std::cout << "-@-Creating " << name << std::endl;
  std::cout << "Reconstruction Method is Fixed in " << name << "[Negative Recombiner]" << std::endl;
  std::cout << "Setting of Reconstruction in XML is to be ignored" << std::endl;
}

DynamicalGroom::~DynamicalGroom()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

//--------------------------------------------------------------------------------------------------
int DynamicalGroom::ReadVariablesFromXML(std::string tag)
{
  int exist = 0;
  // n_var is for 0:"zG", 1:"thetaG", 2:"rG", 3:"mG", 4:"mGOverPt", 5:"pseudoMG", 6:"pseudoMGOverPt"...
  for (int i = 0; i < n_var; i++)
  {
    // tag specifies sets of parameters in the anlysis (e.g. beta and zcut)
    std::string var = varNames[i] + tag;
    auto ite = std::find(variables.begin(), variables.end(), var);
    if (ite != variables.end())
    {
      int index = distance(variables.begin(), ite);
      // std::cout << var << " " << index << std::endl;
      i_var[i].push_back(index);
      exist++;
    }
  }

  return exist;
}

std::string DynamicalGroom::VariableSuffix(int i)
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

int DynamicalGroom::ReadOptionParametersFromXML()
{

  beta = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "aDyn", "Item"});
  // Parameter name, beta, is used instead of a in dynamical grooming for structual reason
  zCut = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "zCut", "Item"}, false);

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

  return beta.size() * zCut.size();
}

//------------------------------------------------------------
// Get Tags for Parameters
std::string DynamicalGroom::GetParamsTag(int i)
{
  return GetParamsTag(GetParamIndex(i));
}

std::string DynamicalGroom::GetParamsTag(std::array<int, 2> i)
{
  return GetParamsTag(i[0], i[1]);
}

std::string DynamicalGroom::GetParamsTag(int i_beta, int i_zCut)
{
  return GetParamsTag(beta[i_beta], zCut[i_zCut]);
}

std::string DynamicalGroom::GetParamsTag(double beta_sd, double z_cut_sd)
{
  std::ostringstream oss;

  oss << std::fixed
      << "aDyn" << std::setprecision(2) << (beta_sd) << "_"
      << "zCut" << std::setprecision(2) << (z_cut_sd);

  return oss.str();
}
//------------------------------------------------------------
// Get Index of Tags for Parameters
int DynamicalGroom::GetParamIndex(std::array<int, 2> i)
{
  return GetParamIndex(i[0], i[1]);
}

int DynamicalGroom::GetParamIndex(int i_beta, int i_zCut)
{
  return zCut.size() * (i_beta) + i_zCut;
}

std::array<int, 2> DynamicalGroom::GetParamIndex(int i)
{
  int i_beta = i / zCut.size();
  int i_zCut = i % zCut.size();
  return std::array<int, 2>{i_beta, i_zCut};
}
//------------------------------------------------------------

void DynamicalGroom::ShowParamsSetting()
{
  std::cout << "[AnalyzeModuleBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalyzeModuleBase] *** [DynamicalGroom]" << std::endl;

  std::cout << "[AnalyzeModuleBase] *** aDyn: ";
  for (auto b : beta)
  {
    std::cout << b << ", ";
  }
  std::cout << "\b\b  " << std::endl;

  std::cout << "[AnalyzeModuleBase] *** z_cut: ";
  for (auto z : zCut)
  {
    std::cout << z << ", ";
  }
  std::cout << "\b\b  " << std::endl;
}

//--------------------------------------------------------------------------------------------------
double DynamicalGroom::CosOpeningAngle(double pmod1, double px1, double py1, double pz1,
                                       double pmod2, double px2, double py2, double pz2)
{
  return (px1 * px2 + py1 * py2 + pz1 * pz2) / pmod1 / pmod2;
}

void DynamicalGroom::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
{

  std::vector<fastjet::PseudoJet> fj_inputs;
  for (auto p : particle_list)
  {
    fj_inputs.push_back(p->GetPseudoJet());
  }

  for (int ir = 0; ir < jetR.size(); ir++)
  {

    double r_cone = jetR[ir];

    // Jet reconstruction
    fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, r_cone);

    // create an instance of the negative energy recombiner, with a given flag ui
    NegativeEnergyRecombiner uir(ui);
    // tell jet_def to use negative energy recombiner
    jetDef.set_recombiner(&uir);
    fastjet::ClusterSequence clustSeq(fj_inputs, jetDef);
    std::vector<fastjet::PseudoJet> jets = sorted_by_pt(clustSeq.inclusive_jets(reco_ptr->JetPtCut()));

    for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
    {
      for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
      {

        int n_jet = 0; // count number of jets in an event
        for (auto j : jets)
        {

          double pt_jet = j.pt();
          if (JetTrigger(j, ir, ijp, ijr))
          {
            n_jet++;
            // nParams for sets of parameters in the anlysis (e.g. beta and zcut)
            for (int ip = 0; ip < nParams; ip++)
            {

              auto ip_array = GetParamIndex(ip);
              double beta_val = beta[ip_array[0]];
              double zcut_val = zCut[ip_array[1]];

              // n_var is for 0:"zG", 1:"thetaG", 2:"rG", 3:"mG", 4:"mGOverPt", 5:"pseudoMG", 6:"pseudoMGOverPt"...
              std::array<std::vector<int>, n_var> index;
              for (int i = 0; i < n_var; i++)
              {
                // i_var[i] is vector. Each element of the vector is for each of multiple bin settings.
                // index[i] stores the indices for all bin settings for this parameter set.
                index[i] = GetHistIndex(i_var[i], ir, ijp, ijr, 0, 0, ip);
                // std::cout << varNames[i] << ": ";
                for (auto ii : index[i])
                {
                  // std::cout << ii << " ";
                  hist_list[ii]->JetTriggered();
                }
                // std::cout << endl;
              }

              //==========================================================================
              // Grooming
              //==========================================================================
              // Define Dynamical Grooming condition
              dyGroomer dyg(beta_val, zcut_val);
              bool hasSub = false;
              fastjet::PseudoJet daughter1, daughter2;
              fastjet::PseudoJet dyg_jet = dyg.doGrooming(j, daughter1, daughter2, hasSub);
              // std::cout << "[DynamicalGroom] groom-test: " << has_substructure << endl;
              //==========================================================================

              //==========================================================================
              // Set Groomed Jet Observables
              //==========================================================================
              double rg = -1.0;
              double zg = -1.0;
              double thg = -1.0;
              double mg = -1.0;
              double mg_over_pt = -1.0;
              double ktg = -1.0;
              //--
              if (hasSub)
              {
                // Fundamentals
                rg = daughter1.delta_R(daughter2);
                double ptsum = daughter1.pt() + daughter2.pt();
                double min_pt = min(daughter1.pt(), daughter2.pt());
                zg = min_pt / ptsum;
                thg = rg / r_cone;        // theta_g = rg/R
                mg = dyg_jet.m();         // groomed mass
                mg_over_pt = mg / pt_jet; // groomed mass/jetPt
                ktg = min_pt * sin(rg);   //
              }
              //==========================================================================

              //================================================================
              // 0:"zG", 1:"thetaG", 2:"rG", 3:"mG", 4:"mGOverPt", 5:"pseudoMG", 6:"pseudoMGOverPt"
              // std::cout << " ->" << varNames[0] << ": ";
              for (auto i : index[0])
              {
                // std::cout << i << " ";
                hist_list[i]->Fill(zg, 1.0);
              }
              // std::cout << std::endl;

              // std::cout << " ->" << varNames[1] << ": ";
              for (auto i : index[1])
              {
                // std::cout << i << " ";
                hist_list[i]->Fill(thg, 1.0);
              }
              // std::cout << std::endl;

              // std::cout << " ->" << varNames[2] << ": ";
              for (auto i : index[2])
              {
                // std::cout << i << " ";
                hist_list[i]->Fill(rg, 1.0);
              }
              // std::cout << std::endl;

              // std::cout << " ->" << varNames[3] << ": ";
              for (auto i : index[3])
              {
                // std::cout << i << " ";
                hist_list[i]->Fill(mg, 1.0);
              }
              // std::cout << std::endl;

              // std::cout << " ->" << varNames[4] << ": ";
              for (auto i : index[4])
              {
                // std::cout << i << " ";
                hist_list[i]->Fill(mg_over_pt, 1.0);
              }
              // std::cout << std::endl;

              // std::cout << " ->" << varNames[5] << ": ";
              for (auto i : index[5])
              {
                // std::cout << i << " ";
                hist_list[i]->Fill(ktg, 1.0);
              }
              // std::cout << std::endl;

              // //std::cout << " ->" << varNames[X] << ": ";
              // for (auto i : index[X])
              // {
              //   //std::cout << i << " ";
              //   hist_list[i]->Fill(pseudo_mg, 1.0);
              // }
              // //std::cout << std::endl;

              // //std::cout << " ->" << varNames[X] << ": ";
              // for (auto i : index[X])
              // {
              //   //std::cout << i << " ";
              //   hist_list[i]->Fill(pseudo_mg_over_pt, 1.0);
              // }
              // //std::cout << std::endl;
              //================================================================
            }

          } // trigger

          //====================================
          // Reach Maximum Triggered Jet Number per Tag
          if (nJetEv * (n_jet == nJetEv))
          {
            break;
          }
          //====================================

        } // jet
      }   // ijr
    }     // ijp

  } // jetR
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// bool DynamicalGroom::SDCondition(double z_g, double theta_g, double z_cut, double beta)
// {
//   if (theta_g > DBL_EPSILON)
//   {
//     if (z_g > z_cut * pow(theta_g, beta))
//     {
//       return true;
//     }
//   }
//   return false;
// }
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DynamicalGroom::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  //
  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[DynamicalGroom] hist_name = " << hist_name << std::endl;

  auto total_hist = CreateHist(hist_name, iv);
  auto normalized_hist = CreateHist("normalized_" + hist_name, iv);

  total_hist->Init();
  normalized_hist->Init();

  double nJetTotal = 0.0;

  for (auto hist : hist_list)
  {
    double n_ev = hist->Nev();
    if (n_ev != 0)
    {
      nJetTotal += hist->GetNjetSigmaOverEev();
      double sigma = hist->Sigma();
      total_hist->Add(hist, sigma / n_ev);
      normalized_hist->Add(hist, sigma / n_ev);
    }
  }
  // #############################################
  total_hist->Print("count_"); // millibarn
  if (nJetTotal != 0)
  {
    total_hist->Scale(1.0 / nJetTotal, "width");
    total_hist->Print("DynamicalGroom_");
  }
  else
  {
    std::cout << "[DynamicalGroom] 0-total Jet" << std::endl;
    std::cout << "[DynamicalGroom] Skip. " << std::endl;
  }
  total_hist->DeleteTH();
  // #############################################
  normalized_hist->Scale(1.0, "width");
  normalized_hist->Normalize("width");
  normalized_hist->Print("DynamicalGroom_");
  normalized_hist->DeleteTH();
  // #############################################
}
