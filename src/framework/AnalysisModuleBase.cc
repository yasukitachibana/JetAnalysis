#include "AnalysisModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"

#include <float.h>

//###############################################################################################################
// Initialize static MakeUniqueHelper.here
Pythia8::Pythia AnalysisModuleBase::InternalHelperPythia("IntentionallyEmpty", false);
//###############################################################################################################

//###############################################################################################################
// AnalysisModuleFactory, RegisterAnalysisModule
//###############################################################################################################
// Create an instance of the static map to register modules
AnalysisModuleFactory::map_type *AnalysisModuleFactory::moduleMap =
    new AnalysisModuleFactory::map_type;
//###############################################################################################################

//###############################################################################################################
// AnalysisModuleBase
//###############################################################################################################
void AnalysisModuleBase::Init(std::shared_ptr<ReconstructionModuleBase> reco_ptr_in,
                              std::shared_ptr<SubtractionModuleBase> sub_ptr_in,
                              std::shared_ptr<LoadFileModuleBase> load_ptr_in)
{

  std::cout << "[AnalyzeBase] Intialize " << Name() << std::endl;

  reco_ptr = nullptr;
  sub_ptr = nullptr;
  load_ptr = nullptr;

  reco_ptr = reco_ptr_in;
  sub_ptr = sub_ptr_in;
  load_ptr = load_ptr_in;

  ReadParametersFromXML();
  SetJetPtCut();
  SetLargestRapidity();

  InitMixedEvent();
}

void AnalysisModuleBase::Analyze(std::string input_file_name)
{

  std::cout
      << "[AnalysisModuleBase] Analyze " << Name()
      << " (" << std::to_string(getMemoryUsage()) << "MB) ..."
      << std::endl;

  load_ptr->Load(input_file_name);
  std::vector<std::shared_ptr<Particle>> particle_list;

  int event_num = 0;
  while (load_ptr->GetLine())
  {
    //    std::cout << "main" << std::endl;
    //    load_ptr->ShowLine();

    if (load_ptr->EventEnd())
    {
      //**************
      EventEndMark(particle_list, event_num);
      //**************
    }
    else if (load_ptr->ValidLine())
    {
      //**************
      auto p = load_ptr->GetParticle();

      //std::cout << p->pid() << " " << <<std::endl;
      if (RapidityCut(p) &&
          jet_charged_ptr->Trigger(p) &&
          jet_pstat_ptr->Trigger(p) &&
          jet_pid_ptr->Trigger(p) &&
          jet_constpt_ptr->Trigger(p))
      {
        particle_list.push_back(p);
      }
      //**************
    }
  }
  if (load_ptr->Last())
  {
    //**************
    EventEndMark(particle_list, event_num);
    //**************
  }
  std::cout
      << "\n[AnalysisModuleBase] Last Event" << event_num
      << " -- DONE! (" << std::to_string(getMemoryUsage()) << "MB) ..."
      << std::endl;
  //*******************************************************************************************
  load_ptr->Clear();
}

void AnalysisModuleBase::Set(double ptHatMin, double ptHatMax)
{
  GenerateHist(ptHatMin, ptHatMax);
}

void AnalysisModuleBase::Clear()
{
  for (auto hist : hist_list)
  {
    hist->Print();
  }
  DeleteHist();
}

void AnalysisModuleBase::Combine(std::vector<double> ptHat)
{

  for (int iv = 0; iv < variables.size(); iv++)
  {
    for (int ir = 0; ir < jetR.size(); ir++)
    {
      for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
      {
        for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
        {
          for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
          {
            for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
            {
              for (int ip = 0; ip < nParams; ip++)
              {

                //_____________________________________________________________________________________
                int nPtHatBins = ptHat.size() - 1;
                for (int i_pthat_bin = 0; i_pthat_bin < nPtHatBins; i_pthat_bin++)
                {
                  LoadHist(ptHat[i_pthat_bin], ptHat[i_pthat_bin + 1], iv, ir, ijp, ijr, ipp, ipr, ip);
                }
                //_____________________________________________________________________________________
                CombineHist(iv, ir, ijp, ijr, ipp, ipr, ip);
                DeleteHist();

              } //option parameters
            }   //had_rap
          }     //had_pt
        }       //jet_rap
      }         //jet_pt
    }           //jetR
  }             //variable

  CombineFinisher();
}

