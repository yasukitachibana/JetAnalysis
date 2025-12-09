#ifndef JETSCAPEASCIIONELINE_H
#define JETSCAPEASCIIONELINE_H

#include "LoadFileModuleBase.h"

class JetScapeAsciiV2OneLine : public LoadFileModuleBase
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterLoadFileModule<JetScapeAsciiV2OneLine> reg;

public:
  JetScapeAsciiV2OneLine(std::string name_in = "JetScapeAsciiV2OneLine");
  ~JetScapeAsciiV2OneLine();
  void Init();

  std::string Name() { return name; }

  bool Load(std::string input_filename);
  void Clear();

  int GetLine();
  void ShowLine();
  int EventEnd();
  int ValidLine();
  int Last();
  void LoadSigma(std::string sigma_filename, double &sigma, double &sigma_err);  

  std::shared_ptr<Particle> GetParticle();  

private:
  const std::string name;

  std::ifstream reading_file;
  std::string input_line;
  int getLineStart;

  int SN, PID, Status;
  double E, Px, Py, Pz;


  //===========================
  void ReadParametersFromXML();
  void ShowParamsSetting();
  void LoadSeparatedSigma(std::string sigma_filename, double &sigma, double &sigma_err);
  void LoadSigmaLastLine(std::string sigma_filename, double &sigma, double &sigma_err);
  //===========================
  int sigmaLastLine;


};

#endif // JetScapeAsciiV2OneLine_H