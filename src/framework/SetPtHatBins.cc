#include "SetPtHatBins.h"
#include "SetXML.h"
#include "VectorUtil.h"

#include <iostream>

namespace SetPtHatBins
{

  std::vector<double> GetPtHatBins()
  {
    std::vector<double> ptHat;

    int ptHatEvenlySpaced = SetXML::Instance()->GetElementInt({"ptHat", "evenlySpaced"}, false);

    if (ptHatEvenlySpaced == 1)
    {
      std::cout << "[EventAnalysiss] Setting ptHat bins evenly spaced." << std::endl;
      double ptHatMin = SetXML::Instance()->GetElementDouble({"ptHat", "min"});
      double ptHatMax = SetXML::Instance()->GetElementDouble({"ptHat", "max"});
      double ptHatInterval = SetXML::Instance()->GetElementDouble({"ptHat", "interval"});
      std::cout << "[EventAnalysiss] ptHat Min: " << ptHatMin << " GeV" << std::endl;
      std::cout << "[EventAnalysiss] ptHat Max: " << ptHatMax << " GeV" << std::endl;
      std::cout << "[EventAnalysiss] ptHat Interval: " << ptHatInterval << " GeV" << std::endl;
      ptHat = VectorUtil::VectorArange(ptHatMin, ptHatMax + 0.5 * ptHatInterval, ptHatInterval);
    }
    else
    {
      ptHat = SetXML::Instance()->GetElementVectorDouble({"ptHat", "Item"});
    }

    return ptHat;
  }

}
