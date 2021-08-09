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
#ifndef SW4_IMAGE3D_H
#define SW4_IMAGE3D_H

#include <string>
#include <sstream>
#include <vector>

#include "boundaryConditionTypes.h"
#include "Sarray.h"
#include "Parallel_IO.h"

class EW;

class SeisImage3D
{
public:
  enum Image3DMode { NONE, UX, UY, UZ, RHO, LAMBDA, MU, P, S, GRADRHO, GRADMU, GRADLAMBDA, GRADP, GRADS, QP, QS };
// 15 modes are currently defined

   static SeisImage3D* nil;

   SeisImage3D( EW * a_ew,
	    double time, 
	    double timeInterval, 
	    int cycle, 
	    int cycleInterval,
	    double tstart,
	    const std::string& filePrefix, 
	    Image3DMode mode,
	    bool doubleMode );
   ~SeisImage3D();

   void setup_images( );

   static void setSteps(int a_steps);

   //   void set_double( bool val=true ); 

   void update_image( int a_cycle, double a_time, double a_dt, std::vector<Sarray>& a_U, 
		      std::vector<Sarray>& a_Rho, std::vector<Sarray>& a_Mu, std::vector<Sarray>& a_Lambda,
		      std::vector<Sarray>& a_gRho, std::vector<Sarray>& a_gMu, std::vector<Sarray>& a_gLambda,
		      std::vector<Sarray>& a_Qp, std::vector<Sarray>& a_Qs,
		      std::string a_path, Sarray& a_Z );

   void force_write_image( double a_time, int a_cycle, std::vector<Sarray>& a_U,
               std::vector<Sarray>& a_Rho, std::vector<Sarray>& a_Mu, std::vector<Sarray>& a_Lambda,
               std::vector<Sarray>& a_gRho, std::vector<Sarray>& a_gMu, std::vector<Sarray>& a_gLambda,
               std::vector<Sarray>& a_Qp, std::vector<Sarray>& a_Qs,
               std::string a_path, Sarray& a_Z);

   //   void set_start_time(double tStart);

protected:
   bool timeToWrite( double time, int cycle, double dt );

   void compute_image( std::vector<Sarray>& a_U, std::vector<Sarray>& a_Rho,
		       std::vector<Sarray>& a_Mu, std::vector<Sarray>& a_Lambda,
		       std::vector<Sarray>& a_gRho, std::vector<Sarray>& a_gMu,
		       std::vector<Sarray>& a_gLambda,
     		       std::vector<Sarray>& a_Qp, std::vector<Sarray>& a_Qs );

   void write_image( int cycle, std::string &path, double t, Sarray& a_Z );

   void define_pio( );

   void compute_file_suffix( int cycle, std::stringstream & fileSuffix );

   Image3DMode mMode;
   std::string mFilePrefix;
   double mTime;
   bool m_time_done;
   double mTimeInterval;
   double mNextTime;
   double mStartTime;

   int mWritingCycle;
   int mCycleInterval;
   int mImageSamplingFactor;

   std::string mFileName;
   std::string m_modestring;

   bool m_isDefinedMPIWriters;
   bool m_winallocated;
   bool m_memallocated;

   static int mPreceedZeros; // number of digits for unique time step in file names
  
private:
   SeisImage3D(); // make it impossible to call default constructor
   SeisImage3D(const SeisImage3D &im); // hide copy constructor

   bool m_double;
   EW* mEW;
   Parallel_IO ** m_parallel_io;

   std::vector<int*> mWindow; // Local in processor start + end indices for (i,j,k) for each grid level
   std::vector<int*> mGlobalDims; // Global start + end indices for (i,j,k) for each grid level

   std::vector<double*> m_doubleField;
   std::vector<float*> m_floatField;
   std::vector<int> m_extraz;
   std::vector<bool> m_ihavearray;
};

#endif
