#ifndef JETSCAPEASCIIONELINE_H
#define JETSCAPEASCIIONELINE_H

#include "LoadFileModuleBase.h"



class JetScapeAsciiOneLine : public LoadFileModuleBase {
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterLoadFileModule<JetScapeAsciiOneLine> reg;
  
public:
  JetScapeAsciiOneLine(std::string name_in = "JetScapeAsciiOneLine");
  ~JetScapeAsciiOneLine();
  std::string Name(){return name;}
  
  bool Load(std::string input_filename);
  void Clear();
  void Reset();
  
  int GetLine();
  void ShowLine();
  int EventEnd();
  int ValidLine();
  int Last(){return 1;}
  void LoadSigma(std::string sigma_filename, double &sigma, double &sigma_err);

  std::shared_ptr<Particle> GetParticle();

private:
  
  void TestStringStream();
  
  const std::string name;
  

  std::ifstream reading_file;
  std::string input_line;
  int getLineStart;
  
  int SN, PID;
  double Px, Py, Pz, E, Eta, Phi;
  int Status;


  
};

#endif // JetScapeAsciiOneLine_H
