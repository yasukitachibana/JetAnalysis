#include "ParticleSpec.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<ParticleSpec> ParticleSpec::reg("ParticleSpec");

ParticleSpec::ParticleSpec(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

ParticleSpec::~ParticleSpec()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

void ParticleSpec::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle)
{

  for (auto &p : particle_list)
  {

    double val = p->perp();
    double n = sub_ptr->nSub(p);

    for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
    {
      for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
      {

        if (ParticleTrigger(p, ipp, ipr))
        {
          for (int iv = 0; iv < variables.size(); iv++)
          {
            hist_list[GetHistIndex(iv, 0, 0, 0, ipp, ipr, 0)]
                ->Fill(val, n);
          }
        }
      }
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ParticleSpec::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[ParticleSpec] hist_name = " << hist_name << std::endl;

  double delta_rapidity = 2.0 * (particleRapMax[ijr] - particleRapMin[ijr]);
  std::cout << "[Particlepec] delta_rapidity = " << delta_rapidity << std::endl;

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

  total_hist->Scale(1.0, "width");
  total_hist->Print("particlespec_dNdpt_");
  total_hist->Scale(1.0 / delta_rapidity);
  total_hist->Print("particlespec_dNdptdrap_");

  total_hist->DeleteTH();
}
