#include "sdpb_util/Boost_Float.hxx"
#include "pmp_read/pmp_read.hxx"
#include "sdpb_util/assert.hxx"

namespace fs = std::filesystem;

void parse_command_line(int argc, char **argv, int &precision,
                        std::vector<fs::path> &input_files,
                        fs::path &output_dir);

void write_functions(
  const fs::path &output_path,
  const std::vector<El::BigFloat> &dual_objective_b,
  const std::vector<Polynomial_Vector_Matrix> &polynomial_vector_matrices);

int main(int argc, char **argv)
{
  El::Environment env(argc, argv);

  try
    {
      // TODO fix parallel
      if(El::mpi::Size() > 1)
        RUNTIME_ERROR("pvm2functions cannot work in parallel!");

      int precision;
      std::vector<fs::path> input_files;
      fs::path output_path;

      parse_command_line(argc, argv, precision, input_files, output_path);
      El::gmp::SetPrecision(precision);
      Boost_Float::default_precision(precision * log(2) / log(10));

      ASSERT(output_path != ".", "Output file is a directory: ", output_path);
      ASSERT(!(fs::exists(output_path) && fs::is_directory(output_path)),
             "Output file exists and is a directory: ", output_path);
      const auto pmp = read_polynomial_matrix_program(input_files);
      write_functions(output_path, pmp.objective, pmp.matrices);
    }
  catch(std::exception &e)
    {
      std::cerr << "Error: " << e.what() << "\n" << std::flush;
      El::mpi::Abort(El::mpi::COMM_WORLD, 1);
    }
  catch(...)
    {
      std::cerr << "Unknown Error\n" << std::flush;
      El::mpi::Abort(El::mpi::COMM_WORLD, 1);
    }
}
