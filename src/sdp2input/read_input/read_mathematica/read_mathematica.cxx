#include "../Input_Parser.hxx"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>

std::vector<char>::const_iterator
parse_SDP(const char *begin, const char *end,
          std::vector<El::BigFloat> &objectives,
          std::vector<El::BigFloat> &normalization,
          std::vector<Positive_Matrix_With_Prefactor> &matrices);

void read_mathematica(const boost::filesystem::path &input_path,
                      std::vector<El::BigFloat> &objectives,
                      std::vector<El::BigFloat> &normalization,
                      std::vector<Positive_Matrix_With_Prefactor> &matrices)

{
  boost::filesystem::ifstream input_stream(input_path);
  if(!input_stream.good())
    {
      throw std::runtime_error("Unable to open input: " + input_path.string());
    }

  boost::interprocess::file_mapping mapped_file(
    input_path.c_str(), boost::interprocess::read_only);
  boost::interprocess::mapped_region mapped_region(
    mapped_file, boost::interprocess::read_only);

  try
    {
      const char *begin(static_cast<const char *>(mapped_region.get_address())),
        *end(begin + mapped_region.get_size());
      parse_SDP(begin, end, objectives, normalization, matrices);
    }
  catch(std::exception &e)
    {
      throw std::runtime_error("Error when parsing " + input_path.string()
                               + ": " + e.what());
    }
}
