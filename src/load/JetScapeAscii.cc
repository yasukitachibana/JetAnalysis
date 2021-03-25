#include "JetScapeAscii.h"


// Register the module with the base class
RegisterLoadFileModule<JetScapeAscii> JetScapeAscii::reg("JetScapeAscii");

JetScapeAscii::JetScapeAscii(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating JetScapeAscii" << std::endl;
}

JetScapeAscii::~JetScapeAscii(){
  std::cout << "-$-Deleting JetScapeAscii"<<std::endl;
}
