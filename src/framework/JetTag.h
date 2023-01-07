#ifndef JETTAG_H_
#define JETTAG_H_

// #include "fastjet/PseudoJet.hh"
// #include "Pythia8/Pythia.h"

#include "Particle.h"
#include "Rapidity.h"
#include "PIdentify.h"
#include "PStat.h"
#include "SubtractionModuleBase.h"
#include "Isolation.h"
#include "DeltaPhiCut.h"
// // #include <vector>
// // #include <memory>
// #include <iostream>
// #include <sstream>
// #include <iomanip>

//===========================================================================================================================
// TagXcut Classes
//===========================================================================================================================
class TagXCutBase
{
public:
  virtual ~TagXCutBase() {}
  virtual std::string Val() { return ""; }
  virtual std::string UnitVal() { return ""; }

  virtual std::vector<double>
  SetJetPtCut(const std::vector<double> &jet_pt, const double pt_tag = 0.0)
  {
    return jet_pt;
  }

  virtual double SetJetPtCut(const double jet_pt, const double pt_tag = 0.0)
  {
    return jet_pt;
  }
};

class TagXCut : public TagXCutBase
{
public:
  TagXCut() {}
  ~TagXCut() {}
  std::string Val() { return "X_jet-tag"; }
  std::string UnitVal() { return ""; }

  std::vector<double>
  SetJetPtCut(const std::vector<double> &x_jet_tag, const double pt_tag);
  double SetJetPtCut(const double x_jet_tag, const double pt_tag);
};

class TagPtJetCut : public TagXCutBase
{
public:
  TagPtJetCut() {}
  ~TagPtJetCut() {}
  std::string Val() { return "pt_jet"; }
  std::string UnitVal() { return "GeV"; }

  std::vector<double>
  SetJetPtCut(const std::vector<double> &jet_pt, const double pt_tag = 0.0)
  {
    return jet_pt;
  }

  double SetJetPtCut(const double jet_pt, const double pt_tag = 0.0)
  {
    return jet_pt;
  }
};

//===========================================================================================================================
// TagNumber Classes
//===========================================================================================================================

class TagNumberBase
{
public:
  virtual ~TagNumberBase() {}
  virtual int GetNtag(int n_tag_list) { return 0; }
  virtual void ShowTagNumberSetting() {}
};

class TagNumber : public TagNumberBase
{
public:
  ~TagNumber() {}
  TagNumber(int n_max_in);
  int GetNtag(int n_tag_list);
  void ShowTagNumberSetting();

private:
  const int n_max;
};

class TagInclusive : public TagNumberBase
{
public:
  ~TagInclusive() {}
  TagInclusive() {}
  TagInclusive(int n_max_in);
  int GetNtag(int n_tag_list) { return n_tag_list; }
  void ShowTagNumberSetting();
};

//===========================================================================================================================
// JetTagBase
//===========================================================================================================================
class JetTagBase
{

public:
  virtual ~JetTagBase() {}
  virtual void Init() {}
  virtual void TagEventClear() {}
  virtual void Feed(std::shared_ptr<Particle> p) {}
  virtual bool DeltaPhiCut() { return false; }
  virtual double DeltaPhiMin() { return 0.0; };
  void ShowJetTagSetting();
  bool Initialized() { return initialized; }
  virtual void TagSort() {}
  virtual int GetNAnalysis() { return 0; }
  virtual double GetPhi(int i) { return 0.0; }
  virtual double GetPtTag(int i) { return 0.0; }
  std::string ValJetCut();
  std::string UnitValJetCut();

  std::vector<double> 
  JetPtForTrigger(const std::vector<double> &jet_pt, const double pt_tag = 0.0);

protected:
  int initialized = 0;
  std::unique_ptr<TagXCutBase> x_tag_ptr;
  //   virtual void ReadParametersFromXML() {}
  //   virtual void PrintJetTagSetting() {}

private:
  virtual void PrintJetTagSetting() {}
  //   static Pythia8::Pythia InternalHelperPythia;

}; // END BASE CLASS

//===========================================================================================================================
// (Simple) JetTag Class
//===========================================================================================================================
class Tagged : public JetTagBase
{

public:
  Tagged(std::shared_ptr<SubtractionModuleBase> sub_ptr_in);
  ~Tagged() {}
  void Init();
  void TagEventClear();
  void Feed(std::shared_ptr<Particle> p);
  bool DeltaPhiCut() { return delta_phi_cut; };
  double DeltaPhiMin() { return delta_phi_min; };
  void TagSort();
  int GetNAnalysis();
  double GetPhi(int i);
  double GetPtTag(int i);

private:
  //-----------------------------------------
  double tag_pt_min, tag_pt_max;
  double tag_rap_min, tag_rap_max;
  //-----------------------------------------
  int delta_phi_cut;
  double delta_phi_min;
  //-----------------------------------------
  std::unique_ptr<TagNumberBase> n_tag_ptr;
  std::unique_ptr<PIdentifyBase> tag_pid_ptr;
  std::unique_ptr<PStatBase> tag_pstat_ptr;
  std::unique_ptr<RapidityBase> tag_rap_ptr;
  std::unique_ptr<IsolationBase> tag_iso_ptr;
  //-----------------------------------------
  std::vector<std::shared_ptr<Particle>> tag_list;
  //-----------------------------------------
  void PrintJetTagSetting();
  void ReadParametersFromXML();
  //-----------------------------------------
  bool TagTrigger(std::shared_ptr<Particle> p);
  //-----------------------------------------
  std::shared_ptr<SubtractionModuleBase> sub_ptr;
  //-----------------------------------------
  int tag_sorted;
};

class NoTag : public JetTagBase
{
public:
  NoTag() {}
  ~NoTag() {}
  void Init();
  void TagEventClear() {}
  void Feed(std::shared_ptr<Particle> p) {}
  bool DeltaPhiCut() { return false; }
  double DeltaPhiMin() { return 0.0; };
  void TagSort() {}
  int GetNAnalysis() { return 1; }
  double GetPhi(int i) { return 0.0; }
  double GetPtTag(int i) { return 0.0; }

private:
  void PrintJetTagSetting();
};
//===========================================================================================================================

#endif
