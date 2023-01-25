#include "JetTag.h"
#include "SetXML.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================
void JetTagBase::ShowJetTagSetting()
{
  if (initialized == 0)
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[JetTagBase] JetTag is NOT initialized." << std::endl;
    std::cout << "[JetTagBase] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
  else
  {
    PrintJetTagSetting();
  }
}

std::string JetTagBase::VarJetCut()
{
  return x_tag_ptr->Var();
}
std::string JetTagBase::UnitVarJetCut()
{
  return x_tag_ptr->UnitVar();
}

std::vector<double>
JetTagBase::JetPtForTrigger(const std::vector<double> &jet_pt, const double pt_tag)
{
  return x_tag_ptr->SetJetPtCut(jet_pt, pt_tag);
}

//===========================================================================================================================
// Tagged
//===========================================================================================================================
Tagged::Tagged(std::shared_ptr<SubtractionModuleBase> sub_ptr_in)
{
  sub_ptr = nullptr;
  sub_ptr = sub_ptr_in;
}

void Tagged::Init()
{
  if (sub_ptr != nullptr)
  {
    ReadParametersFromXML();
    initialized = 1;
    tag_sorted = 0;
  }
}

void Tagged::TagEventClear()
{
  //-----------------------
  tag_list.clear();
  tag_list.shrink_to_fit();
  tag_sorted = 0;
  //-----------------------
  tag_iso_ptr->IsoEventClear();
  //-----------------------
}

void Tagged::Feed(std::shared_ptr<Particle> p)
{
  if (TagTrigger(p))
  {
    // p->PrintInfo();
    tag_list.push_back(p);
  }
  tag_iso_ptr->Feed(p);
}

bool Tagged::TagTrigger(std::shared_ptr<Particle> p)
{
  if (tag_pstat_ptr->Trigger(p) && tag_pid_ptr->Trigger(p))
  {

    double pt = p->perp();
    double rapidity = tag_rap_ptr->Val(p);

    if (pt >= tag_pt_min && pt < tag_pt_max &&
        fabs(rapidity) >= tag_rap_min && fabs(rapidity) < tag_rap_max)
    {
      return true;
    }
  }
  return false;
}

void Tagged::PrintJetTagSetting()
{
  //==============================================================================
  std::cout << "[  Tagged: JetTag  ] ***-------------------------------------------" << std::endl;
  std::cout << "[  Tagged: JetTag  ] *** [Tagged]" << std::endl;
  //==============================================================================
  std::cout << "[  Tagged: JetTag  ] *** Tag Particles: ";
  for (auto pid : tag_pid_ptr->PIDList())
  {
    std::cout << pid << ", ";
  }
  //------------------------------------------------------------------------------
  std::cout << " Status = ";
  for (auto st : tag_pstat_ptr->StatList())
  {
    std::cout << st << ", ";
  }
  std::cout << "\b\b  ";
  std::cout << std::endl;
  //==============================================================================
  std::cout << "[  Tagged: JetTag  ] *** pt_tag: " << tag_pt_min << "-" << tag_pt_max << " GeV" << std::endl;
  //==============================================================================
  std::cout << "[  Tagged: JetTag  ] *** |" << tag_rap_ptr->Type() << "_tag|: " << tag_rap_min << "-" << tag_rap_max << std::endl;
  //==============================================================================
  n_tag_ptr->ShowTagNumberSetting();
  tag_iso_ptr->ShowIsolationSetting();
}

