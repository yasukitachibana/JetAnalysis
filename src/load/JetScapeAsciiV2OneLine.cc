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

void JetScapeAsciiV2OneLine::LoadSigma(std::string sigma_filename, double &sigma, double &sigma_err)
{
  if (sigmaLastLine == 1)
  {
    LoadSigmaLastLine(sigma_filename, sigma, sigma_err);
  }
  else
  {
    LoadSeparatedSigma(sigma_filename, sigma, sigma_err);
  }
}

void JetScapeAsciiV2OneLine::LoadSigmaLastLine(std::string sigma_filename, double &sigma, double &sigma_err)
{

  std::ifstream ifs(sigma_filename, std::ios::binary);
  if (!ifs)
  {
    std::cerr << "[JetScapeAsciiV2OneLine] Cannot open file: " << sigma_filename << std::endl;
    return;
  }

  // Move to the end of file
  ifs.seekg(0, std::ios::end);
  std::streamoff pos = static_cast<std::streamoff>(ifs.tellg());
  if (pos <= 0)
  {
    std::cerr << "[JetScapeAsciiV2OneLine] File is empty: " << sigma_filename << std::endl;
    return;
  }

  char c;

  // --- Step 1: Skip trailing whitespace/newlines at the end of the file ---
  while (pos > 0)
  {
    pos -= 1; // move back by 1 byte
    ifs.seekg(pos, std::ios::beg);
    ifs.get(c);

    // Stop when we find a non-whitespace character
    if (!std::isspace(static_cast<unsigned char>(c)))
    {
      break;
    }
  }

  // If pos == 0 and that character is whitespace, there is no valid line
  if (pos <= 0 && std::isspace(static_cast<unsigned char>(c)))
  {
    std::cerr << "[JetScapeAsciiV2OneLine] No non-empty line found in: " << sigma_filename << std::endl;
    return;
  }

  // --- Step 2: Move backwards to find the previous newline to locate the line start ---
  while (pos > 0)
  {
    pos -= 1;
    ifs.seekg(pos, std::ios::beg);
    ifs.get(c);
    if (c == '\n')
    {
      pos += 1; // the line begins right after this newline
      break;
    }
  }

  // --- Step 3: Read exactly one line starting from the detected position ---
  ifs.clear();
  ifs.seekg(pos, std::ios::beg);

  std::string line;
  std::getline(ifs, line);

  std::istringstream iss(line);
  std::string hash, label1, label2;

  std::cout << "[JetScapeAsciiV2OneLine] Sigma line: " << line << std::endl;

  iss >> hash >> label1 >> sigma >> label2 >> sigma_err; 

  std::cout << "[JetScapeAsciiV2OneLine] sigmaGen = " << sigma << std::endl;
  std::cout << "[JetScapeAsciiV2OneLine] sigmaErr = " << sigma_err << std::endl;

}

void JetScapeAsciiV2OneLine::LoadSeparatedSigma(std::string sigma_filename, double &sigma, double &sigma_err)
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
    std::cout << "[JetScapeAsciiV2OneLine] " << sigma_filename << " NOT FOUND" << std::endl;
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