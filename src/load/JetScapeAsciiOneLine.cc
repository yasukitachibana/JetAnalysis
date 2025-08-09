#include "JetScapeAsciiOneLine.h"

// Register the module with the base class
RegisterLoadFileModule<JetScapeAsciiOneLine> JetScapeAsciiOneLine::reg("JetScapeAsciiOneLine");

void JetScapeAsciiOneLine::TestStringStream()
{
  std::cout << "[JetScapeAsciiOneLine] No Test. Exit." << std::endl;
  exit(-1);
}

JetScapeAsciiOneLine::JetScapeAsciiOneLine(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating JetScapeAsciiOneLine" << std::endl;
  // TestStringStream();
}

JetScapeAsciiOneLine::~JetScapeAsciiOneLine()
{
  std::cout << "-$-Deleting JetScapeAsciiOneLine" << std::endl;
}

bool JetScapeAsciiOneLine::Load(std::string input_filename)
{

  //------------------------------------------
  // First, Clear reading_file
  Clear();

  //------------------------------------------
  // Then, Open input_filename
  std::cout << "[JetScapeAsciiOneLine] Set: " << input_filename << std::endl;
  reading_file.open(input_filename);

  //------------------------------------------
  // Check if the file, input_filename, is open
  // If not, display error message and return false
  if (reading_file.is_open())
  {
    std::cout << "[JetScapeAsciiOneLine] File Set. " << std::endl;
    return true;
  }
  else
  {
    Clear();
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[JetScapeAsciiOneLine] InputFile: " << input_filename << " is not found" << std::endl;
    std::cout << "[JetScapeAsciiOneLine] Skip. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    return false;
  }
}

void JetScapeAsciiOneLine::Clear()
{
  std::cout << "[JetScapeAsciiOneLine] Clear." << std::endl;

  //----------------------------------------
   std::cout << "[JetScapeAsciiOneLine] Before Clear." <<std::endl;
   std::cout << "input_line: " << input_line <<std::endl;
  //----------------------------------------

  reading_file.close();

  // Clear input_line
  input_line.clear();
  input_line.shrink_to_fit();
  getLineStart = 0;

  //----------------------------------------
   std::cout << "[JetScapeAsciiOneLine] After Clear." <<std::endl;
   std::cout << "input_line: " << input_line <<std::endl;
  //----------------------------------------
}

void JetScapeAsciiOneLine::Reset()
{
  std::cout << "[JetScapeAsciiOneLine] Reset." << std::endl;
  //----------------------------------------
   std::cout << "[JetScapeAsciiOneLine] Before Reset." <<std::endl;
   std::cout << "input_line: " << input_line <<std::endl;

  //----------------------------------------
  // Go back to the first line
  reading_file.clear();
  reading_file.seekg(0, std::ios::beg);

  // Clear input_line
  input_line.clear();
  input_line.shrink_to_fit();
  getLineStart = 0;

  //----------------------------------------
   std::cout << "[JetScapeAsciiOneLine] After Reset." <<std::endl;
   std::cout << "input_line: " << input_line <<std::endl;
  //----------------------------------------
}

int JetScapeAsciiOneLine::GetLine()
{
  if (getline(reading_file, input_line))
  {
    // std::cout << ">>" << input_line <<std::endl;
    return 1;
  }
  else
  {
    // std::cout << "last?" << input_line <<std::endl;
    return 0;
  }
}

void JetScapeAsciiOneLine::ShowLine()
{
  std::cout << "[JetScapeAsciiOneLine] Current Line: " << input_line << std::endl;
}

int JetScapeAsciiOneLine::EventEnd()
{
  if (input_line.find("#") != std::string::npos)
  {

    if (getLineStart == 1)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    getLineStart = 1;
    return 0;
  }
}

int JetScapeAsciiOneLine::ValidLine()
{
  if (getLineStart == 1 && !input_line.empty())
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

std::shared_ptr<Particle> JetScapeAsciiOneLine::GetParticle()
{
  sscanf(input_line.data(),
         "%d %d %d %lf %lf %lf %lf %lf %lf",
         &SN, &PID, &Status, &E, &Px, &Py, &Pz, &Eta, &Phi);
  FourVector p(Px, Py, Pz, E);

  return std::make_shared<Particle>(SN, PID, Status, Status, p);
}

void JetScapeAsciiOneLine::LoadSigma(std::string sigma_filename, double &sigma, double &sigma_err)
{
  // Read file
  std::ifstream ifs;
  std::stringstream str_stream;

  ifs.open(sigma_filename.c_str()); // open the input file
  str_stream << ifs.rdbuf();        // read the file
  if (ifs.is_open())
  {
    ifs.close();
  }
  else
  {
    ifs.close();
    std::cout << "[JetScapeAsciiOneLine] " << sigma_filename << " NOT FOUND" << std::endl;
    return;
  }

  std::string str;

  while (getline(str_stream, str))
  {

    if (str.find("#") == std::string::npos && !str.empty())
    {

      sscanf(str.data(),
             "%lf %lf",
             &sigma, &sigma_err);
      break;
    }
  }
}