void Tagged::ReadParametersFromXML()
{
  //------------------------------------------------------------------------------
  int n_tag_per_ev = SetXML::Instance()->GetElementInt({"jetTag", "nTagEv"});
  int xJetTag = SetXML::Instance()->GetElementInt({"jetTag", "xJetTag"});
  std::vector<int> pid_tag = SetXML::Instance()->GetElementVectorInt({"jetTag", "tagParticle", "Item"});
  std::vector<int> stat_tag = SetXML::Instance()->GetElementVectorInt({"jetTag", "tagStat", "Item"}, false);
  int tag_rapidity = SetXML::Instance()->GetElementInt({"jetTag", "tagRapidity"});
  tag_pt_min = SetXML::Instance()->GetElementDouble({"jetTag", "tagPtMin"});
  tag_pt_max = SetXML::Instance()->GetElementDouble({"jetTag", "tagPtMax"});
  tag_rap_min = SetXML::Instance()->GetElementDouble({"jetTag", "tagRapMin"});
  tag_rap_max = SetXML::Instance()->GetElementDouble({"jetTag", "tagRapMax"});
  //------------------------------------------------------------------------------
  delta_phi_cut = SetXML::Instance()->GetElementInt({"jetTag", "deltaPhiCut"});
  //------------------------------------------------------------------------------
  int iso = SetXML::Instance()->GetElementInt({"jetTag", "isolation"}, false);
  //------------------------------------------------------------------------------
  if (n_tag_per_ev)
  {
    n_tag_ptr = std::unique_ptr<TagNumber>(new TagNumber(n_tag_per_ev));
  }
  else
  {
    n_tag_ptr = std::unique_ptr<TagInclusive>(new TagInclusive(n_tag_per_ev));
  }
  //------------------------------------------------------------------------------
  if (xJetTag)
  {
    x_tag_ptr = std::unique_ptr<TagXCut>(new TagXCut());
  }
  else
  {
    x_tag_ptr = std::unique_ptr<TagPtJetCut>(new TagPtJetCut());
  }
  //------------------------------------------------------------------------------
  tag_pid_ptr = std::unique_ptr<PIDSelected>(new PIDSelected(pid_tag));
  //------------------------------------------------------------------------------
  if (stat_tag.size())
  {
    tag_pstat_ptr = std::unique_ptr<PStatSelected>(new PStatSelected(stat_tag));
  }
  else
  {
    tag_pstat_ptr = std::unique_ptr<PStatSelected>(new PStatSelected({0, 1}));
  }
  //------------------------------------------------------------------------------
  if (tag_rapidity == 0)
  {
    tag_rap_ptr = std::unique_ptr<RapidityY>(new RapidityY());
  }
  else if (tag_rapidity == 1)
  {
    tag_rap_ptr = std::unique_ptr<PseudoRapidityEta>(new PseudoRapidityEta());
  }
  //------------------------------------------------------------------------------
  if (iso)
  {
    tag_iso_ptr = std::unique_ptr<Isolated>(new Isolated(sub_ptr));
  }
  else
  {
    tag_iso_ptr = std::unique_ptr<NoIsolation>(new NoIsolation());
  }
  tag_iso_ptr->Init("jetTag");
  tag_iso_ptr->SetRapidityCut(tag_rap_max);
  //------------------------------------------------------------------------------
  if (delta_phi_cut)
  {
    delta_phi_min = SetXML::Instance()->GetElementDouble({"jetTag", "deltaPhiMin"});
    delta_phi_min = DeltaPhiCutBase::PhiFormat(delta_phi_min);
  }
  else
  {
    delta_phi_min = 0.0;
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[  Tagged: JetTag  ] Bad Input (0) for deltaPhiCut" << std::endl;
    std::cout << "[  Tagged: JetTag  ] [Tag will be Triggered as a Jet]" << std::endl;
    std::cout << "[  Tagged: JetTag  ] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);    
  }
  //------------------------------------------------------------------------------
}

void Tagged::TagSort()
{
  // FourVector p(Px, Py, Pz, E);
  // return std::make_shared<Particle>(SN, PID, Status, Status, p);
  tag_list = sorted_by_pt(tag_list);
  tag_sorted = 1;
}

int Tagged::GetNAnalysis()
{
  // Sort
  TagSort();
  // Isolation Check
  int n_max = n_tag_ptr->GetNtag(tag_list.size());
  tag_list = tag_iso_ptr->IsolationPassed(tag_list, n_max);

  return tag_list.size();
}

double Tagged::GetPhi(int i)
{
  return tag_list[i]->phi();
}

double Tagged::GetPtTag(int i)
{
  return tag_list[i]->perp();
}

double Tagged::GetJetPtCut(double val_min)
{
  return x_tag_ptr->SetJetPtCut(val_min,tag_pt_min);
}

//===========================================================================================================================
// NoTag
//===========================================================================================================================
void NoTag::Init()
{
  x_tag_ptr = std::unique_ptr<TagPtJetCut>(new TagPtJetCut());
  initialized = 1;
}

void NoTag::PrintJetTagSetting() {}

//===========================================================================================================================
// TagNumber
//===========================================================================================================================
void TagNumber::ShowTagNumberSetting()
{
  std::cout << "[     TagNumber    ] *** Trigger " << n_max
            << "-tag(s) per an event at most" << std::endl;
}

void TagInclusive::ShowTagNumberSetting()
{
  std::cout << "[     TagNumber    ] *** Trigger tags inclusively per an event" << std::endl;
}

TagNumber::TagNumber(int n_max_in) : n_max(n_max_in)
{
  if (n_max_in == 0)
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[TagNumber] Bad Input (zero) for n_max." << std::endl;
    std::cout << "[TagNumber] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
}

TagInclusive::TagInclusive(int n_max_in)
{
  if (n_max_in != 0)
  {
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[InclusiveTag] Bad Input (non-zero) for n_max." << std::endl;
    std::cout << "[InclusiveTag] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
}

int TagNumber::GetNtag(int n_tag_list)
{
  if (n_tag_list <= n_max)
  {
    return n_tag_list;
  }
  else
  {
    return n_max;
  }
}

//===========================================================================================================================
// TagXCut
//===========================================================================================================================

std::vector<double>
TagXCut::SetJetPtCut(const std::vector<double> &x_jet_tag, const double pt_tag)
{
  std::vector<double> jet_pt(x_jet_tag.size());

  for (size_t i = 0; i < x_jet_tag.size(); i++)
  {
    jet_pt[i] = SetJetPtCut(x_jet_tag[i], pt_tag);
  }

  return jet_pt;
}

double TagXCut::SetJetPtCut(const double x_jet_tag, const double pt_tag)
{
  return x_jet_tag * pt_tag;
}
