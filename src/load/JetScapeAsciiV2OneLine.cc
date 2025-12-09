#include "JetScapeAsciiV2OneLine.h"

// Register the module with the base class
RegisterLoadFileModule<JetScapeAsciiV2OneLine> JetScapeAsciiV2OneLine::reg("JetScapeAsciiV2OneLine");

JetScapeAsciiV2OneLine::JetScapeAsciiV2OneLine(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating JetScapeAsciiV2OneLine" << std::endl;
  // TestStringStream();
}

JetScapeAsciiV2OneLine::~JetScapeAsciiV2OneLine()
{
  std::cout << "-$-Deleting JetScapeAsciiV2OneLine" << std::endl;
}

void JetScapeAsciiV2OneLine::Init()
{
  ReadParametersFromXML();
  ShowParamsSetting();
  // std::cout << "[JetScapeAsciiV2OneLine] Init." << std::endl;
  // exit(1);
}

void JetScapeAsciiV2OneLine::ReadParametersFromXML()
{

  sigmaLastLine = SetXML::Instance()->GetElementInt({"inputFiles", "sigmaLastLine"}, false);
}

void JetScapeAsciiV2OneLine::ShowParamsSetting()
{
  std::cout << "[JetScapeAsciiV2OneLine] ***-------------------------------------------" << std::endl;
  std::cout << "[JetScapeAsciiV2OneLine] *** [JetScapeAsciiV2OneLine]" << std::endl;
  std::cout << "[JetScapeAsciiV2OneLine] *** "
            << " -The last line for sigma : "
            << ((sigmaLastLine == 1) ? "Yes" : "No") // The parentheses are required.
            << std::endl;
  std::cout << "[JetScapeAsciiV2OneLine] ***-------------------------------------------" << std::endl;
}

bool JetScapeAsciiV2OneLine::Load(std::string input_filename)
{

  //------------------------------------------
  // First, Clear reading_file
  Clear();

  //------------------------------------------
  // Then, Open input_filename
  std::cout << "[JetScapeAsciiV2OneLine] Set: " << input_filename << std::endl;
  reading_file.open(input_filename);

  //------------------------------------------

  // Check if the file, input_filename, is open
  // If not, display error message and return false
  if (reading_file.is_open())
  {
    std::cout << "[JetScapeAsciiV2OneLine] File Set. " << std::endl;
    return true;
  }
  else
  {
    Clear();
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[JetScapeAsciiV2OneLine] InputFile: " << input_filename << " is not found" << std::endl;
    std::cout << "[JetScapeAsciiV2OneLine] Skip. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    return false;
  }
}

void JetScapeAsciiV2OneLine::Clear()
{
  std::cout << "[JetScapeAsciiV2OneLine] Clear." << std::endl;

  //----------------------------------------
  std::cout << "[JetScapeAsciiV2OneLine] Before Clear." << std::endl;
  std::cout << "input_line: " << input_line << std::endl;
  //----------------------------------------

  reading_file.close();

  // Clear input_line
  input_line.clear();
  input_line.shrink_to_fit();
  getLineStart = 0;

  //----------------------------------------
  std::cout << "[JetScapeAsciiV2OneLine] After Clear." << std::endl;
  std::cout << "input_line: " << input_line << std::endl;
  //----------------------------------------
}

int JetScapeAsciiV2OneLine::GetLine()
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

void JetScapeAsciiV2OneLine::ShowLine()
{
  std::cout << "[JetScapeAsciiV2OneLine] Current Line: " << input_line << std::endl;
}

int JetScapeAsciiV2OneLine::EventEnd()
{
  // Event End Line: Line with '#'
  if (input_line.find("#") != std::string::npos)
  {
    if (getLineStart == 1)
    {
      return 1;
    }
    else
    {
      // When the first Event End Line is not found yet
      return 0;
    }
  }
  else
  {
    getLineStart = 1;
    return 0;
  }
}

int JetScapeAsciiV2OneLine::ValidLine()
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

std::shared_ptr<Particle> JetScapeAsciiV2OneLine::GetParticle()
{
  sscanf(input_line.data(),
         "%d %d %d %lf %lf %lf %lf",
         &SN, &PID, &Status, &E, &Px, &Py, &Pz);
  FourVector p(Px, Py, Pz, E);

  return std::make_shared<Particle>(SN, PID, Status, Status, p);
}

int JetScapeAsciiV2OneLine::Last()
{
  if (sigmaLastLine == 1)
  {
    // std::cout
    //     << "[JetScapeAsciiV2OneLine] "
    //        " -The last line is for sigma."
    //     << std::endl;
    return 0;
  }
  else
  {
    return 1;
  }
}