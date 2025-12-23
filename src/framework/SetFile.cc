#include "SetFile.h"
#include "SetXML.h"
// #include "JetScapeLogger.h"
// #include <stdlib.h>

#include <sstream>
#include <iomanip>
#include <cmath> // std::round, std::abs

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
  if (division_number == 0)
  {
    division_unit = "";
  }
  else
  {
  division_unit = SetXML::Instance()->GetElementText({"inputFiles", "divUnit"});    
  }

  xjtag = SetXML::Instance()->GetElementInt({"jetTag", "xJetTag"}, false);
  if (xjtag)
  {
    jet_cut_name = "xjtag";
    jet_cut_precision = 2;
  }
  else
  {
    jet_cut_name = "ptj";
    jet_cut_precision = 0;
  }

  single_number_id = SetXML::Instance()->GetElementInt({"inputFiles", "singleNumberId"}, false);

  input_head = SetXML::Instance()->GetElementText({"inputFiles", "events", "head"});
  input_tail = SetXML::Instance()->GetElementText({"inputFiles", "events", "tail"});

  if (single_number_id == 1)
  {
    input_join = "";
  }
  else
  {
    input_join = SetXML::Instance()->GetElementText({"inputFiles", "events", "join"});
  }

  sigma_last_line = SetXML::Instance()->GetElementInt({"inputFiles", "sigmaLastLine"}, false);

  if (sigma_last_line == 0)
  {
    sigma_head = SetXML::Instance()->GetElementText({"inputFiles", "sigma", "head"});
    sigma_join = SetXML::Instance()->GetElementText({"inputFiles", "sigma", "join"});
    sigma_tail = SetXML::Instance()->GetElementText({"inputFiles", "sigma", "tail"});

    sigma_dir = SetXML::Instance()->GetElementText({"inputFiles", "sigma", "path"});
    if (sigma_dir == "na")
    {
      sigma_dir = in_dir;
    }
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

  std::string upto_id_part = in_dir + '/' + input_head + std::to_string(int(ptHatMin));

  if (single_number_id != 1)
  {
    upto_id_part = upto_id_part + input_join + std::to_string(int(ptHatMax));
  }

  return upto_id_part + GetDivTail(i_seq) + input_tail;
}

std::string
SetFile::GetSigmaFileName(double ptHatMin, double ptHatMax, int i_seq)
{
  // return sigma_dir + '/' + sigma_head + std::to_string(int(ptHatMin)) + sigma_join + std::to_string(int(ptHatMax)) + GetDivTail(i_seq) + sigma_tail;

  if (sigma_last_line == 1)
  {
    return GetInputFileName(ptHatMin, ptHatMax, i_seq);
  }
  else
  {
    std::string upto_id_part = sigma_dir + '/' + sigma_head + std::to_string(int(ptHatMin));

    if (single_number_id != 1)
    {
      upto_id_part = upto_id_part + sigma_join + std::to_string(int(ptHatMax));
    }
    return upto_id_part + sigma_tail;
  }
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

  // int r_precision = 1;
  // if (jetR < 0.1)
  // {
  //   r_precision = 3;
  //   if (jetR < 0.01)
  //   {
  //     r_precision = 4;
  //   }
  // }

  const double eps = 1e-9;

  int r_precision = 4;
  // Detect Precision of Cone Size
  double v2 = jetR * 100.0;
  if (std::abs(v2 - std::round(v2)) < eps)
  {
    r_precision = 3;
  }
  double v1 = jetR * 10.0;
  if (std::abs(v1 - std::round(v1)) < eps)
  {
    r_precision = 1;
  }

  int particle_pt_precision = 4;
  double w2 = particlePtMin * 100.0;
  if (std::abs(w2 - std::round(w2)) < eps)
  {
    particle_pt_precision = 3;
  }
  double w1 = particlePtMin * 10.0;
  if (std::abs(w1 - std::round(w1)) < eps)
  {
    particle_pt_precision = 1;
  }

  oss << std::fixed
      << "hist_"
      << "ptHat" << std::setprecision(0) << (ptHatMin) << "-" << (ptHatMax) << "_"
      << observable << "_"
      << variable << "_"
      << "jetr" << std::setprecision(r_precision) << (jetR) << "_"
      << jet_cut_name << std::setprecision(jet_cut_precision) << (jetPtMin) << "-" << (jetPtMax) << "_"
      << "rapj" << std::setprecision(1) << (jetRapMin) << "-" << (jetRapMax) << "_"
      << "pt" << std::setprecision(particle_pt_precision) << (particlePtMin) << "-" << (particlePtMax) << "_"
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

  // int r_precision = 1;
  // if (jetR < 0.1)
  // {
  //   r_precision = 3;
  //   if (jetR < 0.01)
  //   {
  //     r_precision = 4;
  //   }
  // }
  const double eps = 1e-9;
  int r_precision = 4;
  // Detect Precision of Cone Size
  double v2 = jetR * 100.0;
  if (std::abs(v2 - std::round(v2)) < eps)
  {
    r_precision = 3;
  }
  double v1 = jetR * 10.0;
  if (std::abs(v1 - std::round(v1)) < eps)
  {
    r_precision = 1;
  }

  int particle_pt_precision = 4;
  double w2 = particlePtMin * 100.0;
  if (std::abs(w2 - std::round(w2)) < eps)
  {
    particle_pt_precision = 3;
  }
  double w1 = particlePtMin * 10.0;
  if (std::abs(w1 - std::round(w1)) < eps)
  {
    particle_pt_precision = 1;
  }

  oss << std::fixed
      << "hist_total_"
      << observable << "_"
      << variable << "_"
      << "jetr" << std::setprecision(r_precision) << (jetR) << "_"
      << jet_cut_name << std::setprecision(jet_cut_precision) << (jetPtMin) << "-" << (jetPtMax) << "_"
      << "rapj" << std::setprecision(1) << (jetRapMin) << "-" << (jetRapMax) << "_"
      << "pt" << std::setprecision(particle_pt_precision) << (particlePtMin) << "-" << (particlePtMax) << "_"
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
