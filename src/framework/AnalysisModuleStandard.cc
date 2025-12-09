#include "AnalysisModuleStandard.h"

void AnalysisModuleStandard::EventEndMark(std::vector<std::shared_ptr<Particle>> &particle_list, int &event_num)
{

  //================================================
  // Jet Info Output
  // if (event_num == 0)
  // {
  //   std::string jet_infos_filename = "jet_infos.txt";
  //   std::cout
  //       << "[AnalysisModuleStandard, EventEndMark] Creating Output File:"
  //       << jet_infos_filename
  //       << std::endl;
  //   jet_infos.open(jet_infos_filename.c_str(),
  //                  std::ios::out | std::ios::binary | std::ios::trunc);
  //   jet_infos
  //       << "# current_event" << " "
  //       << "n_jet" << " "
  //       << "perp()" << " "
  //       << "rapidity()" << " "
  //       << "phi()" << " "
  //       << "eta()" << "\n";
  // }
  // current_event = event_num;
  //================================================

  // std::cout << "=========================================" << std::endl;
  // std::cout << "Event Particle Info:" << std::endl;
  // std::cout << "n_particle = " << particle_list.size() << std::endl;
  // std::cout << "#0 pstat=" << particle_list[0]->pstat()
  //           << ", pid=" << particle_list[0]->pid()
  //           << ", pt=" << particle_list[0]->perp()
  //           << ", eta=" << particle_list[0]->eta() << std::endl;

  lead_ptr->SetEvent(particle_list);

  if (event_num % 2500 == 0)
  {
    std::cout
        << "Event" << event_num
        << " (" << std::to_string(getMemoryUsage()) << "MB) "
        << std::flush;

    if (((event_num / 2500) + 1) % 5 == 0)
    {
      std::cout << std::endl;
    }
  }

  //================================================
  // For Debug
  //  auto pf =  particle_list.back();
  //
  //  std::cout
  //  << "Event:" << event_num
  //  << ", particle number: " << particle_list.size()
  //  << ", stat:" << pf->pstat()
  //  << ", pid:"<< pf->pid()
  //  << ", plabel:" <<pf->plabel()
  //  << ", e:" <<pf->e()
  //  << ", px:" <<pf->px()
  //  << ", py:" <<pf->py()
  //  << ", eta:"<< pf->eta()
  //  << ", phi:" <<pf->phi_std()
  //  << std::endl;
  //================================================

  for (auto hist : hist_list)
  {
    hist->EventCount();
  }

  //--------------------------------------------------------
  int n_analysis = jet_tag_ptr->GetNAnalysis();
  for (int i_tag_particle = 0; i_tag_particle < n_analysis; i_tag_particle++)
  {
    // std::cout << "##i_analysis" << i << std::endl;
    // std::cout << "##->PhiTag = " << jet_tag_ptr->GetPhi(i);
    jet_deltaphi_ptr->PhiBasis(jet_tag_ptr->GetPhi(i_tag_particle));
    double pt_tag = jet_tag_ptr->GetPtTag(i_tag_particle);
    jetPtMinForTrigger = jet_tag_ptr->JetPtForTrigger(jetPtMin, pt_tag);
    jetPtMaxForTrigger = jet_tag_ptr->JetPtForTrigger(jetPtMax, pt_tag);

    //==============================
    // For debug
    // std::cout << "[AnalysisModuleStandard] *** pt_tag = "
    //           << pt_tag << " GeV" << std::endl;
    // for (int i = 0; i < jetPtMinForTrigger.size(); i++)
    // {
    //   std::cout << "[AnalysisModuleStandard] *** "
    //             << jetPtMinForTrigger[i]
    //             << " < pt_jet < "
    //             << jetPtMaxForTrigger[i]
    //             << " GeV" << std::endl;
    // }
    //==============================

    //==============================
    OneEventAnalysis(particle_list, i_tag_particle);
    //==============================
  }
  jet_tag_ptr->TagEventClear();
  //--------------------------------------------------------
  particle_list.clear();
  particle_list.shrink_to_fit();
  event_num++;

  // std::cout << "--Debug--[AnalysisModuleStandard]" << std::endl;
  // std::cout << "event_num: " << event_num << std::endl;
  // load_ptr->ShowLine();
}

void AnalysisModuleStandard::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
{

  for (int ir = 0; ir < jetR.size(); ir++)
  {

    double r_cone = jetR[ir];

    // Get Jets Reconstructed with Jet Cone Size = r_cone
    std::vector<fastjet::PseudoJet> jets = reco_ptr->JetReco(r_cone, particle_list);

    for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
    {
      for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
      {

        int n_jet = 0;
        for (auto j : jets)
        {

          // std::cout << "jet#" << i_jet << std::endl;

          if (JetTrigger(j, ir, ijp, ijr))
          {
            SetObservable(i_tag_particle, j, particle_list, ir, ijp, ijr);
            //================================================
            // Jet Info Output
            // std::cout << "#"
            //           << n_jet << " pt="
            //           << j.perp() << " rap="
            //           << j.rapidity() << " phi="
            //           << j.phi() << " eta="
            //           << j.eta() << std::endl;
            //================================================

            // Count Triggered Jet
            n_jet++;
          } // trigger

          //====================================
          // Reach Maximum Triggered Jet Number per Tag
          if (nJetEv * (n_jet == nJetEv))
          {
            break;
          }
          //====================================
        } // jet
      }
    }
  } // jetR
}
