#ifndef ISOLATION_H_
#define ISOLATION_H_

#include "Particle.h"
#include "Rapidity.h"
#include "PStat.h"
#include "Charged.h"
#include "SubtractionModuleBase.h"
// #include <vector>
// #include <memory>
// #include <iostream>
// #include <sstream>
// #include <iomanip>

//===========================================================================================================================
// BASE
//===========================================================================================================================
class IsolationBase
{

public:
  virtual ~IsolationBase() {}
  virtual void IsoEventClear() {}
  virtual void Init() {}
  virtual void Init(std::string xml_tag_name) {}
  virtual void Feed(std::shared_ptr<Particle> p) {}
  virtual void SetRapidityCut(double eta_max_in) {}
  void ShowIsolationSetting();
  //-----------------------------------------------------------------------
  virtual std::vector<std::shared_ptr<Particle>>
  IsolationPassed(std::vector<std::shared_ptr<Particle>> tag_list, int n_max = -1)
  {
    return tag_list;
  }
  //-----------------------------------------------------------------------  

protected:
  int initialized = 0;

private:
  virtual void PrintIsolationSetting() {}

}; // END BASE CLASS

//===========================================================================================================================
// (Simple) Isolation Class
//===========================================================================================================================
class Isolated : public IsolationBase
{

public:
  Isolated(std::shared_ptr<SubtractionModuleBase> sub_ptr_in);
  Isolated();
  ~Isolated() {}
  void SetSubstruction(std::shared_ptr<SubtractionModuleBase> sub_ptr_in);
  void SetRapidityCut(double eta_max_tag);
  void IsoEventClear();
  void Init(std::string xml_tag_name);
  void Feed(std::shared_ptr<Particle> p);
  std::vector<std::shared_ptr<Particle>>
  IsolationPassed(std::vector<std::shared_ptr<Particle>> tag_list, int n_max = -1);


private:
  void PrintIsolationSetting();
  void ReadParametersFromXML(std::string xml_tag_name);
  std::shared_ptr<SubtractionModuleBase> sub_ptr;
  std::vector<std::shared_ptr<Particle>> iso_check_list;
  //----------------------------------------------------
  double rIsolation;
  double ptIsolation;
  //-----------------------------------------
  double iso_pt_min, iso_pt_max;
  double iso_rap_min, iso_rap_max;
  //-----------------------------------------
  std::unique_ptr<ChargedBase> iso_charged_ptr;
  std::unique_ptr<PStatBase> iso_pstat_ptr;
  std::unique_ptr<RapidityBase> iso_rap_ptr;
  //-----------------------------------------
  bool IsoTrigger(std::shared_ptr<Particle> p);
  bool IsolationCheck(std::shared_ptr<Particle> tag);
  //-----------------------------------------
};

class NoIsolation : public IsolationBase
{
public:
  NoIsolation() {}
  ~NoIsolation() {}
  void IsoEventClear() {}
  void Init();
  void Init(std::string xml_tag_name);
  void Feed(std::shared_ptr<Particle> p) {}
  void SetRapidityCut(double eta_max_tag) {}
  //-----------------------------------------------------------------------
  std::vector<std::shared_ptr<Particle>>
  IsolationPassed(std::vector<std::shared_ptr<Particle>> tag_list, int n_max = -1);
  //-----------------------------------------------------------------------  


private:
  void PrintIsolationSetting();
};
//===========================================================================================================================

#endif
