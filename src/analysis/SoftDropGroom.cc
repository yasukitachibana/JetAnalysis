#include "SoftDropGroom.h"
#include "TMath.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<SoftDropGroom> SoftDropGroom::reg("SoftDropGroom");

SoftDropGroom::SoftDropGroom(std::string name_in) : name(name_in), ui(-123456)
{
  std::cout << "-@-Creating " << name << std::endl;
  std::cout << "Reconstruction Method is Fixed in " << name << "[Negative Recombiner]" << std::endl;
  std::cout << "Setting of Reconstruction in XML is to be ignored" << std::endl;
}

SoftDropGroom::~SoftDropGroom()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

//--------------------------------------------------------------------------------------------------
int SoftDropGroom::ReadOptionParametersFromXML()
{

  beta = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "beta", "Item"});
  zCut = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "zCut", "Item"});
  double deltaRCut = SetXML::Instance()->GetElementDouble({"observable", Name().c_str(), "deltaRCut"}, false);

  i_zG = distance(variables.begin(), std::find(variables.begin(), variables.end(), "zG"));
  i_thetaG = distance(variables.begin(), std::find(variables.begin(), variables.end(), "thetaG"));
  i_rG = distance(variables.begin(), std::find(variables.begin(), variables.end(), "rG"));

  if (deltaRCut < DBL_EPSILON)
  {
    additional_cond_ptr = std::unique_ptr<SDDeltaRCut>(new SDDeltaRCut(deltaRCut));
  }
  else
  {
    additional_cond_ptr = std::unique_ptr<SDNoCondition>(new SDNoCondition());
  }

  return beta.size() * zCut.size();
}

std::string SoftDropGroom::GetParamsTag(int i)
{
  return GetParamsTag(GetParamIndex(i));
}

std::string SoftDropGroom::GetParamsTag(std::array<int, 2> i)
{
  return GetParamsTag(i[0], i[1]);
}

std::string SoftDropGroom::GetParamsTag(int i_beta, int i_zCut)
{
  return GetParamsTag(beta[i_beta], zCut[i_zCut]);
}

std::string SoftDropGroom::GetParamsTag(double beta_sd, double z_cut_sd)
{
  std::ostringstream oss;

  oss << std::fixed
      << "beta" << std::setprecision(2) << (beta_sd) << "_"
      << "zCut" << std::setprecision(2) << (z_cut_sd);

  return oss.str();
}

int SoftDropGroom::GetParamIndex(std::array<int, 2> i)
{
  return GetParamIndex(i[0], i[1]);
}

int SoftDropGroom::GetParamIndex(int i_beta, int i_zCut)
{
  return zCut.size() * (i_beta) + i_zCut;
}

std::array<int, 2> SoftDropGroom::GetParamIndex(int i)
{
  int i_beta = i / zCut.size();
  int i_zCut = i % zCut.size();
  return std::array<int, 2>{i_beta, i_zCut};
}

void SoftDropGroom::ShowParamsSetting()
{
  std::cout << "[AnalyzeBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalyzeBase] *** [Parameters]" << std::endl;

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

  //additional_cond_ptr->ShowSettings();
}

//--------------------------------------------------------------------------------------------------

void SoftDropGroom::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list)
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

    for (auto j : jets)
    {
      for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
      {
        for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
        {

          if (JetTrigger(j, ir, ijp, ijr))
          {

            for (int ip = 0; ip < nParams; ip++)
            {

              auto ip_array = GetParamIndex(ip);
              double beta_val = beta[ip_array[0]];
              double zcut_val = zCut[ip_array[1]];

              int index_zG = GetHistIndex(i_zG, ir, ijp, ijr, 0, 0, ip);
              int index_thetaG = GetHistIndex(i_thetaG, ir, ijp, ijr, 0, 0, ip);
              int index_rG = GetHistIndex(i_rG, ir, ijp, ijr, 0, 0, ip);

              hist_list[index_zG]->JetTriggered();
              hist_list[index_thetaG]->JetTriggered();
              hist_list[index_rG]->JetTriggered();

              // Define SoftDrop condition
              fastjet::contrib::SoftDrop sd(beta_val, zcut_val);
              fastjet::PseudoJet sd_jet = sd(j);
              double zg = sd_jet.structure_of<fastjet::contrib::SoftDrop>().symmetry();
              double rg = sd_jet.structure_of<fastjet::contrib::SoftDrop>().delta_R();
              //bool hasSub = sd_jet.structure_of<contrib::SoftDrop>().has_substructure();
              double thg = rg / jetR[ir];

              if (SDCondition(zg, thg, zcut_val, beta_val) && additional_cond_ptr->Trigger(rg))
              {
                hist_list[index_zG]->Fill(zg, 1.0);
                hist_list[index_thetaG]->Fill(thg, 1.0);
                hist_list[index_rG]->Fill(rg, 1.0);
              }
            }

          } //trigger

        } //ijr
      }   //ijp
    }     //jet

  } //jetR
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool SoftDropGroom::SDCondition(double z_g, double theta_g, double z_cut, double beta)
{
  if (theta_g > DBL_EPSILON)
  {
    if (z_g > z_cut * pow(theta_g, beta))
    {
      return true;
    }
  }
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void SoftDropGroom::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  //
  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[SoftDropGroom] hist_name = " << hist_name << std::endl;

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
  //#############################################
  total_hist->Print("count_");
  if (nJetTotal != 0)
  {
    total_hist->Scale(1.0 / nJetTotal, "width");
    total_hist->Print("SoftDropGroom_");
  }
  else
  {
    std::cout << "[SoftDropGroom] 0-total Jet" << std::endl;
    std::cout << "[SoftDropGroom] Skip. " << std::endl;
  }
  total_hist->DeleteTH();
  //#############################################
  normalized_hist->Normalize("width");
  normalized_hist->Print("SoftDropGroom_");
  normalized_hist->DeleteTH();
  //#############################################
}
