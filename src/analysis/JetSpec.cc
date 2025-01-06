#include "JetSpec.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetSpec> JetSpec::reg("JetSpec");

JetSpec::JetSpec(std::string name_in) : name(name_in), ui(-123456)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetSpec::~JetSpec()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

// void JetSpec::SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
//                             std::vector<std::shared_ptr<Particle>> particle_list,
//                             int ir, int ijp, int ijr)
// {

//   for (int iv = 0; iv < variables.size(); iv++)
//   {
//     for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
//     {
//       for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
//       {

//         int index = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, 0);
//         hist_list[index]->JetTriggered();
//         hist_list[index]->Fill(jet.perp(), 1.0);

//       } // ipr
//     }   // ipp
//   }     // iv
// }

void JetSpec::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
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
            for (int iv = 0; iv < variables.size(); iv++)
            {
              for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
              {
                for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
                {

                  int index = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, 0);
                  hist_list[index]->JetTriggered();
                  hist_list[index]->Fill(j.perp(), 1.0);

                } // ipr
              }   // ipp
            }     // iv

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
void JetSpec::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[JetSpec] hist_name = " << hist_name << std::endl;

  double delta_rapidity = 2.0 * (jetRapMax[ijr] - jetRapMin[ijr]);
  std::cout << "[JetSpec] delta_rapidity = " << delta_rapidity << std::endl;

  auto total_hist = CreateHist(hist_name, iv);
  total_hist->Init();

  double nJetTotal = 0.0;

  for (auto hist : hist_list)
  {
    // ###########################
    //  Reset for Outliers
    hist->ResetOutliers(2.0);
    // ###########################
    double n_ev = hist->Nev();
    if (n_ev != 0)
    {
      nJetTotal += hist->GetNjetSigmaOverEev();
      double sigma = hist->Sigma();
      total_hist->Add(hist, sigma / n_ev);
    }
  }

  total_hist->Print("count_"); // millibarn

  total_hist->Scale(1.0, "width");
  total_hist->Print("jetspec_dNdpt_");
  total_hist->Scale(1.0 / delta_rapidity);
  total_hist->Print("jetspec_dNdptdrap_");

  total_hist->DeleteTH();
}
