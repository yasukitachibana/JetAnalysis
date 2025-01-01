#include "HardestKt.h"
#include "TMath.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<HardestKt> HardestKt::reg("HardestKt");

HardestKt::HardestKt(std::string name_in) : name(name_in), ui(-123456)
{
  std::cout << "-@-Creating " << name << std::endl;
  std::cout << "Reconstruction Method is Fixed in " << name << "[Negative Recombiner]" << std::endl;
  std::cout << "Setting of Reconstruction in XML is to be ignored" << std::endl;
}

HardestKt::~HardestKt()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

//--------------------------------------------------------------------------------------------------
int HardestKt::ReadVariablesFromXML(std::string tag)
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

std::string HardestKt::VariableSuffix(int i)
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

int HardestKt::ReadOptionParametersFromXML()
{

  ktBins = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "ktBins", "Item"});

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

  return ktBins.size() - 1;
}

//------------------------------------------------------------
// Get Tags for Parameters
std::string HardestKt::GetParamsTag(int i)
{
  return GetParamsTag(ktBins[i], ktBins[i + 1]);
}

std::string HardestKt::GetParamsTag(double ktMin, double ktMax)
{
  std::ostringstream oss;
  oss << std::fixed
      << "kt" << std::setprecision(1) << (ktMin) << "-"
      << std::setprecision(1) << (ktMax);
  return oss.str();
}

void HardestKt::ShowParamsSetting()
{
  std::cout << "[AnalyzeModuleBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalyzeModuleBase] *** [HardestKt]" << std::endl;

  std::cout << "[AnalyzeModuleBase] *** ktBins: ";
  for (int i = 0; i < nParams; i++)
  {
    std::cout << ktBins[i] << "--" << ktBins[i + 1] << ", ";
  }
  std::cout << "\b\b  " << std::endl;
}

//--------------------------------------------------------------------------------------------------
// double HardestKt::CosOpeningAngle(double pmod1, double px1, double py1, double pz1,
//                                        double pmod2, double px2, double py2, double pz2)
// {
//   return (px1 * px2 + py1 * py2 + pz1 * pz2) / pmod1 / pmod2;
// }

fastjet::PseudoJet
HardestKt::IdentifyHardestKt(fastjet::PseudoJet jet,
                             fastjet::PseudoJet &daughter1,
                             fastjet::PseudoJet &daughter2,
                             bool &has_substructure)
{
  //===============================================================
  // Set Cambrige-Aachen
  fastjet::JetDefinition jet_def_ca(fastjet::cambridge_algorithm, 1.);
  // Set Recombiner----
  // create an instance of the negative energy recombiner, with a given flag ui
  NegativeEnergyRecombiner uir_ca(ui);
  // tell jet_def_ca to use negative energy recombiner
  jet_def_ca.set_recombiner(&uir_ca);
  //===============================================================
  // Reclustering by Cambrige-Aachen
  std::vector<fastjet::PseudoJet> particles = jet.constituents();
  fastjet::ClusterSequence cs_ca(particles, jet_def_ca);
  std::vector<fastjet::PseudoJet> output_jets = cs_ca.inclusive_jets(1);
  output_jets = sorted_by_pt(output_jets);
  fastjet::PseudoJet jj = output_jets[0];
  //===============================================================
  // Declustering by Cambrige-Aachen
  fastjet::PseudoJet j1; // subjet 1 (largest pt)
  fastjet::PseudoJet j2; // subjet 2 (smaller pt)
  double ktmax = -100;
  fastjet::PseudoJet hardest_kt_jet;
  while (jj.has_parents(j1, j2))
  {

    if (j1.user_index() < 0 && j2.user_index() < 0)
    {
      // Both subjets are negatives. Exit.
      break;
    }
    else if (j1.user_index() < 0)
    {
      // j1 is negative. Assign j2 to jj and continue.
      jj = j2;
      continue;
    }
    else if (j2.user_index() < 0)
    {
      // j2 is negative. Assign j1 to jj and continue.
      jj = j1;
      continue;
    }

    // Ensure j1 has the higher transverse momentum (pT)
    if (j1.perp() < j2.perp())
    {
      std::swap(j1, j2);
    }

    // Calculate the angular separation (delta R) and the kT value
    double delta_R = j1.delta_R(j2);
    double kt = j2.perp() * delta_R; // CMS definition of kt

    // Update ktmax if a new maximum kT is found
    if (kt > ktmax)
    {
      has_substructure = true;
      ktmax = kt;
      hardest_kt_jet = jj;
      daughter1 = j1;
      daughter2 = j2;
    }

    // Assign j1 (the leading jet) to jj
    jj = j1;
  }

  return hardest_kt_jet;
}

void HardestKt::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
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
            // nParams for sets of parameters in the anlysis

            //==========================================================================
            // Find hardest kt
            //==========================================================================
            bool hasSub = false;
            fastjet::PseudoJet daughter1, daughter2;
            fastjet::PseudoJet hardest_kt_jet = IdentifyHardestKt(j, daughter1, daughter2, hasSub);
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
              mg = hardest_kt_jet.m();  // groomed mass
              mg_over_pt = mg / pt_jet; // groomed mass/jetPt
              ktg = min_pt * rg;        // CMS definition of kt
            }
            //==========================================================================

            for (int ip = 0; ip < nParams; ip++)
            {
              // Set kt bin
              double ktMin = ktBins[ip];
              double ktMax = ktBins[ip + 1];

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

              if (ktg >= ktMin && ktg < ktMax)
              {
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
                //   //std::cout <<
                //================================================================
              } // kt bin
            } // ip

          } // trigger

          //====================================
          // Reach Maximum Triggered Jet Number per Tag
          if (nJetEv * (n_jet == nJetEv))
          {
            break;
          }
          //====================================

        } // jet
      } // ijr
    } // ijp

  } // jetR
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// bool HardestKt::SDCondition(double z_g, double theta_g, double z_cut, double beta)
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
void HardestKt::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  //
  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[HardestKt] hist_name = " << hist_name << std::endl;

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
    total_hist->Print("HardestKt_");
  }
  else
  {
    std::cout << "[HardestKt] 0-total Jet" << std::endl;
    std::cout << "[HardestKt] Skip. " << std::endl;
  }
  total_hist->DeleteTH();
  // #############################################
  normalized_hist->Scale(1.0, "width");
  normalized_hist->Normalize("width");
  normalized_hist->Print("HardestKt_");
  normalized_hist->DeleteTH();
  // #############################################
}
