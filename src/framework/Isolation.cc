#include "Isolation.h"
#include "SetXML.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================
void IsolationBase::ShowIsolationSetting()
{
  if (initialized == 0)
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[IsolationBase] Isolation is NOT initialized." << std::endl;
    std::cout << "[IsolationBase] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
  else
  {
    PrintIsolationSetting();
  }
}
//===========================================================================================================================
// Isolated
//===========================================================================================================================
Isolated::Isolated(std::shared_ptr<SubtractionModuleBase> sub_ptr_in)
{
  sub_ptr = nullptr;
  sub_ptr = sub_ptr_in;
}

Isolated::Isolated()
{
  sub_ptr = nullptr;
}

void Isolated::SetSubstruction(std::shared_ptr<SubtractionModuleBase> sub_ptr_in)
{
  sub_ptr = nullptr;
  sub_ptr = sub_ptr_in;
  if (initialized == 1)
  {
  }
}

void Isolated::IsoEventClear()
{
  // std::cout << "0 iso_check_list.size():" << iso_check_list.size() << std::endl;
  iso_check_list.clear();
  iso_check_list.shrink_to_fit();
  // std::cout << "1 iso_check_list.size():" << iso_check_list.size() << std::endl;
}

void Isolated::Init(std::string xml_tag_name)
{
  if (sub_ptr != nullptr)
  {
    ReadParametersFromXML(xml_tag_name);
    initialized = 1;
  }
}

void Isolated::SetRapidityCut(double eta_max_tag)
{
  iso_rap_max = eta_max_tag + 1.2 * rIsolation;
}

void Isolated::PrintIsolationSetting()
{
  //==============================================================================
  std::cout << "[Isolated:Isolation] ***-------------------------------------------" << std::endl;
  std::cout << "[Isolated:Isolation] *** [Isolation]" << std::endl;
  std::cout << "[Isolated:Isolation] *** R = " << rIsolation
            << ", accum. "
            << iso_var_ptr->VarName()
            << " < " << cutIsolation << " GeV" << std::endl;
  std::cout << "[Isolated:Isolation] *** Count " << iso_charged_ptr->Type()
            << " Particles";
  if (iso_pstat_ptr->StatList().size())
  {
    std::cout << ", Status = ";
    for (auto st : iso_pstat_ptr->StatList())
    {
      std::cout << st << ", ";
    }
    std::cout << "\b\b";
  }
  std::cout << std::endl;
  //==============================================================================
  std::cout << "[Isolated:Isolation] *** " << iso_rap_min << " < |" << iso_rap_ptr->Type() << "| < " << iso_rap_max << std::endl;
  //==============================================================================
}

void Isolated::ReadParametersFromXML(std::string xml_tag_name)
{
  rIsolation = SetXML::Instance()->GetElementDouble({xml_tag_name.c_str(), "RIsolation"});
  cutIsolation = SetXML::Instance()->GetElementDouble({xml_tag_name.c_str(), "cutIsolation"});
  int charged = SetXML::Instance()->GetElementInt({xml_tag_name.c_str(), "chIsolation"}, false);
  std::vector<int> stat = SetXML::Instance()->GetElementVectorInt({xml_tag_name.c_str(), "statIsolation", "Item"}, false);
  std::string varIsolation = SetXML::Instance()->GetElementText({xml_tag_name.c_str(), "varIsolation"}, false);
  //------------------------------------------------------------------------------
  if (charged)
  {
    iso_charged_ptr = std::unique_ptr<ChargedOnly>(new ChargedOnly());
  }
  else
  {
    iso_charged_ptr = std::unique_ptr<IncludeNeutral>(new IncludeNeutral());
  }
  //------------------------------------------------------------------------------
  if (stat.size())
  {
    iso_pstat_ptr = std::unique_ptr<PStatSelected>(new PStatSelected(stat));
  }
  else
  {
    iso_pstat_ptr = std::unique_ptr<PStatInclusive>(new PStatInclusive());
  }
  if (varIsolation == "" ||
      varIsolation == "pt" || varIsolation == "pT" ||
      varIsolation == "Pt" || varIsolation == "PT")
  {
    iso_var_ptr = std::unique_ptr<PtIsolation>(new PtIsolation(sub_ptr));
  }
  else if (varIsolation == "et" || varIsolation == "eT" ||
           varIsolation == "Et" || varIsolation == "ET")
  {
    iso_var_ptr = std::unique_ptr<EtIsolation>(new EtIsolation(sub_ptr));
  }
  else
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[Isolated] Bad Input for Quantity for Isolation Cut." << std::endl;
    std::cout << "[Isolated] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
  //------------------------------------------------------------------------------
  iso_rap_ptr = std::unique_ptr<PseudoRapidityEta>(new PseudoRapidityEta());
  iso_pt_min = 0.0;
  iso_pt_max = 1000000.0;
  iso_rap_min = 0.0;
  iso_rap_max = 50.0;
  //------------------------------------------------------------------------------
}

