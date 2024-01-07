#pragma once

#include "sdp_convert/Polynomial_Vector_Matrix.hxx"

#include <El.hpp>

#include <filesystem>
#include <vector>

void read_xml(
  const std::filesystem::path &input_file,
  const std::function<bool(size_t matrix_index)> &should_parse_matrix,
  std::vector<El::BigFloat> &objective,
  size_t &num_matrices,
  std::map<size_t, Polynomial_Vector_Matrix> &polynomial_vector_matrices);
