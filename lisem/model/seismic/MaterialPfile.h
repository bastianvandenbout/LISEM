//  SW4 LICENSE
// # ----------------------------------------------------------------------
// # SW4 - Seismic Waves, 4th order
// # ----------------------------------------------------------------------
// # Copyright (c) 2013, Lawrence Livermore National Security, LLC. 
// # Produced at the Lawrence Livermore National Laboratory. 
// # 
// # Written by:
// # N. Anders Petersson (petersson1@llnl.gov)
// # Bjorn Sjogreen      (sjogreen2@llnl.gov)
// # 
// # LLNL-CODE-643337 
// # 
// # All rights reserved. 
// # 
// # This file is part of SW4, Version: 1.0
// # 
// # Please also read LICENCE.txt, which contains "Our Notice and GNU General Public License"
// # 
// # This program is free software; you can redistribute it and/or modify
// # it under the terms of the GNU General Public License (as published by
// # the Free Software Foundation) version 2, dated June 1991. 
// # 
// # This program is distributed in the hope that it will be useful, but
// # WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
// # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the terms and
// # conditions of the GNU General Public License for more details. 
// # 
// # You should have received a copy of the GNU General Public License
// # along with this program; if not, write to the Free Software
// # Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA 
#ifndef MATERIALPFILE_FILE
#define MATERIALPFILE_FILE

#include <string>

#include "MaterialData.h"

class EW;

using namespace std;

class MaterialPfile : public MaterialData
{
 public:
   
   MaterialPfile( EW * a_ew,
	      const std::string file,
	      const std::string directory,
	      const int nstenc,
	      const double vpminppm,
	      const double vsminppm,
	      const double rhominppm,
	      const bool flatten,
	      const bool coords_geographic );

  ~MaterialPfile();

  void set_material_properties(std::vector<Sarray> & rho, 
			       std::vector<Sarray> & cs,
			       std::vector<Sarray> & cp, 
			       std::vector<Sarray> & xis, 
			       std::vector<Sarray> & xip);

  //  int get_material_pt( double x, double y, double z, double& rho, double& cs, double& cp,
  //		       double& qs, double& qp );

  //  void getMinMaxBoundsZ(double& zmin, double& zmax);
   
 protected:
  inline bool inside( double lat, double lon, double depth )
  {
    return m_latmin <= lat && lat <= m_latmax && m_lonmin <= lon && lon <= m_lonmax 
      && m_depthmin <= depth && depth <= m_depthmax;
  }

  inline bool inside_cart( double x, double y, double depth )
  {
    return m_xmin <= x && x <= m_xmax && m_ymin <= y && y <= m_ymax 
      && m_depthmin <= depth && depth <= m_depthmax;
  }

   void read_pfile( );

   void sample_cart(double xs, double ys, double zs, double &vp, 
		    double &vs, double &rho, double &qp, double &qs, bool debug );

   void sample_latlon(double lats, double lons, double zs, double &vp, 
		      double &vs, double &rho, double &qp, double &qs,
		      bool debug );

   EW* mEW;
   int m_nlat, m_nlon, m_nmaxdepth, m_nx, m_ny;
   int m_nstenc;
   double m_h, m_dlon, m_dlat;
   int     m_ksed, m_kmoho, m_k410, m_k660;
   double *m_lon, *m_lat, *m_x, *m_y;
// new 3-dimensional Sarrays
   Sarray mZ, mVp, mVs, mRho, mQp, mQs;
   
   double  m_vpmin, m_vsmin, m_rhomin;
   string m_model_file, m_model_dir, m_model_name;
   bool m_qf;

   double m_latmin, m_latmax, m_lonmin, m_lonmax, m_depthmin, m_depthmax;
   double m_xmin, m_xmax, m_ymin, m_ymax;
   bool m_coords_geographic;
};
#endif
