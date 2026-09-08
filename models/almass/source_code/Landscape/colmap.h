#include <string>
#include <array>
#include <map>

namespace almasscmap
{
  using rgbarray = std::array<unsigned char,3>;
  using colourmap = std::map<std::string, rgbarray>;
  colourmap makeColourMap();
}
 
