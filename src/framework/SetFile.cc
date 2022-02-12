#include "SetFile.h"
#include "SetXML.h"
//#include "JetScapeLogger.h"
//#include <stdlib.h>

#include <sstream>
#include <iomanip>

SetFile *SetFile::m_pInstance = NULL;

SetFile *SetFile::Instance()
{
  if (!m_pInstance)
  {
    std::cout << "[SetFile] Created SetFile Instance" << std::endl;
    m_pInstance = new SetFile();
  }
  return m_pInstance;
}

void SetFile::Init(std::string m_in_name, std::string m_out_name)
{

  SetDir(m_in_name, m_out_name);

  division_number = SetXML::Instance()->GetElementInt({"inputFiles", "divNum"});
  if (division_number > 0)
  {
    division_unit = SetXML::Instance()->GetElementText({"inputFiles", "divUnit"});
  }
  else
  {
    division_unit = "";
  }

  input_head = SetXML::Instance()->GetElementText({"inputFiles", "events", "head"});
  input_join = SetXML::Instance()->GetElementText({"inputFiles", "events", "join"});
  input_tail = SetXML::Instance()->GetElementText({"inputFiles", "events", "tail"});

  sigma_head = SetXML::Instance()->GetElementText({"inputFiles", "sigma", "head"});
  sigma_join = SetXML::Instance()->GetElementText({"inputFiles", "sigma", "join"});
  sigma_tail = SetXML::Instance()->GetElementText({"inputFiles", "sigma", "tail"});

  sigma_dir = SetXML::Instance()->GetElementText({"inputFiles", "sigma", "path"});
  if (sigma_dir == "na")
  {
    sigma_dir = in_dir;
  }
}

void SetFile::SetDir(std::string m_in_name, std::string m_out_name)
{
  SetInputDir(m_in_name);
  SetOutputDir(m_out_name);
}

std::string
SetFile::GetInputFileName(double ptHatMin, double ptHatMax, int i_seq)
{
  return in_dir + '/' + input_head + std::to_string(int(ptHatMin)) + input_join + std::to_string(int(ptHatMax)) + GetDivTail(i_seq) + input_tail;
}

std::string
SetFile::GetSigmaFileName(double ptHatMin, double ptHatMax, int i_seq)
{
  //return sigma_dir + '/' + sigma_head + std::to_string(int(ptHatMin)) + sigma_join + std::to_string(int(ptHatMax)) + GetDivTail(i_seq) + sigma_tail;
  return sigma_dir + '/' + sigma_head + std::to_string(int(ptHatMin)) + sigma_join + std::to_string(int(ptHatMax)) + sigma_tail;  
}

std::string SetFile::GetDivTail(int i_seq)
{
  if (division_number == 0)
  {
    return "";
  }
  else
  {
    return division_unit + std::to_string(i_seq);
  }
}

std::string
SetFile::GetHistName(double ptHatMin, double ptHatMax,
                     std::string observable,
                     std::string variable,
                     double jetR,
                     double jetPtMin, double jetPtMax,
                     double jetRapMin, double jetRapMax,
                     double particlePtMin, double particlePtMax,
                     double particleRapMin, double particleRapMax,
                     std::string tag /* = "" */)
{

  std::ostringstream oss;

  int r_precision = 1;
  if (jetR < 0.1)
  {
    r_precision = 3;
    if (jetR < 0.01)
    {
      r_precision = 4;
    }
  }
  oss << std::fixed
      << "hist_"
      << "ptHat" << std::setprecision(0) << (ptHatMin) << "-" << (ptHatMax) << "_"
      << observable << "_"
      << variable << "_"
      << "jetr" << std::setprecision(r_precision) << (jetR) << "_"
      << "ptj" << std::setprecision(0) << (jetPtMin) << "-" << (jetPtMax) << "_"
      << "rapj" << std::setprecision(1) << (jetRapMin) << "-" << (jetRapMax) << "_"
      << "pt" << std::setprecision(1) << (particlePtMin) << "-" << (particlePtMax) << "_"
      << "rap" << std::setprecision(1) << (particleRapMin) << "-" << (particleRapMax);

  if (tag != "")
  {
    oss << "_" << tag;
  }

  return oss.str();
}

std::string
SetFile::GetHistName(std::string observable,
                     std::string variable,
                     double jetR,
                     double jetPtMin, double jetPtMax,
                     double jetRapMin, double jetRapMax,
                     double particlePtMin, double particlePtMax,
                     double particleRapMin, double particleRapMax,
                     std::string tag /* = "" */)
{

  std::ostringstream oss;

  oss << std::fixed
      << "hist_total_"
      << observable << "_"
      << variable << "_"
      << "jetr" << std::setprecision(1) << (jetR) << "_"
      << "ptj" << std::setprecision(0) << (jetPtMin) << "-" << (jetPtMax) << "_"
      << "rapj" << std::setprecision(1) << (jetRapMin) << "-" << (jetRapMax) << "_"
      << "pt" << std::setprecision(1) << (particlePtMin) << "-" << (particlePtMax) << "_"
      << "rap" << std::setprecision(1) << (particleRapMin) << "-" << (particleRapMax);

  if (tag != "")
  {
    oss << "_" << tag;
  }

  return oss.str();
}

std::string SetFile::GetOutPath(std::string name)
{
  return out_dir + '/' + name;
}

std::string SetFile::GetInPath(std::string name)
{
  return in_dir + '/' + name;
}

std::string SetFile::GetOutPath()
{
  return out_dir;
}

std::string SetFile::GetInPath()
{
  return in_dir;
}
