#include "SoftDropGroomConditional.h"
#include "TMath.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<SoftDropGroomConditional> SoftDropGroomConditional::reg("SoftDropGroomConditional");

SoftDropGroomConditional::SoftDropGroomConditional(std::string name_in) : name(name_in), ui(-123456)
{
  std::cout << "-@-Creating " << name << std::endl;
  std::cout << "Reconstruction Method is Fixed in " << name << "[Negative Recombiner]" << std::endl;
  std::cout << "Setting of Reconstruction in XML is to be ignored" << std::endl;
}

SoftDropGroomConditional::~SoftDropGroomConditional()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

//--------------------------------------------------------------------------------------------------

std::string SoftDropGroomConditional::GetConditionalCutTag(std::string cutname, double lower, double upper)
{
  std::ostringstream oss;

  oss << std::fixed
      << cutname 
      << std::setprecision(2) << (lower) << "-"
      << std::setprecision(2) << (upper);

  return oss.str();
}

void SoftDropGroomConditional::SetVaribalesWithCondition()
{
  std::cout << "SetVaribalesWithCondition " << std::endl;

  // n_var is for 0:"zG", 1:"thetaG", 2:"rG"
  
  // For zG ==================================================================
  for ( int i = 0; i < thetaGcutbins.size() - 1; i++)
  {
    // std::cout << GetConditionalCutTag(conditionalCutNames[0], thetaGcutbins[i], thetaGcutbins[i+1])
    //           << std::endl;

    std::string tag = GetConditionalCutTag(conditionalCutNames[0], thetaGcutbins[i], thetaGcutbins[i+1]);
    std::string var = varNames[0] + "_" + tag;     
    auto ite = std::find(variables.begin(), variables.end(), var);             
    if (ite != variables.end())
    {
      int index = distance(variables.begin(), ite);
      // std::cout << var << " " << *ite << ": " << index << std::endl;
      // std::cout << var << " " << index << std::endl;
      i_var[0].push_back(index);
    }
    else
    {
      std::cout << "[SoftDropGroomConditional] No Variables with Name " << var << std::endl;      
      std::cout << "[SoftDropGroomConditional] Please fix the XML " << var << std::endl;    
      std::cout << "[SoftDropGroomConditional] EXIT" << std::endl;          
      exit(-1);
    }
  }

  // For thetaG and rG =======================================================
  for ( int i = 0; i < zGcutbins.size() - 1; i++)
  {
    // std::cout << GetConditionalCutTag(conditionalCutNames[1], zGcutbins[i], zGcutbins[i+1])
    //           << std::endl;
    std::string tag = GetConditionalCutTag(conditionalCutNames[1], zGcutbins[i], zGcutbins[i+1]);

    // thetaG
    std::string var = varNames[1] + "_" + tag;
    auto ite = std::find(variables.begin(), variables.end(), var);         
    if (ite != variables.end())
    {
      int index = distance(variables.begin(), ite);
      // std::cout << var << " " << *ite << ": " << index << std::endl;
      // std::cout << var << " " << index << std::endl;
      i_var[1].push_back(index);
    }
    else
    {
      std::cout << "[SoftDropGroomConditional] No Variables with Name " << var << std::endl;      
      std::cout << "[SoftDropGroomConditional] Please fix the XML " << var << std::endl;    
      std::cout << "[SoftDropGroomConditional] EXIT" << std::endl;          
      exit(-1);
    }

    // rG
    var = varNames[2] + "_" + tag;
    ite = std::find(variables.begin(), variables.end(), var);         
    if (ite != variables.end())
    {
      int index = distance(variables.begin(), ite);
      // std::cout << var << " " << *ite << ": " << index << std::endl;
      // std::cout << var << " " << index << std::endl;
      i_var[2].push_back(index);
    }
    else
    {
      std::cout << "[SoftDropGroomConditional] No Variables with Name " << var << std::endl;      
      std::cout << "[SoftDropGroomConditional] Please fix the XML " << var << std::endl;    
      std::cout << "[SoftDropGroomConditional] EXIT" << std::endl;          
      exit(-1);
    }

  }
}


int SoftDropGroomConditional::ReadOptionParametersFromXML()
{

  beta = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "beta", "Item"});
  zCut = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "zCut", "Item"});
  double deltaRCut = SetXML::Instance()->GetElementDouble({"observable", Name().c_str(), "deltaRCut"}, false);

  thetaGcutbins = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "thetaGcutbins", "Item"}); 
  zGcutbins = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "zGcutbins", "Item"});    

  SetVaribalesWithCondition();
  // for (int i = 1; i < 30; i++)
  // {
  //   std::string i_str = VariableSuffix(i);
  //   // std::cout << "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV" << std::endl;
  //   // std::cout << i_str << std::endl;
  //   if (ReadVariablesFromXML(i_str) == 0)
  //   {
  //     break;
  //   }
  // }

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

  if (deltaRCut < DBL_EPSILON)
  {
    additional_cond_ptr = std::unique_ptr<SDNoCondition>(new SDNoCondition());
  }
  else
  {
    additional_cond_ptr = std::unique_ptr<SDDeltaRCut>(new SDDeltaRCut(deltaRCut));
  }

  return beta.size() * zCut.size();
}

