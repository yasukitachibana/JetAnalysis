#include "MomFracTagJet.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<MomFracTagJet> MomFracTagJet::reg("MomFracTagJet");

MomFracTagJet::MomFracTagJet(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

MomFracTagJet::~MomFracTagJet()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

//--------------------------------------------------------------------------------------------------
void MomFracTagJet::ShowParamsSetting()
{

  if (smearing)
  {
    std::cout << "[   MomFracTagJet  ] ***-------------------------------------------" << std::endl;
    std::cout << "[   MomFracTagJet  ] *** [MomFracTagJet]" << std::endl;
    smear_ptr->ShowSmearingSetting();
  }
}

int MomFracTagJet::ReadOptionParametersFromXML()
{

  // Smearing ---------------------------------
  smearing = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "smearing"}, false);
  if (smearing)
  {
    std::string smearing_method = SetXML::Instance()->GetElementText({"smearing", "method"});
    smear_ptr = SmearingModuleFactory::createInstance(smearing_method);
  }
  else
  {
    smear_ptr = SmearingModuleFactory::createInstance("NoSmearing");
  }
  smear_ptr->Init();
  //---------------------------------

  return 1;
}

std::string MomFracTagJet::GetParamsTag(int i)
{
  std::string tag = smear_ptr->SettingTag();
  return tag;
}

void MomFracTagJet::
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
        // Dummy Loop for Particle Cuts------------
        for (int iv = 0; iv < variables.size(); iv++)
        {
          for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
          {
            for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
            {

              // Get Index for Histogram
              int index = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, 0);

              // Trigger Tag (Not Jet!)
              hist_list[index]->JetTriggered();

              // Counter for Number of Triggered Jets per Tag
              int n_jet = 0;

              // For Loop for Jets
              for (auto &j : jets)
              {
                auto j_smeard = smear_ptr->Smear(j);

                // std::cout<< "-" << std::endl;
                // PrintPseudoJetInfo(j);
                // PrintPseudoJetInfo(j_smeard);

                //====================================
                // Trigger Jets
                if (JetTrigger(j_smeard, ir, ijp, ijr))
                {
                  // std::cout << "-" << std::endl;
                  // PrintPseudoJetInfo(j);
                  // PrintPseudoJetInfo(j_smeard);

                  // X_j-tag
                  double xJTag = j_smeard.perp() / pt_tag;

                  // Fill Histogram
                  hist_list[index]->Fill(xJTag, 1.0);

                  // Count Triggered Jet
                  n_jet++;
                }
                //====================================

                //====================================
                // Reach Maximum Triggered Jet Number per Tag
                if (nJetEv * (n_jet == nJetEv))
                {
                  break;
                }
                //====================================

              } // jet

            } // ipr
          }   // ipp
        }     // iv
        // End Dummy Loop--------------------------
      } // ijr
    }   // ijp
  }     // ir
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MomFracTagJet::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[MomFracTagJet] hist_name = " << hist_name << std::endl;

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

      // std::cout<< "----------" << std::endl;
      // total_hist->Show();
      // hist->Show();
      // std::cout<< "sigma / n_ev: " << sigma / n_ev << std::endl;
      total_hist->Add(hist, sigma / n_ev);
      // total_hist->Show();
    }
  }

  total_hist->Print("count_");

  // absolute normalization
  total_hist->Scale(1.0, "width");
  total_hist->ResetNjet(nTagTotal);
  total_hist->Print("abso_norm_");  

  if (nTagTotal != 0)
  {
    // bin width normalization has already been done for absolute normalization above
    total_hist->Scale(1.0 / nTagTotal);
    total_hist->Print("momfractagjet_");
  }
  else
  {
    std::cout << "[MomFracTagJet] 0-total Tag Particle" << std::endl;
    std::cout << "[MomFracTagJet] Skip. " << std::endl;
  }

  total_hist->DeleteTH();
}
