#include "AnalysisModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"
#include "Particle.h"

#include <float.h>

// ###############################################################################################################
//  Initialize static MakeUniqueHelper.here
Pythia8::Pythia AnalysisModuleBase::InternalHelperPythia("IntentionallyEmpty", false);
// ###############################################################################################################

// ###############################################################################################################
//  AnalysisModuleFactory, RegisterAnalysisModule
// ###############################################################################################################
//  Create an instance of the static map to register modules
AnalysisModuleFactory::map_type *AnalysisModuleFactory::moduleMap =
    new AnalysisModuleFactory::map_type;
// ###############################################################################################################

// ###############################################################################################################
//  AnalysisModuleBase
// ###############################################################################################################
void AnalysisModuleBase::Init(std::shared_ptr<ReconstructionModuleBase> reco_ptr_in,
                              std::shared_ptr<SubtractionModuleBase> sub_ptr_in,
                              std::shared_ptr<LoadFileModuleBase> load_ptr_in)
{

  std::cout << "[AnalysisModuleBase] Intialize " << Name() << std::endl;

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

int AnalysisModuleBase::Analyze(std::string input_file_name)
{

  std::cout
      << "[AnalysisModuleBase] Analyze " << Name()
      << " (" << std::to_string(getMemoryUsage()) << "MB) ..."
      << std::endl;

  bool loaded = load_ptr->Load(input_file_name);
  if (loaded)
  {
    std::vector<std::shared_ptr<Particle>> particle_list;
    int event_num = 0;
    jet_tag_ptr->TagEventClear();
    while (load_ptr->GetLine())
    {
      //    std::cout << "main" << std::endl;
      //    load_ptr->ShowLine();

      if (load_ptr->EventEnd())
      {
        //**************
        // Sort Test
        // PrintParticleInfoList(particle_list);
        // particle_list = sorted_by_pt(particle_list);
        // PrintParticleInfoList(particle_list);
        // exit(-1);
        //**************
        EventEndMark(particle_list, event_num);
        //**************
      }
      else if (load_ptr->ValidLine())
      {
        //**************
        auto p = load_ptr->GetParticle();

        // std::cout << p->pid() << " " << <<std::endl;
        if (RapidityCut(p) &&
            jet_charged_ptr->Trigger(p) &&
            jet_pstat_ptr->Trigger(p) &&
            jet_pid_ptr->Trigger(p) &&
            jet_constpt_ptr->Trigger(p))
        {
          particle_list.push_back(p);
        }
        // For jet tag
        jet_tag_ptr->Feed(p);
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
  }
  //*******************************************************************************************
  load_ptr->Clear();
  return loaded;
}

void AnalysisModuleBase::Set(double ptHatMin, double ptHatMax)
{
  GenerateHist(ptHatMin, ptHatMax);
}

void AnalysisModuleBase::Clear(int seq_loaded)
{
  std::cout << "[AnalysisModuleBase] Start Printing Histograms " << std::endl;
  if (seq_loaded)
  {
    for (auto hist : hist_list)
    {
      hist->Print();
    }
  }
  else
  {
    std::cout << "[AnalysisModuleBase] Skip. " << std::endl;
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

              } // option parameters
            }   // had_rap
          }     // had_pt
        }       // jet_rap
      }         // jet_pt
    }           // jetR
  }             // variable

  CombineFinisher();
}

// ###############################################################################################################

