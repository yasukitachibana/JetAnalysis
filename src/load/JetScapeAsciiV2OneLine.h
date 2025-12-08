#ifndef JETSCAPEASCIIONELINE_H
#define JETSCAPEASCIIONELINE_H

#include "LoadFileModuleBase.h"

class JetScapeAsciiV2OneLine : public LoadFileModuleBase
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterLoadFileModule<JetScapeAsciiV2OneLine> reg;

public:
private:
};

#endif // JetScapeAsciiV2OneLine_H