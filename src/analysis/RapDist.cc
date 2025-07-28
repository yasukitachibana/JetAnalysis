#include "RapDist.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<RapDist> RapDist::reg("RapDist");

RapDist::RapDist(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

RapDist::~RapDist()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

int RapDist::ReadOptionParametersFromXML()
{
  return 4;
}

std::string RapDist::GetParamsTag(int i)
{

  if (i == 0)
  {
    return "e";
  }
  else if (i == 1)
  {
    return "n";
  }
  else if (i == 2)
  {
    return "pt";
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

void RapDist::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
{

  for (auto &p : particle_list)
  {

    double e = sub_ptr->eSub(p);
    double pt = sub_ptr->ptSub(p);
    double n = sub_ptr->nSub(p);
    double pz = sub_ptr->pzSub(p);
    double rap = particle_rap_ptr->Val(p);

    for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
    {
      for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
      {

        if (ParticleTrigger(p, ipp, ipr))
        {

          for (int iv = 0; iv < variables.size(); iv++)
          {

            hist_list[GetHistIndex(iv, 0, 0, 0, ipp, ipr, 0)]
                ->Fill(rap, e);

            hist_list[GetHistIndex(iv, 0, 0, 0, ipp, ipr, 1)]
                ->Fill(rap, n);

            hist_list[GetHistIndex(iv, 0, 0, 0, ipp, ipr, 2)]
                ->Fill(rap, pt);

            hist_list[GetHistIndex(iv, 0, 0, 0, ipp, ipr, 3)]
                ->Fill(rap, pz);
          }
        }
      }
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void RapDist::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[RapDist] hist_name = " << hist_name << std::endl;

  auto total_hist = CreateHist(hist_name, iv);
  total_hist->Init();

  double nEvent = 0.0;

  for (auto hist : hist_list)
  {
    double n_ev = hist->Nev();
    if (n_ev != 0)
    {
      double sigma = hist->Sigma();
      nEvent += sigma;
      total_hist->Add(hist, sigma / n_ev);
    }
  }

  total_hist->Print("count_");

  if (nEvent != 0)
  {
    total_hist->Scale(1.0 / nEvent, "width");
    total_hist->Print("rapiditydist_");
  }
  else
  {
    std::cout << "[FragF] 0-total Jet" << std::endl;
    std::cout << "[FragF] Skip. " << std::endl;
  }

  total_hist->DeleteTH();
}