void AnalysisModuleBase::ReadParametersFromXML()
{
  // ###############################################################################################################
  //  <JetReco>
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
  jetPtMinForTrigger = jetPtMin;
  jetPtMaxForTrigger = jetPtMax;
  int jet_const_pt_min = SetXML::Instance()->GetElementDouble({"jetReco", "jetConstPtMin"}, false);
  int jet_const_pt_max = SetXML::Instance()->GetElementDouble({"jetReco", "jetConstPtMax"}, false);
  // ###############################################################################################################
  int leading_particle = 0;
  // ###############################################################################################################
  //  <observable>
  int ch_particle = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "chParticle"});
  std::vector<int> stat_particle = SetXML::Instance()->GetElementVectorInt({"observable", Name().c_str(), "statParticle", "Item"}, false);
  std::vector<int> pid_particle = SetXML::Instance()->GetElementVectorInt({"observable", Name().c_str(), "pidParticle", "Item"}, false);
  int particle_rapidity = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "particleRapidity"});
  particleRapMin = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particleRapMin", "Item"});
  particleRapMax = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particleRapMax", "Item"});
  particlePtMin = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particlePtMin", "Item"});
  particlePtMax = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particlePtMax", "Item"});
  int k0sStrangeness = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "K0SStrange"}, false);
  // ###############################################################################################################
  //  <inputFiles>
  p_gun = SetXML::Instance()->GetElementInt({"inputFiles", "sigma", "pGun"}, false);
  // ###############################################################################################################
  //  <jetTag>
  jet_tag = SetXML::Instance()->GetElementInt({"jetTag", "tagged"}, false);
  // ###############################################################################################################
  variables = SetXML::Instance()->GetElementNameVector({"observable", Name().c_str(), "var"});
  for (auto var : variables)
  {
    std::vector<double> bins = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "var", var.c_str(), "Item"});
    binSettings.push_back(bins);
  }
  // ###############################################################################################################
  nParams = ReadOptionParametersFromXML();
  // ###############################################################################################################

  ParticleBase::SetStrangeK0S(k0sStrangeness);

  if (ch_jet == 1)
  {
    if (ch_particle == 0)
    {
      std::cout << std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << "[AnalysisModuleBase] Bad Combination: Charged Jet and Charged+Neutral Particles" << std::endl;
      std::cout << "[AnalysisModuleBase] Exit. " << std::endl;
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
  // ###############################################################################################################
  if (stat_jet.size())
  {
    jet_pstat_ptr = std::unique_ptr<PStatSelected>(new PStatSelected(stat_jet));
  }
  else
  {
    jet_pstat_ptr = std::unique_ptr<PStatInclusive>(new PStatInclusive());
  }
  // ###############################################################################################################
  if (stat_particle.size())
  {
    particle_pstat_ptr = std::unique_ptr<PStatSelected>(new PStatSelected(stat_particle));
  }
  else
  {
    particle_pstat_ptr = std::unique_ptr<PStatInclusive>(new PStatInclusive());
  }
  // ###############################################################################################################
  if (pid_jet.size())
  {
    jet_pid_ptr = std::unique_ptr<PIDSelected>(new PIDSelected(pid_jet));
  }
  else
  {
    jet_pid_ptr = std::unique_ptr<PIDInclusive>(new PIDInclusive());
  }
  // ###############################################################################################################
  if (pid_particle.size())
  {
    particle_pid_ptr = std::unique_ptr<PIDSelected>(new PIDSelected(pid_particle));
  }
  else
  {
    particle_pid_ptr = std::unique_ptr<PIDInclusive>(new PIDInclusive());
  }
  // ###############################################################################################################
  if (jet_rapidity == 0)
  {
    jet_rap_ptr = std::unique_ptr<RapidityY>(new RapidityY());
  }
  else if (jet_rapidity == 1)
  {
    jet_rap_ptr = std::unique_ptr<PseudoRapidityEta>(new PseudoRapidityEta());
  }
  // ###############################################################################################################
  if (particle_rapidity == 0)
  {
    particle_rap_ptr = std::unique_ptr<RapidityY>(new RapidityY());
  }
  else if (particle_rapidity == 1)
  {
    particle_rap_ptr = std::unique_ptr<PseudoRapidityEta>(new PseudoRapidityEta());
  }
  // ###############################################################################################################
  if (jet_const_pt_max < DBL_EPSILON)
  {
    jet_constpt_ptr = std::unique_ptr<ConstPtInclusive>(new ConstPtInclusive());
  }
  else
  {
    jet_constpt_ptr = std::unique_ptr<ConstPtSelected>(new ConstPtSelected(jet_const_pt_min, jet_const_pt_max));
  }
  // ###############################################################################################################
  if (jet_tag)
  {
    jet_tag_ptr = std::unique_ptr<Tagged>(new Tagged(sub_ptr));
  }
  else
  {
    jet_tag_ptr = std::unique_ptr<NoTag>(new NoTag());
  }
  jet_tag_ptr->Init();
  // ###############################################################################################################
  if (jet_tag_ptr->Initialized())
  {
    if (jet_tag_ptr->DeltaPhiCut())
    {
      jet_deltaphi_ptr = std::unique_ptr<DeltaPhiCut>(new DeltaPhiCut(jet_tag_ptr->DeltaPhiMin()));
    }
    else
    {
      jet_deltaphi_ptr = std::unique_ptr<NoPhiCut>(new NoPhiCut());
    }
  }
  else
  {
    jet_tag_ptr->ShowJetTagSetting();
  }
  // ###############################################################################################################
  std::cout << std::endl;
  std::cout << "[AnalysisModuleBase] **********************************************" << std::endl;
  std::cout << "[AnalysisModuleBase] *** Settings" << std::endl;
  //-------------------------------------
  jet_tag_ptr->ShowJetTagSetting();
  jet_deltaphi_ptr->ShowDeltaPhiCutSetting();
  //-------------------------------------
  ShowObservableSetting();
  ShowJetSetting();
  ShowParticleSetting();
  ShowParamsSetting();
  std::cout << "[AnalysisModuleBase] **********************************************" << std::endl;
  std::cout << std::endl;
  // ###############################################################################################################
}

// ###############################################################################################################
void AnalysisModuleBase::ShowObservableSetting()
{
  std::cout << "[AnalysisModuleBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalysisModuleBase] *** [" << Name() << "]" << std::endl;
  std::cout << "[AnalysisModuleBase] *** variables: ";
  for (auto var : variables)
  {
    std::cout << var << ", ";
  }
  std::cout << "\b\b  " << std::endl;
}

void AnalysisModuleBase::ShowJetSetting()
{
  std::cout << "[AnalysisModuleBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalysisModuleBase] *** [Jet]" << std::endl;

  if (nJetEv > 0)
  {
    std::cout << "[AnalysisModuleBase] *** " << nJetEv << "-jets per an event (a tag)" << std::endl;
  }
  else
  {
    nJetEv = -1;
  }

  std::cout << "[AnalysisModuleBase] *** R_jet=";
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
    std::cout << "[AnalysisModuleBase] *** PID = ";
    for (auto pid : jet_pid_ptr->PIDList())
    {
      std::cout << pid << ", ";
    }
    std::cout << "\b\b  " << std::endl;
  }

  if (jet_constpt_ptr->Selected())
  {
    std::cout << "[AnalysisModuleBase] *** pt_constituents = "
              << jet_constpt_ptr->Min()
              << "-"
              << jet_constpt_ptr->Max()
              << " GeV"
              << std::endl;
  }

  std::string val_jetcut = jet_tag_ptr->ValJetCut();
  std::string unit_val_jetcut = jet_tag_ptr->UnitValJetCut();
  for (int i = 0; i < jetPtMin.size(); i++)
  {
    std::cout << "[AnalysisModuleBase] *** "
              << jetPtMin[i]
              << " < "
              << val_jetcut
              << " < "
              << jetPtMax[i]
              << " "
              << unit_val_jetcut << std::endl;
  }
  for (int i = 0; i < jetRapMin.size(); i++)
  {
    std::cout << "[AnalysisModuleBase] *** " << jetRapMin[i] << " < |" << jet_rap_ptr->Type() << "_jet| < " << jetRapMax[i] << std::endl;
  }
}

