#ifndef JETSCAPEASCII_H
#define JETSCAPEASCII_H

#include "LoadFileModuleBase.h"
#include <iostream>

class JetScapeAscii : public LoadFileModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterLoadFileModule<JetScapeAscii> reg;
  const std::string name;
  
public:
  JetScapeAscii(std::string name_in = "JetScapeAscii");
  ~JetScapeAscii();
  std::string Name(){return name;}

  
};

#endif // JetScapeAscii_H