//###############################################################################################################

void AnalysisModuleBase::ReadParametersFromXML()
{
  //###############################################################################################################
  nJetEv = SetXML::Instance()->GetElementInt({"jetReco", "nJetEv"});
  jetR = SetXML::Instance()->GetElementVectorDouble({"jetReco", "jetR", "Item"});
  int ch_jet = SetXML::Instance()->GetElementInt({"jetReco", "chJet"});
  std::vector<int> stat_jet = SetXML::Instance()->GetElementVectorInt({"jetReco", "statJet", "Item"}, false);
  std::vector<int> pid_jet = SetXML::Instance()->GetElementVectorInt({"jetReco", "pidJet", "Item"}, false);
  int jet_rapidity = SetXML::Instance()->GetElementInt({"jetReco", "jetRapidity"});
  jetRapMin = SetXML::Instance()->GetElementVectorDouble({"jetReco", "jetRapMin", "Item"});
  jetRapMax = SetXML::Instance()->GetElementVectorDouble({"jetReco", "jetRapMax", "Item"});
  jetPtMin = SetXML::Instance()->GetElementVectorDouble({"jetReco", "jetPtMin", "Item"});
  jetPtMax = SetXML::Instance()->GetElementVectorDouble({"jetReco", "jetPtMax", "Item"});
  int jet_const_pt_min = SetXML::Instance()->GetElementDouble({"jetReco", "jetConstPtMin"}, false);
  int jet_const_pt_max = SetXML::Instance()->GetElementDouble({"jetReco", "jetConstPtMax"}, false);
  //###############################################################################################################
  int ch_particle = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "chParticle"});
  std::vector<int> stat_particle = SetXML::Instance()->GetElementVectorInt({"observable", Name().c_str(), "statParticle", "Item"}, false);
  std::vector<int> pid_particle = SetXML::Instance()->GetElementVectorInt({"observable", Name().c_str(), "pidParticle", "Item"}, false);
  int particle_rapidity = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "particleRapidity"});
  particleRapMin = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particleRapMin", "Item"});
  particleRapMax = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particleRapMax", "Item"});
  particlePtMin = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particlePtMin", "Item"});
  particlePtMax = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particlePtMax", "Item"});
  //###############################################################################################################
  variables = SetXML::Instance()->GetElementNameVector({"observable", Name().c_str(), "var"});
  for (auto var : variables)
  {
    std::vector<double> bins = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "var", var.c_str(), "Item"});
    binSettings.push_back(bins);
  }
  //###############################################################################################################
  nParams = ReadOptionParametersFromXML();
  //###############################################################################################################

  if (ch_jet == 1)
  {
    if (ch_particle == 0)
    {
      std::cout << std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << "[AnalyzeBase] Bad Combination: Charged Jet and Charged+Neutral Particles" << std::endl;
      std::cout << "[AnalyzeBase] Exit. " << std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << std::endl;
      exit(-1);
    }
    jet_charged_ptr = std::unique_ptr<ChargedOnly>(new ChargedOnly());
    particle_charged_ptr = std::unique_ptr<ChargedOnly>(new ChargedOnly());
  }
  else
  {
    jet_charged_ptr = std::unique_ptr<IncludeNeutral>(new IncludeNeutral());
    if (ch_particle == 1)
    {
      particle_charged_ptr = std::unique_ptr<ChargedOnly>(new ChargedOnly());
    }
    else
    {
      particle_charged_ptr = std::unique_ptr<IncludeNeutral>(new IncludeNeutral());
    }
  }
  //###############################################################################################################
  if (stat_jet.size())
  {
    jet_pstat_ptr = std::unique_ptr<PStatSelected>(new PStatSelected(stat_jet));
  }
  else
  {
    jet_pstat_ptr = std::unique_ptr<PStatInclusive>(new PStatInclusive());
  }
  //###############################################################################################################
  if (stat_particle.size())
  {
    particle_pstat_ptr = std::unique_ptr<PStatSelected>(new PStatSelected(stat_particle));
  }
  else
  {
    particle_pstat_ptr = std::unique_ptr<PStatInclusive>(new PStatInclusive());
  }
  //###############################################################################################################
  if (pid_jet.size())
  {
    jet_pid_ptr = std::unique_ptr<PIDSelected>(new PIDSelected(pid_jet));
  }
  else
  {
    jet_pid_ptr = std::unique_ptr<PIDInclusive>(new PIDInclusive());
  }
  //###############################################################################################################
  if (pid_particle.size())
  {
    particle_pid_ptr = std::unique_ptr<PIDSelected>(new PIDSelected(pid_particle));
  }
  else
  {
    particle_pid_ptr = std::unique_ptr<PIDInclusive>(new PIDInclusive());
  }
  //###############################################################################################################
  if (jet_rapidity == 0)
  {
    jet_rap_ptr = std::unique_ptr<RapidityY>(new RapidityY());
  }
  else if (jet_rapidity == 1)
  {
    jet_rap_ptr = std::unique_ptr<PseudoRapidityEta>(new PseudoRapidityEta());
  }
  //###############################################################################################################
  if (particle_rapidity == 0)
  {
    particle_rap_ptr = std::unique_ptr<RapidityY>(new RapidityY());
  }
  else if (particle_rapidity == 1)
  {
    particle_rap_ptr = std::unique_ptr<PseudoRapidityEta>(new PseudoRapidityEta());
  }

  if (jet_const_pt_max < DBL_EPSILON)
  {
    jet_constpt_ptr = std::unique_ptr<ConstPtInclusive>(new ConstPtInclusive());
  }
  else
  {
    jet_constpt_ptr 
    = std::unique_ptr<ConstPtSelected>
    (new ConstPtSelected(jet_const_pt_min, jet_const_pt_max));
  }

  //###############################################################################################################
  std::cout << std::endl;
  std::cout << "[AnalyzeBase] **********************************************" << std::endl;
  std::cout << "[AnalyzeBase] *** Settings" << std::endl;
  ShowObservableSetting();
  ShowJetSetting();
  ShowParticleSetting();
  ShowParamsSetting();
  std::cout << "[AnalyzeBase] **********************************************" << std::endl;
  std::cout << std::endl;
  //###############################################################################################################
}