void AnalysisModuleBase::ShowParticleSetting()
{
  std::cout << "[AnalysisModuleBase] ***-------------------------------------------" << std::endl;
  std::cout << "[AnalysisModuleBase] *** [Particles]" << std::endl;
  std::cout << "[AnalysisModuleBase] *** " << particle_charged_ptr->Type() << " Particles";

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
    std::cout << "[AnalysisModuleBase] *** PID = ";
    for (auto pid : particle_pid_ptr->PIDList())
    {
      std::cout << pid << ", ";
    }
    std::cout << "\b\b  " << std::endl;
  }

  for (int i = 0; i < particlePtMin.size(); i++)
  {
    std::cout << "[AnalysisModuleBase] *** " << particlePtMin[i] << " < pt < " << particlePtMax[i] << " GeV" << std::endl;
  }
  for (int i = 0; i < particleRapMin.size(); i++)
  {
    std::cout << "[AnalysisModuleBase] *** " << particleRapMin[i] << " < |" << particle_rap_ptr->Type() << "| < " << particleRapMax[i] << std::endl;
  }
}
// ###############################################################################################################
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
  std::cout << "[AnalysisModuleBase] Largest Rapidity: " << largestRapidity << std::endl;
}
// ###############################################################################################################

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
                // std::cout << "[AnalysisModuleBase] generate histogram #" << nHist << " " << hist_name << std::endl;
                auto hist_this_bin = CreateHist(hist_name, iv);
                hist_this_bin->Init();
                hist_list.push_back(hist_this_bin);
                nHist++;
                // std::string hist_name_test = hist_list[GetHistIndex(iv,ir,ijp,ijr,ipp,ipr,ip)]->HistName();

                //
              } // option parameters
            }   // had_rap
          }     // had_pt
        }       // jet_rap
      }         // jet_pt
    }           // jetR
  }             // variable

  std::cout << "[AnalysisModuleBase] number of generated histogram: " << nHist << std::endl;
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
  if (p_gun == 1)
  {
    sigma = 1.0;
    sigma_err = 0.0;
  }
  else
  {
    std::string sigma_file_name = SetFile::Instance()->GetSigmaFileName(ptHatMin, ptHatMax);
    std::cout << "[AnalysisModuleBase] Load Sigma File: " << sigma_file_name << std::endl;
    load_ptr->LoadSigma(sigma_file_name, sigma, sigma_err);
  }
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

