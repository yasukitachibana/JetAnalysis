#include "CMSJetEnergyFlow.h"
#include "TMath.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<CMSJetEnergyFlow> CMSJetEnergyFlow::reg("CMSJetEnergyFlow");

CMSJetEnergyFlow::CMSJetEnergyFlow(std::string name_in) : name(name_in), ui(-123456)
{
  std::cout << "-@-Creating " << name << std::endl;
  std::cout << "Reconstruction Method is Fixed in " << name << "[Negative Recombiner]" << std::endl;
  std::cout << "Setting of Reconstruction in XML is to be ignored" << std::endl;
}

CMSJetEnergyFlow::~CMSJetEnergyFlow()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

int CMSJetEnergyFlow::ReadOptionParametersFromXML()
{
  delta_R_match = SetXML::Instance()->GetElementDouble({"observable", Name().c_str(), "delta_R_match"});

  match_jet_pt_min = SetXML::Instance()->GetElementDouble({"observable", Name().c_str(), "match_jet_pt_min"});
  match_jet_pt_max = SetXML::Instance()->GetElementDouble({"observable", Name().c_str(), "match_jet_pt_max"});

  return 1;
}

//------------------------------------------------------------

void CMSJetEnergyFlow::ShowParamsSetting()
{
  std::cout << "[AnalysisModuleBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalysisModuleBase] *** [CMSJetEnergyFlow]" << std::endl;

  std::cout << "[AnalysisModuleBase] *** DeltaRmatch: "
            << delta_R_match << std::endl;
}

//--------------------------------------------------------------------------------------------------

void CMSJetEnergyFlow::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
{

  // Pass all particles to jet reconstruction algorithm
  std::vector<fastjet::PseudoJet> fj_inputs;
  for (auto p : particle_list)
  {
    fj_inputs.push_back(p->GetPseudoJet());
  }

  for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
  {
    for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
    {

      std::vector<std::vector<fastjet::PseudoJet>> jetR_list;

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

        std::vector<fastjet::PseudoJet> triggered_jets; // Store jets that pass the trigger condition

        // Loop over jets and apply trigger condition
        for (auto j : jets)
        {
            // match_jet_pt_min and match_jet_pt_max are used to filter jets based on their transverse momentum
          if (JetTrigger(j, ir, ijr, match_jet_pt_min, match_jet_pt_max))
          {
            // Store the jet that passes the trigger condition
            triggered_jets.push_back({j});
          }
        }
        // Add the list of triggered jets for this cone size
        jetR_list.push_back(triggered_jets);
      }

      for (int ir = 0; ir < jetR.size() - 1; ir++)
      {
        int ir1 = ir;
        int ir2 = ir + 1;
        double r_cone1 = jetR[ir1]; // narrower jet cone size
        double r_cone2 = jetR[ir2]; // wider jet cone size

        std::vector<fastjet::PseudoJet> jet1_list = jetR_list[ir1]; // narrower cone jets
        std::vector<fastjet::PseudoJet> jet2_list = jetR_list[ir2]; // wider cone jets

        // Loop over jets in narrower cone
        for (std::size_t i_jet1 = 0; i_jet1 < jet1_list.size(); ++i_jet1)
        {

          // Get the current jet from the narrower cone list
          auto jet1 = jet1_list[i_jet1];

          // Check if the narrower jet passes the pT trigger condition
          if( jet1.pt() < jetPtMinForTrigger[ijp] || jet1.pt() > jetPtMaxForTrigger[ijp] )
          {
            continue;
          }

          // jet1.pt();


          // Initialize the matched candidate index
          std::size_t matched_jet2_candidate_index = -1;
          double min_delta_r = delta_R_match; // Initialize with the delta R match threshold

          for (std::size_t i_jet2 = 0; i_jet2 < jet2_list.size(); ++i_jet2)
          {
            // Get the current jet from the wider cone list
            auto jet2 = jet2_list[i_jet2];

            // Calculate delta R between the two jets
            double delta_r = jet1.delta_R(jet2);

            // Check if this is the closest match so far
            if (delta_r < min_delta_r)
            {
              matched_jet2_candidate_index = i_jet2;
              min_delta_r = delta_r;
            }
          }

          fastjet::PseudoJet matched_jet2_candidate;
          // If a match was found, process the matched jet
          if (matched_jet2_candidate_index != -1)
          {
            matched_jet2_candidate = jet2_list[matched_jet2_candidate_index];
          }
          else
          {
            continue; // Skip to the next narrower cone jet if no match is found
          }

          std::size_t matched_jet1_candidate_index = -1;
          min_delta_r = delta_R_match; // Reset for matching back to narrower cone
          for (std::size_t i_jet1_candidate = 0; i_jet1_candidate < jet1_list.size(); ++i_jet1_candidate)
          {
            // Get the current jet from the narrower cone list
            auto jet1_candidate = jet1_list[i_jet1_candidate];

            // Calculate delta R between the two jets
            double delta_r = matched_jet2_candidate.delta_R(jet1_candidate);

            // Check if this is the closest match so far
            if (delta_r < min_delta_r)
            {
              matched_jet1_candidate_index = i_jet1_candidate;
              min_delta_r = delta_r;
            }
          }

          // If a match was found, process the matched jet
          if (matched_jet1_candidate_index == i_jet1)
          {
            ////////////////////////////////////////////////////
            // Process the matched jets
            //--------------------------------------------------
            // std::cout << "[CMSJetEnergyFlow] Matched narrower cone " << r_cone1 << " jet: " << jet1.pt() << std::endl;
            // std::cout << "[CMSJetEnergyFlow] Matched wider cone " << r_cone2 << " jet: " << matched_jet2_candidate.pt() << std::endl;
            // std::cout<<"====="<<std::endl;
            //--------------------------------------------------
            fastjet::PseudoJet jet2 = matched_jet2_candidate;

            // Fill Histograms
            for (int iv = 0; iv < variables.size(); iv++)
            {
              int index = GetHistIndex(iv, ir, ijp, ijr, 0, 0, 0);

              hist_list[index]->JetTriggered();
              double delta_pt = jet2.pt() - jet1.pt();
              if (variables[iv] == "deltaPt")
              {

                hist_list[index]->Fill(delta_pt, 1.0);

                // std::cout << "[CMSJetEnergyFlow] Filling histograms for variable" << iv << " : " << variables[iv] << std::endl;
              }
              else if (variables[iv] == "meanDeltaPt")
              {
                hist_list[index]->Fill(0.5, delta_pt);
                // std::cout << "[CMSJetEnergyFlow] Filling histograms for variable" << iv << " : " << variables[iv] << std::endl;
              }
            }
          }
          else
          {
            continue; // Skip to the next narrower cone jet if no match is found
          }
        }
      }
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CMSJetEnergyFlow::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  //
  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[CMSJetEnergyFlow] hist_name = " << hist_name << std::endl;

  // Combine histograms for deltaPt

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
  total_hist->Print("count_"); // millibarn
  if (nJetTotal != 0)
  {
    total_hist->Scale(1.0 / nJetTotal, "width");
    total_hist->Print("CMSJetEnergyFlow_");
  }
  else
  {
    std::cout << "[CMSJetEnergyFlow] 0-total Jet" << std::endl;
    std::cout << "[CMSJetEnergyFlow] Skip. " << std::endl;
  }
  total_hist->DeleteTH();
}