//###############################################################################################################
void AnalysisModuleBase::ShowObservableSetting()
{
  std::cout << "[AnalyzeBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalyzeBase] *** [" << Name() << "]" << std::endl;
  std::cout << "[AnalyzeBase] *** variables: ";
  for (auto var : variables)
  {
    std::cout << var << ", ";
  }
  std::cout << "\b\b  " << std::endl;
}

void AnalysisModuleBase::ShowJetSetting()
{
  std::cout << "[AnalyzeBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalyzeBase] *** [Jet]" << std::endl;

  if (nJetEv != 0)
  {
    std::cout << "[AnalyzeBase] *** " << nJetEv << "-jets per events" << std::endl;
  }

  std::cout << "[AnalyzeBase] *** R_jet=";
  for (auto r_cone : jetR)
  {
    std::cout << r_cone << ", ";
  }

  std::cout << jet_charged_ptr->Type() << "Jet";
  if (jet_pstat_ptr->StatList().size())
  {
    std::cout << ", Status = ";
    for (auto st : jet_pstat_ptr->StatList())
    {
      std::cout << st << ", ";
    }
    std::cout << "\b\b  ";
  }
  std::cout << std::endl;

  if (jet_pid_ptr->PIDList().size())
  {
    std::cout << "[AnalyzeBase] *** PID = ";
    for (auto pid : jet_pid_ptr->PIDList())
    {
      std::cout << pid << ", ";
    }
    std::cout << "\b\b  " << std::endl;
  }

  if (jet_constpt_ptr->Selected())
  {
    std::cout << "[AnalyzeBase] *** pt_constituents = "
              << jet_constpt_ptr->Min()
              << "-"
              << jet_constpt_ptr->Max()
              << " GeV"
              << std::endl;
  }

  for (int i = 0; i < jetPtMin.size(); i++)
  {
    std::cout << "[AnalyzeBase] *** " << jetPtMin[i] << " < pt_jet < " << jetPtMax[i] << " GeV" << std::endl;
  }
  for (int i = 0; i < jetRapMin.size(); i++)
  {
    std::cout << "[AnalyzeBase] *** " << jetRapMin[i] << " < |" << jet_rap_ptr->Type() << "_jet| < " << jetRapMax[i] << std::endl;
  }
}

