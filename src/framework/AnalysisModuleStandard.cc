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

  OneEventAnalysis(particle_list);
  particle_list.clear();
  particle_list.shrink_to_fit();
  event_num++;
}

void AnalysisModuleStandard::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list)
{

  for (int ir = 0; ir < jetR.size(); ir++)
  {

    double r_cone = jetR[ir];
    int n_jet = 0;
    // Get Jets Reconstructed with Jet Cone Size = r_cone
    std::vector<fastjet::PseudoJet> jets = reco_ptr->JetReco(r_cone, particle_list);
    for (auto j : jets)
    {

      //std::cout << "jet#" << i_jet << std::endl;

      for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
      {
        for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
        {

          if (JetTrigger(j, ir, ijp, ijr))
          {

            SetObservable(j, particle_list, ir, ijp, ijr);
            //================================================
            // Jet Info Output
            // jet_infos
            //     << current_event << " "
            //     << n_jet << " "
            //     << j.perp() << " "
            //     << j.rapidity() << " "
            //     << j.phi() << " "
            //     << j.eta() << "\n";
            //================================================

          } //trigger
        }
      }

      n_jet++;
      if (n_jet == nJetEv)
      {
        break;
      }

    } //jet

  } //jetR
}
