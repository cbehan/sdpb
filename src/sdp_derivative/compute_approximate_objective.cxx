#include "../sdp_solve.hxx"

void compute_dx_dy(const Block_Info &block_info, const El::Grid &grid,
                   const SDP &sdp, const SDP &d_sdp, const Block_Vector &x,
                   const Block_Vector &y, const Block_Diagonal_Matrix &Y,
                   Block_Vector &dx, Block_Vector &dy);

El::BigFloat
compute_approximate_objective(const Block_Info &block_info, const El::Grid &grid,
                              const SDP &sdp, const SDP &d_sdp,
                              const Block_Vector &x, const Block_Vector &y,
                              const Block_Diagonal_Matrix &Y)
{
  Block_Vector dx(x), dy(y);
  compute_dx_dy(block_info, grid, sdp, d_sdp, x, y, Y, dx, dy);
  
  // b.y
  El::BigFloat objective(El::Dot(sdp.dual_objective_b, y.blocks.at(0))
                         + El::Dot(d_sdp.dual_objective_b, y.blocks.at(0)));

  std::cout << "objective: "
            << El::Dot(sdp.dual_objective_b, y.blocks.at(0)) << "\n "
            << El::Dot(d_sdp.dual_objective_b, y.blocks.at(0));

  // ydy = y + dy
  El::DistMatrix<El::BigFloat> ydy(y.blocks.at(0));
  ydy+=dy.blocks.at(0);

  // db.(y + dy)
  objective+=El::Dot(d_sdp.dual_objective_b, ydy);
  
        
  El::BigFloat local_sum(0);
  for(size_t block(0); block != x.blocks.size(); ++block)
    {
      // dc.x
      local_sum
        += Dotu(d_sdp.primal_objective_c.blocks.at(block), x.blocks.at(block));
      // dc.dx
      local_sum
        += Dotu(d_sdp.primal_objective_c.blocks.at(block), dx.blocks.at(block));

      std::cout << "\n " << local_sum;
      {
        // temp = dB.(y + dy)
        El::DistMatrix<El::BigFloat> temp(x.blocks.at(block));
        El::Zero(temp);
        El::Gemv(El::Orientation::NORMAL, El::BigFloat(1.0),
                 d_sdp.free_var_matrix.blocks[block], ydy,
                 El::BigFloat(0.0), temp);

        // xdx = x + dx
        El::DistMatrix<El::BigFloat> xdx(x.blocks[block]);
        xdx+=dx.blocks[block];
        
        // -(x + dx).dB.(y + dy)
        // Doing it this way reduces the number of dot products, but
        // includes the third order term dx.dB.dy which comes along for
        // the ride.
        local_sum -= El::Dotu(temp, xdx);
      }
      std::cout << "\n " << local_sum << "\n";;
    }
  if(!x.blocks.empty() && x.blocks.at(0).Grid().Rank() != 0)
    {
      local_sum = 0;
    }

  
  return objective
         + El::mpi::AllReduce(local_sum, El::mpi::SUM, El::mpi::COMM_WORLD);
}
