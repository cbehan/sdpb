#include "../SDP_Solver.hxx"

void SDP_Solver::save_solution(
  const SDP_Solver_Terminate_Reason terminate_reason,
  const boost::filesystem::path &out_file)
{
  // El::Print requires all processors, but we do not want all processors
  // writing header information.
  // FIXME: Write separate files for each rank.

  boost::filesystem::ofstream ofs(out_file);
  if(El::mpi::Rank() == 0)
    {
      std::cout << "Saving solution to      : " << out_file << '\n';
      ofs << "terminateReason = \"" << terminate_reason << "\";\n"
          << "primalObjective = " << primal_objective_elemental << ";\n"
          << "dualObjective   = " << dual_objective_elemental << ";\n"
          << "dualityGap      = " << duality_gap_elemental << ";\n"
          << "primalError     = " << primal_error_elemental << ";\n"
          << "dualError       = " << dual_error_elemental << ";\n"
          << "y = {";
    }
  El::Print(y_elemental, "", ofs);

  if(El::mpi::Rank() == 0)
    {
      ofs << "};\nx = {";
    }

  for(auto &block : x_elemental.blocks)
    {
      El::Print(block, "", ofs);
    }

  if(El::mpi::Rank() == 0)
    {
      ofs << "};\n";
    }
}