//------------------------------------------------------------
// Get Tags for Parameters
std::string SoftDropGroomConditional::GetParamsTag(int i)
{
  return GetParamsTag(GetParamIndex(i));
}

std::string SoftDropGroomConditional::GetParamsTag(std::array<int, 2> i)
{
  return GetParamsTag(i[0], i[1]);
}

std::string SoftDropGroomConditional::GetParamsTag(int i_beta, int i_zCut)
{
  return GetParamsTag(beta[i_beta], zCut[i_zCut]);
}

std::string SoftDropGroomConditional::GetParamsTag(double beta_sd, double z_cut_sd)
{
  std::ostringstream oss;

  oss << std::fixed
      << "beta" << std::setprecision(2) << (beta_sd) << "_"
      << "zCut" << std::setprecision(2) << (z_cut_sd);

  return oss.str();
}
//------------------------------------------------------------
// Get Index of Tags for Parameters
int SoftDropGroomConditional::GetParamIndex(std::array<int, 2> i)
{
  return GetParamIndex(i[0], i[1]);
}

int SoftDropGroomConditional::GetParamIndex(int i_beta, int i_zCut)
{
  return zCut.size() * (i_beta) + i_zCut;
}

std::array<int, 2> SoftDropGroomConditional::GetParamIndex(int i)
{
  int i_beta = i / zCut.size();
  int i_zCut = i % zCut.size();
  return std::array<int, 2>{i_beta, i_zCut};
}
//------------------------------------------------------------

void SoftDropGroomConditional::ShowParamsSetting()
{
  std::cout << "[AnalyzeBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalyzeBase] *** [SoftDropGroomConditional]" << std::endl;

  std::cout << "[AnalyzeBase] *** beta: ";
  for (auto b : beta)
  {
    std::cout << b << ", ";
  }
  std::cout << "\b\b  " << std::endl;

  std::cout << "[AnalyzeBase] *** z_cut: ";
  for (auto z : zCut)
  {
    std::cout << z << ", ";
  }
  std::cout << "\b\b  " << std::endl;

  additional_cond_ptr->ShowSettings();
}

//--------------------------------------------------------------------------------------------------
double SoftDropGroomConditional::CosOpeningAngle(double pmod1, double px1, double py1, double pz1,
                                      double pmod2, double px2, double py2, double pz2)
{
  return (px1 * px2 + py1 * py2 + pz1 * pz2) / pmod1 / pmod2;
}

