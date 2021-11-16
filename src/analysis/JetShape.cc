#include "JetShape.h"
#include "TMath.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetShape> JetShape::reg("JetShape");

JetShape::JetShape(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetShape::~JetShape()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

int JetShape::ReadOptionParametersFromXML()
{
  return n_p;
}

std::string JetShape::GetParamsTag(int i)
{

  if (i == 0)
  {
    return "pt";
  }
  else if (i == 1)
  {
    return "n";
  }
  else if (i == 2)
  {
    return "e";
  }
  else if (i == 3)
  {
    return "pz";
  }
  else
  {
    return "Error!";
  }
}

void JetShape::SetObservable(fastjet::PseudoJet jet,
                             std::vector<std::shared_ptr<Particle>> particle_list,
                             int ir, int ijp, int ijr)
{
  //--------------------------------------------------------------------------------------------------
  for (int iv = 0; iv < variables.size(); iv++)
  {
    for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
    {
      for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
      {

        std::array<int, n_p> index;
        for (int ip = 0; ip < n_p; ip++)
        {
          int ii = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, ip);
          index[ip] = ii;
          hist_list[ii]->JetTriggered();
        }

        //        std::cout << "############\n"
        //        << "particle number: " << particle_list.size() <<std::endl;

        for (auto p : particle_list)
        {
          //          std::cout
          //            << "\n[BT] stat:" << p->pstat()
          //            << ", pid:"<< p->pid()
          //            << ", plabel:" <<p->plabel()
          //            << ", e:" <<p->e()
          //            << ", px:" <<p->px()
          //            << ", py:" <<p->py()
          //            << ", eta:"<< p->eta()
          //            << ", phi:" <<p->phi_std()
          //            << std::endl;

          if (ParticleTrigger(p, ipp, ipr))
          {

            //  std::cout
            //    << "[T] stat:" << p->pstat()
            //    << ", pid:"<< p->pid()
            //    << ", plabel:" <<p->plabel()
            //    << ", e:" <<p->e()
            //    << ", px:" <<p->px()
            //    << ", py:" <<p->py()
            //    << ", eta:"<< p->eta()
            //    << ", phi:" <<p->phi_std()
            //    << std::endl;

            std::array<double, n_p> val = {sub_ptr->ptSub(p),
                                           sub_ptr->nSub(p),
                                           sub_ptr->eSub(p),
                                           sub_ptr->pzSub(p)};

            double delta_eta = p->eta() - jet.eta();
            double delta_phi = jet.delta_phi_to(p->GetPseudoJet());
            double delta_r = TMath::Sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

            // std::cout
            //  << "[T] stat:" << p->pstat()
            //  << ", pt_eff:"<< pt
            //  << std::endl;

            //            std::cout
            //              << "[T] pt:" << pt
            //              << ", eta_j:"<< jet.eta()
            //              << ", phi_j:"<< jet.phi()
            //              << ", delta_eta:"<< delta_eta
            //              << ", delta_phi:" << delta_phi
            //              << ", delta_r:" << delta_r
            //              << std::endl;
            //--------------------------------------------------------------------------------------------------
            for (int ip = 0; ip < n_p; ip++)
            {
              hist_list[index[ip]]->Fill(delta_r, val[ip]);
            }
            //--------------------------------------------------------------------------------------------------
          }
        }

      } //ipr
    }   //ipp
  }     //iv
  //--------------------------------------------------------------------------------------------------
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void JetShape::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[JetShape] hist_name = " << hist_name << std::endl;

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

  total_hist->Print("count_");

  if (nJetTotal != 0)
  {
    total_hist->Scale(1.0 / nJetTotal, "width");
    total_hist->Print("jetshape_");
    total_hist->Normalize("width");
    total_hist->Print("normalized_jetshape_");
  }
  else
  {
    std::cout << "[JetShape] 0-total Jet" << std::endl;
    std::cout << "[JetShape] Skip. " << std::endl;
  }

  total_hist->DeleteTH();
}
