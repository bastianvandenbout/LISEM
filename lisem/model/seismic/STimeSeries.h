// -*-c++-*-
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
#pragma once
#include <vector>
#include <string>

class EW;
class Sarray;
class SeisFilter;


class STimeSeries{

public:

// support for derived quantities of the time derivative are not yet implemented
  enum receiverMode{Displacement, Div, Curl, Strains, Velocity, DisplacementGradient /*, DivVelo, CurlVelo, StrainsVelo */ };

STimeSeries( EW* a_ew, std::string fileName, std::string staName, receiverMode mode, bool sacFormat, bool usgsFormat,
	    double x, double y, double z, bool topoDepth, int writeEvery, bool xyzcomponent=true );
~STimeSeries();

void allocateRecordingArrays( int numberOfTimeSteps, double startTime, double timeStep );
  
void recordData(std::vector<double> & u);

void writeFile( std::string suffix="" );

void readFile( EW* ew, bool ignore_utc );

double **getRecordingArray(){ return mRecordedSol; }

int getNsteps() const {return mLastTimeStep+1;}

bool myPoint(){ return m_myPoint; }

receiverMode getMode(){ return m_mode; }

double getX() const {return mX;}
double getY() const {return mY;}
double getZ() const {return mZ;}

double arrival_time( double lod );

STimeSeries* copy( EW* a_ew, std::string filename, bool addname=false );

double misfit( STimeSeries& observed, STimeSeries* diff, double& dshift, double& ddshift, double& dd1shift );
double misfit2( STimeSeries& observed );

void interpolate( STimeSeries& intpfrom );

void use_as_forcing( int n, std::vector<Sarray>& f, std::vector<double> & h, double dt,
		     Sarray& Jac, bool topography_exists );

double product( STimeSeries& ts ) const;
double product_wgh( STimeSeries& ts ) const;

   //void reset_utc( int utc[7] );
void set_utc_to_simulation_utc();
void filter_data( SeisFilter* filter_ptr );
void print_timeinfo() const;
void set_window( double winl, double winr );
void exclude_component( bool usex, bool usey, bool usez );
void readSACfiles( EW* ew, const char* sac1, const char* sac2, const char* sac3, bool ignore_utc );
void set_shift( double shift );
double get_shift() const;
void add_shift( double shift );
std::string getStationName(){return m_staName;}

void set_scalefactor( double value );
bool get_compute_scalefactor() const;
double get_scalefactor() const;

// for simplicity, make the grid point location public
int m_i0;
int m_j0;
int m_k0;
int m_grid0;

private:   
STimeSeries();
void write_usgs_format( std::string a_fileName);
void write_sac_format( int npts, char *ofile, float *y, float btime, float dt, char *var,
		       float cmpinc, float cmpaz);
double utc_distance( int utc1[7], int utc2[7] );
void dayinc( int date[7] );
int lastofmonth( int year, int month );
int utccompare( int utc1[7], int utc2[7] );
int leap_second_correction( int utc1[7], int utc2[7] );

void readSACheader( const char* fname, double& dt, double& t0, double& lat,
		    double& lon, double& cmpaz, double& cmpinc, int utc[7], int& npts);
void readSACdata( const char* fname, int npts, double* u );		    
void convertjday( int jday, int year, int& day, int& month );   
void getwgh( double ai, double wgh[6], double dwgh[6], double ddwgh[6] );
void getwgh5( double ai, double wgh[6], double dwgh[6], double ddwgh[6] );

receiverMode m_mode;
int m_nComp;

bool m_myPoint; // set to true if this processor writes to the arrays

std::string m_fileName, m_staName;

double mX, mY, mZ, mGPX, mGPY, mGPZ; // original and actual location
double m_zTopo;

bool m_zRelativeToTopography; // location is given relative to topography

int mWriteEvery;

bool m_usgsFormat, m_sacFormat;
std::string m_path;

// start time, shift, and time step 
double m_t0, m_shift, m_dt;

// size of recording arrays
int mAllocatedSize;

// index of last recorded element
int mLastTimeStep;

// recording arrays
double** mRecordedSol;
float**  mRecordedFloats;

// ignore this station if it is above the topography or outside the computational domain
//bool mIgnore;

// sac header data
int mEventYear, mEventMonth, mEventDay, mEventHour, mEventMinute;
double mEventSecond, m_rec_lat, m_rec_lon, m_rec_gp_lat, m_rec_gp_lon;
double m_epi_lat, m_epi_lon, m_epi_depth, m_epi_time_offset, m_x_azimuth;

// sac ascii or binary?
bool mBinaryMode;

// UTC time for start of seismogram, 
//     m_t0  =  m_utc - 'utc reference time',
//           where 'utc reference time' corresponds to simulation time zero.
//     the time series values are thus given by simulation times t_k = m_t0 + m_shift + k*m_dt, k=0,1,..,mLastTimeStep
int m_utc[7];

// Variables for rotating the output displacement or velocity components when Nort-East-UP is 
// selected (m_xyzcomponent=false) instead of Cartesian components (m_xyzcomponent=true).
bool m_xyzcomponent;
double m_calpha, m_salpha, m_thxnrm, m_thynrm;

bool m_compute_scalefactor;
double m_scalefactor;

//  double m_dthi, m_velocities;
// double m_dmx, m_dmy, m_dmz, m_d0x, m_d0y, m_d0z;
// double m_dmxy, m_dmxz, m_dmyz, m_d0xy, m_d0xz, m_d0yz;

// Window for optimization, m_winL, m_winR given relative simulation time zero.
   double m_winL, m_winR;
   bool   m_use_win, m_use_x, m_use_y, m_use_z;

// quiet mode?
   bool mQuietMode;

// pointer to EW object
   EW * m_ew;

};