void SoftDropGroomConditional::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
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

              // Define SoftDrop condition
              fastjet::contrib::SoftDrop sd(beta_val, zcut_val, r_cone);

              // std::cout << "SoftDrop groomer is: " << sd.description() << std::endl;
              // Recombiner is automatically inherited (Manually checked with sample events).
              fastjet::PseudoJet sd_jet = sd(j);

              // Check user_index of groomed jet prongs
              int posSub = 1;
              fastjet::PseudoJet j1, j2;
              sd_jet.has_parents(j1, j2);

              while (j1.user_index() < 0 || j2.user_index() < 0)
              {
                // If both two prongs are negatives, we give up Grooming.
                if (j1.user_index() < 0 && j2.user_index() < 0)
                {
                  posSub = 0;
                  break;
                }

                // Continue grooming by passing the positive prong
                if (!(j1.user_index() < 0))
                {
                  sd_jet = sd(j1);
                }
                else
                {
                  sd_jet = sd(j2);
                }

                // Check whether the sd_jet has parents. If not, we give up grooming.
                if (!sd_jet.has_parents(j1, j2))
                {
                  posSub = 0;
                  break;
                }
              }
              // j1 should always be the harder of the two subjets.
              if (j1.perp() < j2.perp())
              {
                swap(j1, j2);
              }

              //-----------------------------------------------------------------------
              bool hasSub = sd_jet.structure_of<contrib::SoftDrop>().has_substructure();

              // Set
              double rg = sd_jet.structure_of<fastjet::contrib::SoftDrop>().delta_R();
              double zg = -1.0;
              // double mu = -1.0;
              double thg = -1.0;
              double mg = -1.0;
              double mg_over_pt = -1.0;
              double ktg = -1.0;
              double pseudo_mg = -1.0;
              double pseudo_mg_over_pt = -1.0;
              double ztg = -1.0;
              //--

              // std::cout << "\n jet-e" << j.e() << std::endl;
              // std::cout << " jgr-e" << sd_jet.e() << std::endl;
              // std::cout << " rg" << rg << std::endl;
              // std::cout << " zg" << sd_jet.structure_of<fastjet::contrib::SoftDrop>().symmetry() << std::endl;
              // exit(-1);

              if (posSub && hasSub && additional_cond_ptr->Trigger(rg))
              {
                // Fundamentals
                zg = sd_jet.structure_of<fastjet::contrib::SoftDrop>().symmetry();
                // mu = sd_jet.structure_of<fastjet::contrib::SoftDrop>().mu();
                //  Standard
                thg = rg / r_cone;        // theta_g = rg/R
                mg = sd_jet.m();          // groomed mass
                mg_over_pt = mg / pt_jet; // groomed mass/jetPt
                //-------------------------------
                // kt
                ktg = j2.perp() * sin(rg);
                ztg = zg * sin(rg);
                //-------------------------------
                // Pseudo Mass
                double cos_theta_j1_j2 = (j1.px() * j2.px() +
                                          j1.py() * j2.py() +
                                          j1.pz() * j2.pz()) /
                                         sqrt(j1.modp2() * j2.modp2());
                pseudo_mg = sqrt(2.0 * j1.e() * j2.e() * (1.0 - cos_theta_j1_j2));
                pseudo_mg_over_pt = pseudo_mg / pt_jet;
              }
              else
              {
                rg = -1.0;
              }

              //================================================================
              // 0:"zG", 1:"thetaG", 2:"rG",
              // 3:"mG", 4:"mGOverPt", 5:"pseudoMG", 6:"pseudoMGOverPt"
              // std::cout << " ->" << varNames[0] << ": ";


              for ( int i = 0; i < thetaGcutbins.size() - 1; i++)
              {
                if( thg >= thetaGcutbins[i] && thg < thetaGcutbins[i+1])
                {
                  hist_list[index[0][i]]->Fill(zg, 1.0);
                }
              }

              for ( int i = 0; i < zGcutbins.size() - 1; i++)
              {
                if( zg >= zGcutbins[i] && zg < zGcutbins[i+1])
                {
                  hist_list[index[1][i]]->Fill(thg, 1.0);
                  hist_list[index[2][i]]->Fill(rg, 1.0);
                }
              }              

              // std::cout << std::endl;

              // // std::cout << " ->" << varNames[3] << ": ";
              // for (auto i : index[3])
              // {
              //   // std::cout << i << " ";
              //   hist_list[i]->Fill(mg, 1.0);
              // }
              // // std::cout << std::endl;

              // // std::cout << " ->" << varNames[4] << ": ";
              // for (auto i : index[4])
              // {
              //   // std::cout << i << " ";
              //   hist_list[i]->Fill(mg_over_pt, 1.0);
              // }
              // // std::cout << std::endl;

              // // std::cout << " ->" << varNames[5] << ": ";
              // for (auto i : index[5])
              // {
              //   // std::cout << i << " ";
              //   hist_list[i]->Fill(ktg, 1.0);
              // }
              // // std::cout << std::endl;

              // // //std::cout << " ->" << varNames[X] << ": ";
              // for (auto i : index[6])
              // {
              //   // std::cout << i << " ";
              //   hist_list[i]->Fill(pseudo_mg, 1.0);
              // }
              // // std::cout << std::endl;

              // // //std::cout << " ->" << varNames[X] << ": ";
              // for (auto i : index[7])
              // {
              //   // std::cout << i << " ";
              //   hist_list[i]->Fill(pseudo_mg_over_pt, 1.0);
              // }
              // // std::cout << std::endl;

              // // //std::cout << " ->" << varNames[X] << ": ";
              // for (auto i : index[8])
              // {
              //   // std::cout << i << " ";
              //   hist_list[i]->Fill(ztg, 1.0);
              // }
              //================================================================
            }

          } // trigger

          //====================================
          // Reach Maximum Triggered Jet Number per Tag
          if (nJetEv && (n_jet == nJetEv))
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
// bool SoftDropGroomConditional::SDCondition(double z_g, double theta_g, double z_cut, double beta)
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
void SoftDropGroomConditional::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  //
  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[SoftDropGroomConditional] hist_name = " << hist_name << std::endl;

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
  total_hist->JetTriggered(nJetTotal);
  total_hist->Print("count_"); // millibarn
  if (nJetTotal != 0)
  {
    total_hist->Scale(1.0 / nJetTotal, "width");
    total_hist->Print("SoftDropGroomConditional_");
  }
  else
  {
    std::cout << "[SoftDropGroomConditional] 0-total Jet" << std::endl;
    std::cout << "[SoftDropGroomConditional] Skip. " << std::endl;
  }
  total_hist->DeleteTH();
  // #############################################
  normalized_hist->Scale(1.0, "width");
  normalized_hist->Normalize("width");
  normalized_hist->Print("SoftDropGroomConditional_");
  normalized_hist->DeleteTH();
  // #############################################
}
