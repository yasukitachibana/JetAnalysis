#include "TotalEnergy.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<TotalEnergy> TotalEnergy::reg("TotalEnergy");

TotalEnergy::TotalEnergy(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

TotalEnergy::~TotalEnergy()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

int TotalEnergy::ReadOptionParametersFromXML()
{
  return 3;
}

std::string TotalEnergy::GetParamsTag(int i)
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
  else
  {
    return "error!";
  }
}

void TotalEnergy::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
{

  for (auto &p : particle_list)
  {

    double stat = p->pstat();
    double e = p->e();
    double pt = p->perp();

    for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
    {
      for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
      {

        if (ParticleTrigger(p, ipp, ipr))
        {
          for (int iv = 0; iv < variables.size(); iv++)
          {

            hist_list[GetHistIndex(iv, 0, 0, 0, ipp, ipr, 0)]
                ->Fill(stat, e);

            hist_list[GetHistIndex(iv, 0, 0, 0, ipp, ipr, 1)]
                ->Fill(stat, 1.0);

            hist_list[GetHistIndex(iv, 0, 0, 0, ipp, ipr, 2)]
                ->Fill(stat, pt);
          }
        }
      }
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TotalEnergy::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
}