void AnalysisModuleBase::ShowParticleSetting()
{
  std::cout << "[AnalyzeBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalyzeBase] *** [Particles]" << std::endl;
  std::cout << "[AnalyzeBase] *** " << particle_charged_ptr->Type() << " Particles";

  if (particle_pstat_ptr->StatList().size())
  {
    std::cout << ", Status = ";
    for (auto st : particle_pstat_ptr->StatList())
    {
      std::cout << st << ", ";
    }
    std::cout << "\b\b  ";
  }
  std::cout << std::endl;

  if (particle_pid_ptr->PIDList().size())
  {
    std::cout << "[AnalyzeBase] *** PID = ";
    for (auto pid : particle_pid_ptr->PIDList())
    {
      std::cout << pid << ", ";
    }
    std::cout << "\b\b  " << std::endl;
  }

  for (int i = 0; i < particlePtMin.size(); i++)
  {
    std::cout << "[AnalyzeBase] *** " << particlePtMin[i] << " < pt < " << particlePtMax[i] << " GeV" << std::endl;
  }
  for (int i = 0; i < particleRapMin.size(); i++)
  {
    std::cout << "[AnalyzeBase] *** " << particleRapMin[i] << " < |" << particle_rap_ptr->Type() << "| < " << particleRapMax[i] << std::endl;
  }
}
//###############################################################################################################
void AnalysisModuleBase::SetJetPtCut()
{
  double jetPtCut = 10000.0;
  double pt = 0.0;

  for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
  {
    pt = jetPtMin[ijp];
    if (jetPtCut > pt)
    {
      jetPtCut = pt;
    }
  }
  double factor = 0.6;
  if (reco_ptr->Name() == "NegativeReco")
  {
    factor = 1.0;
  }
  jetPtCut = factor * jetPtCut;
  reco_ptr->SetJetPtCut(jetPtCut);
}

void AnalysisModuleBase::SetLargestRapidity()
{
  largestRapidity = 0.0;
  double rap;
  double jetRmax = *max_element(jetR.begin(), jetR.end());

  for (int i_jet_rap = 0; i_jet_rap < jetRapMax.size(); i_jet_rap++)
  {
    rap = jetRapMax[i_jet_rap] + 1.2 * jetRmax;
    if (largestRapidity < rap)
    {
      largestRapidity = rap;
    }
  }

  for (int i_particle_rap = 0; i_particle_rap < particleRapMax.size(); i_particle_rap++)
  {
    rap = particleRapMax[i_particle_rap];
    if (largestRapidity < rap)
    {
      largestRapidity = rap;
    }
  }
  std::cout << "[AnalyzeBase] Largest Rapidity: " << largestRapidity << std::endl;
}
//###############################################################################################################

void AnalysisModuleBase::GenerateHist(double ptHatMin, double ptHatMax)
{

  int nHist = 0;

  for (int iv = 0; iv < variables.size(); iv++)
  {
    for (int ir = 0; ir < jetR.size(); ir++)
    {
      for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
      {
        for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
        {
          for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
          {
            for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
            {
              for (int ip = 0; ip < nParams; ip++)
              {

                std::string hist_name = GetHistName(ptHatMin, ptHatMax, iv, ir, ijp, ijr, ipp, ipr, ip);
                //std::cout << "[AnalyzeBase] generate histogram #" << nHist << " " << hist_name << std::endl;
                auto hist_this_bin = CreateHist(hist_name, iv);
                hist_this_bin->Init();
                hist_list.push_back(hist_this_bin);
                nHist++;
                //std::string hist_name_test = hist_list[GetHistIndex(iv,ir,ijp,ijr,ipp,ipr,ip)]->HistName();

                //
              } //option parameters
            }   //had_rap
          }     //had_pt
        }       //jet_rap
      }         //jet_pt
    }           //jetR
  }             //variable

  std::cout << "[AnalyzeBase] number of generated histogram: " << nHist << std::endl;
}

void AnalysisModuleBase::LoadHist(double ptHatMin, double ptHatMax,
                                  int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  //*******************************************************************************************
  std::string hist_name = GetHistName(ptHatMin, ptHatMax, iv, ir, ijp, ijr, ipp, ipr, ip);
  auto hist_this_bin = CreateHist(hist_name, iv);
  hist_this_bin->Init();
  hist_this_bin->LoadHistFromFile();
  //*******************************************************************************************
  double sigma, sigma_err;
  std::string sigma_file_name = SetFile::Instance()->GetSigmaFileName(ptHatMin, ptHatMax);
  std::cout << "[AnalysisModuleBase] Load Sigma File: " << sigma_file_name << std::endl;
  load_ptr->LoadSigma(sigma_file_name, sigma, sigma_err);
  std::cout << "[AnalysisModuleBase] Sigma = " << sigma << ", sigma error = " << sigma_err << std::endl;
  hist_this_bin->SetSigma(sigma, sigma_err);
  //*******************************************************************************************
  hist_list.push_back(hist_this_bin);
}