std::vector<int> AnalysisModuleBase::GetHistIndex(std::vector<int> iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  std::vector<int> indices;
  for (auto iiv : iv)
  {
    indices.push_back(GetHistIndex(iiv, ir, ijp, ijr, ipp, ipr, ip));
  }
  return indices;
}

int AnalysisModuleBase::GetHistIndex(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  return nParams * (particleRapMin.size() * (particlePtMin.size() * (jetRapMin.size() * (jetPtMin.size() * (jetR.size() * (iv) + ir) + ijp) + ijr) + ipp) + ipr) + ip;
}

std::shared_ptr<Histogram> AnalysisModuleBase::CreateHist(std::string hist_name, int iv)
{
  return std::make_shared<Hist1D>(hist_name, binSettings[iv]);
}

// ###############################################################################################################
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
// ###############################################################################################################
bool AnalysisModuleBase::JetTrigger(fastjet::PseudoJet jet, int ir, int ijp, int ijr)
{

  double pt_jet = jet.perp();
  double rapidity_jet = jet_rap_ptr->Val(jet);

  if (pt_jet >= jetPtMinForTrigger[ijp] &&
      pt_jet < jetPtMaxForTrigger[ijp] &&
      fabs(rapidity_jet) >= jetRapMin[ijr] &&
      fabs(rapidity_jet) < jetRapMax[ijr] &&
      jet_deltaphi_ptr->Trigger(jet))
  {
    return true;
  } // trigger

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

// ###############################################################################################################
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