void Isolated::Feed(std::shared_ptr<Particle> p)
{
  // p->PrintInfo();
  if (IsoTrigger(p))
  {
    // p->PrintInfo();
    // std::cout << "---> Pass!" << std::endl;
    iso_check_list.push_back(p);
  }
  // std::cout << "-----" << std::endl;
}

bool Isolated::IsoTrigger(std::shared_ptr<Particle> p)
{
  if (iso_pstat_ptr->Trigger(p) && iso_charged_ptr->Trigger(p))
  {

    double pt = p->perp();
    double rapidity = iso_rap_ptr->Val(p);

    if (pt >= iso_pt_min && pt < iso_pt_max &&
        fabs(rapidity) >= iso_rap_min && fabs(rapidity) < iso_rap_max)
    {
      return true;
    }
  }
  return false;
}

std::vector<std::shared_ptr<Particle>>
Isolated::IsolationPassed(std::vector<std::shared_ptr<Particle>> tag_list, int n_max)
{
  std::vector<std::shared_ptr<Particle>> tag_passed_list;
  int n_tag_passed = 0;

  for (size_t i = 0; (i < tag_list.size() && n_tag_passed != n_max); i++)
  {
    if (IsolationCheck(tag_list[i]))
    {
      tag_passed_list.push_back(tag_list[i]);
      n_tag_passed++;
    }
  }

  return tag_passed_list;
}

bool Isolated::IsolationCheck(std::shared_ptr<Particle> tag)
{
  double total_pt_in_cone = 0.0;

  // Subtract the contribution from the tag particle itself
  if (IsoTrigger(tag))
  {
    total_pt_in_cone -= iso_var_ptr->GetValue(tag);
  }

  // Calculate Accumulated pT in Tag Isolation Cone
  for (auto p : iso_check_list)
  {
    double delta_eta = p->eta() - tag->eta();
    double delta_phi = tag->delta_phi_to(p->GetPseudoJet());
    double delta_r2 = (delta_eta * delta_eta) + (delta_phi * delta_phi);

    if (delta_r2 <= rIsolation * rIsolation)
    {
      total_pt_in_cone += iso_var_ptr->GetValue(p);
      // p->PrintInfo(0);
    }
  }

  // Isolation Check
  if (total_pt_in_cone >= cutIsolation)
  {
    return false;
  }
  else
  {
    return true;
  }
}

//===========================================================================================================================
// NoIsolation
//===========================================================================================================================
void NoIsolation::Init(std::string xml_tag_name)
{
  Init();
}

void NoIsolation::Init()
{
  initialized = 1;
}

void NoIsolation::PrintIsolationSetting() {}

std::vector<std::shared_ptr<Particle>>
NoIsolation::IsolationPassed(std::vector<std::shared_ptr<Particle>> tag_list, int n_max)
{

  if (n_max == -1)
  {
    return tag_list;
  }

  int n_size = n_max;
  std::vector<std::shared_ptr<Particle>> tag_passed_list;
  for (size_t i = 0; i < n_size; i++)
  {
    tag_passed_list.push_back(tag_list[i]);
  }

  return tag_passed_list;
}