void AnalysisModuleBase::DeleteHist()
{
  for (auto hist : hist_list)
  {
    hist->DeleteTH();
  }
  hist_list.clear();
  hist_list.shrink_to_fit();
}

std::string AnalysisModuleBase::GetHistName(double ptHatMin, double ptHatMax,
                                            int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string tag = GetParamsTag(ip);
  std::string histname = SetFile::Instance()->GetHistName(ptHatMin, ptHatMax,
                                                          Name(),
                                                          variables[iv],
                                                          jetR[ir],
                                                          jetPtMin[ijp], jetPtMax[ijp],
                                                          jetRapMin[ijr], jetRapMax[ijr],
                                                          particlePtMin[ipp], particlePtMax[ipp],
                                                          particleRapMin[ipr], particleRapMax[ipr], tag);
  return histname;
}

std::string AnalysisModuleBase::GetHistName(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string tag = GetParamsTag(ip);

  std::string histname = SetFile::Instance()->GetHistName(Name(),
                                                          variables[iv],
                                                          jetR[ir],
                                                          jetPtMin[ijp], jetPtMax[ijp],
                                                          jetRapMin[ijr], jetRapMax[ijr],
                                                          particlePtMin[ipp], particlePtMax[ipp],
                                                          particleRapMin[ipr], particleRapMax[ipr], tag);
  return histname;
}

int AnalysisModuleBase::GetHistIndex(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  return nParams * (particleRapMin.size() * (particlePtMin.size() * (jetRapMin.size() * (jetPtMin.size() * (jetR.size() * (iv) + ir) + ijp) + ijr) + ipp) + ipr) + ip;
}

std::shared_ptr<Histogram> AnalysisModuleBase::CreateHist(std::string hist_name, int iv)
{
  return std::make_shared<Hist1D>(hist_name, binSettings[iv]);
}

//###############################################################################################################
bool AnalysisModuleBase::RapidityCut(std::shared_ptr<Particle> p)
{

  double rapidity = particle_rap_ptr->Val(p);

  if (fabs(rapidity) <= largestRapidity)
  {
    return true;
  }
  else
  {
    return false;
  }
}
//###############################################################################################################
bool AnalysisModuleBase::JetTrigger(fastjet::PseudoJet jet, int ir, int ijp, int ijr)
{

  double pt_jet = jet.perp();
  double rapidity_jet = jet_rap_ptr->Val(jet);

  if (pt_jet >= jetPtMin[ijp] &&
      pt_jet < jetPtMax[ijp] &&
      fabs(rapidity_jet) >= jetRapMin[ijr] &&
      fabs(rapidity_jet) < jetRapMax[ijr])
  {
    return true;
  } //trigger

  return false;
}

bool AnalysisModuleBase::ParticleTrigger(std::shared_ptr<Particle> p, int ipp, int ipr)
{

  if (particle_charged_ptr->Trigger(p) && particle_pstat_ptr->Trigger(p) && particle_pid_ptr->Trigger(p))
  {

    double pt = p->perp();
    double rapidity = particle_rap_ptr->Val(p);

    if (pt >= particlePtMin[ipp] && pt < particlePtMax[ipp] &&
        fabs(rapidity) >= particleRapMin[ipr] && fabs(rapidity) < particleRapMax[ipr])
    {
      return true;
    }
  }
  return false;
}

//###############################################################################################################
long AnalysisModuleBase::getMemoryUsage()
{
  struct rusage usage;
  // NOTE: Reported in kB on BSD/Linux, bytes in Mac/Darwin
  // Could try to explicitly catch __linux__ as well
  float mbsize = 1024;
#ifdef __MACH__
  mbsize = 1024 * 1024;
#endif

  if (0 == getrusage(RUSAGE_SELF, &usage))
    return usage.ru_maxrss / mbsize;
  else
    return 0;
}
