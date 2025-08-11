#ifndef JETSCAPEASCII_H
#define JETSCAPEASCII_H

#include "LoadFileModuleBase.h"



class JetScapeAscii : public LoadFileModuleBase {
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterLoadFileModule<JetScapeAscii> reg;
  
public:
  JetScapeAscii(std::string name_in = "JetScapeAscii");
  ~JetScapeAscii();
  std::string Name(){return name;}
  
  bool Load(std::string input_filename);
  void Clear();//Clear the buffer
  void Reset();//Go back to the first line
  
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
  
  std::stringstream input_str_stream; //Full file data
  std::string input_line; 
  int getLineStart;
  
  int SN, PID;
  double Px, Py, Pz, E, Eta, Phi;
  int Status;


  
};

#endif // JetScapeAscii_H
