#include "Dijet.h"
#include "TMath.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<Dijet> Dijet::reg("Dijet");

Dijet::Dijet(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

Dijet::~Dijet()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

//--------------------------------------------------------------------------------------------------
int Dijet::ReadVariablesFromXML(std::string tag)
{
  int exist = 0;
  // n_var is for 0:"xJ", 1:"pTLead", 2:"pTSub", 3:"deltaPhi", 4:"deltaPt"
  for (int i = 0; i < n_var; i++)
  {
    // tag specifies sets of parameters in the anlysis
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

std::string Dijet::VariableSuffix(int i)
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

int Dijet::ReadOptionParametersFromXML()
{
  // Set PT Cut Setting----------------------
  pTLeadMin = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "pTLeadMin", "Item"});
  pTLeadMax = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "pTLeadMax", "Item"});
  pTSubMin = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "pTSubMin", "Item"});
  pTSubMax = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "pTSubMax", "Item"});

  // Check the consistency of the number of parameters
  int n_param = pTLeadMin.size();
  if (n_param != pTLeadMax.size() ||
      n_param != pTSubMin.size() ||
      n_param != pTSubMax.size())
  {
    std::cout << "[Dijet] Error! Inconsistent number of pTLeadMin, pTLeadMax, pTSubMin, pTSubMax! Exit." << std::endl;
    exit(1);
  }
  // Set DeltaPhi Cut Setting----------------------
  delta_phi_cut = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "deltaPhiCut"});
  if (delta_phi_cut)
  {
    delta_phi_min = SetXML::Instance()->GetElementDouble({"observable", Name().c_str(), "deltaPhiMin"});
    delta_phi_min = DeltaPhiCutBase::PhiFormat(delta_phi_min);
    dijet_deltaphi_ptr = std::unique_ptr<DeltaPhiCut>(new DeltaPhiCut(delta_phi_min));
  }
  else
  {
    dijet_deltaphi_ptr = std::unique_ptr<NoPhiCut>(new NoPhiCut());
  }
  // Set xJ Cut Setting----------------------
  int xDijet = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "xDijet"});
  if (xDijet)
  {
    x_dijet_ptr = std::unique_ptr<TagXCut>(new TagXCut());
  }
  else
  {
    x_dijet_ptr = std::unique_ptr<TagPtJetCut>(new TagPtJetCut());
  }
  //-----------------------------------------
  for (int i = 1; i < 30; i++)
  {
    std::string i_str = VariableSuffix(i);
    // // std::cout << i_str << std::endl;
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

  // Return the number of parameters
  return n_param;
}

//------------------------------------------------------------
// Get Tags for Parameters
std::string Dijet::GetParamsTag(int i)
{
  return GetParamsTag(
      pTLeadMin[i], pTLeadMax[i],
      pTSubMin[i], pTSubMax[i]);
}

std::string Dijet::GetParamsTag(
    double pt_lead_min, double pt_lead_max,
    double pt_sub_min, double pt_sub_max)
{
  std::ostringstream oss;

  oss << std::fixed
      << "pt_lead"
      << std::setprecision(2) << (pt_lead_min)
      << "-"
      << std::setprecision(2) << (pt_lead_max)
      << "_"
      << "pt_sub"
      << std::setprecision(2) << (pt_sub_min)
      << "-"
      << std::setprecision(2) << (pt_sub_max);
  return oss.str();
}

void Dijet::ShowParamsSetting()
{
  std::cout << "[AnalyzeBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalyzeBase] *** [Dijet]" << std::endl;
  std::cout << "[AnalyzeBase] *** Maximum number of subleading jets per a leading jet: " << nJetEv << std::endl;
  for (int i = 0; i < pTLeadMin.size(); i++)
  {
    std::cout << "[AnalyzeBase] *** "
              << "Setting"
              << i
              << " ---"
              << std::endl;
    std::cout << "[AnalyzeBase] *** "
              << " -leading jet, pt_jet: "
              << pTLeadMin[i]
              << "-"
              << pTLeadMax[i]
              << " GeV"
              << std::endl;
    std::cout << "[AnalyzeBase] *** "
              << " -subleading jet, "
              << x_dijet_ptr->Var()
              << ": "
              << pTSubMin[i]
              << "-"
              << pTSubMax[i]
              << " "
              << x_dijet_ptr->UnitVar()
              << std::endl;
  }
  dijet_deltaphi_ptr->ShowDeltaPhiCutSetting();
}

//--------------------------------------------------------------------------------------------------

void Dijet::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
{
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

        for (int ip = 0; ip < nParams; ip++)
        {
          int n_dijet = 0; // count number of dijets in an event (If a dijet is found, we do not look for another dijet in the same event)
          for (int i_lead = 0; i_lead < jets.size(); i_lead++)
          {
            // Leading Jet
            fastjet::PseudoJet j_lead = jets[i_lead];
            if (JetTrigger(j_lead, ir, ijr, pTLeadMin[ip], pTLeadMax[ip]))
            {
              // Set Leading Jet Pt Cut
              double pt_jet_lead = j_lead.perp();

              // std::cout << "Trgger!: pt_jet_lead = " << pt_jet_lead << std::endl;

              double pt_jet_sub_min = x_dijet_ptr->SetJetPtCut(pTSubMin[ip], pt_jet_lead);
              double pt_jet_sub_max = x_dijet_ptr->SetJetPtCut(pTSubMax[ip], pt_jet_lead);

              // Set Leading Jet Phi
              dijet_deltaphi_ptr->PhiBasis(j_lead.phi());

              int n_sublead = 0; // count number of subleading jets in an event

              for (int i_sub = i_lead + 1; i_sub < jets.size(); i_sub++)
              {
                // Subleading Jet
                fastjet::PseudoJet j_sub = jets[i_sub];
                if (JetTrigger(j_sub, ir, ijr, pt_jet_sub_min, pt_jet_sub_max))
                {
                  if (dijet_deltaphi_ptr->Trigger(j_sub))
                  {
                    double pt_jet_sub = j_sub.perp();

                    // std::cout << "Trgger!: pt_jet_sub = " << pt_jet_sub << std::endl;

                    std::array<double, n_var> val = {pt_jet_sub/pt_jet_lead, pt_jet_lead, pt_jet_sub, 
                    dijet_deltaphi_ptr->DeltaPhi02Pi(j_sub.phi(),j_lead.phi()), pt_jet_lead-pt_jet_sub};

                    for (int i = 0; i < n_var; i++)
                    {
                      for (auto iv : i_var[i])
                      {
                        int index = GetHistIndex(iv, ir, ijp, ijr, 0, 0, ip);

                        hist_list[index]->JetTriggered();
                        hist_list[index]->Fill(val[i], 1.0);
                      }
                    }
                    n_dijet++;
                    n_sublead++;

                  } // sub trigger angle
                }   // sub trigger
                //====================================
                // Reach Maximum Subleading Jet Number per Leading Jet
                if (nJetEv * (n_sublead == nJetEv))
                {
                  break;
                }
                //====================================
              } // i_sub
            }   // lead trigger
            // If a dijet is found, we do not look for another dijet with different leading jet in the same event
            if (n_dijet > 0)
            {
              break;
            }
          } // i_lead
        }   // ip (leading and subleading jet pt cuts)
      }     // ijr
    }       // ijp
  }         // ir
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Dijet::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
}