/*
  Diana - A Free Meteorological Visualisation Tool

  Copyright (C) 2006-2016 met.no and SMHI

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no

  This file is part of Diana

  Diana is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Diana is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Diana; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
//#ifndef ROADOBS
//#define ROADOBS 1
//#endif
//#ifdef ROADOBS
#include "diObsRoad.h"

#include <newarkAPI/diParam.h>
#include <newarkAPI/diStation.h>
#include <newarkAPI/diRoaddata.h>
#include <puTools/miStringFunctions.h>

#include <boost/bind/bind.hpp>

#include <algorithm>
#include <fstream>
#include <vector>

//#define DEBUGPRINT 1

using namespace road;
using namespace miutil;
using namespace std;

ObsRoad::ObsRoad(const std::string &filename, const std::string &databasefile, const std::string &stationfile, const std::string &headerfile,
		   const miTime &filetime, int timeDiff, bool breadData)
{
#ifdef DEBUGPRINT
	std::cerr << "++ ObsRoad::ObsRoad() ++" << std::endl;
#endif
	// init members
  filename_ = filename;
  databasefile_ = databasefile;
  stationfile_ = stationfile;
  headerfile_ = headerfile;
  filetime_ = filetime;
  timeDiff_ = timeDiff;
	if (!breadData) {
		readHeader();
	}
	else
		readData();
}

void ObsRoad::readHeader()
{
#ifdef DEBUGPRINT
  std::cerr << "++ ObsRoad::readHeader( headerfile: " << headerfile_ << " ) ++" << std::endl;
#endif
  int n,i;
  vector<std::string> vstr,pstr;
  std::string str;
	// Dont tamper with the plot object...
	if (true) {
	int theresult = diParam::initParameters(headerfile_);
    if (theresult)
	{
		// take a local copy
		vector<diParam> * params = NULL;
		map<std::string, vector<diParam> * >::iterator itp = diParam::params_map.find(headerfile_);
		if (itp != diParam::params_map.end())
		{
			params = itp->second;
		}
		/* this should not happen if configured properly */
		if (params == NULL)
		{
				//oplot->roadobsHeader = false;
			std::cerr << " ObsRoad::readHeader() error, parameterfile: " << headerfile_ << std::endl;
#ifdef DEBUGPRINT
  std::cerr << "++ ObsRoad::readHeader() done, error finding parameters ++" << std::endl;
#endif
		}

			// Construct a header line like this
			//[NAME UALF_Lyn]
			//[COLUMNS
                        //Ver:f:"UALF versjons nummer" Year:year:"aar" Month:month:"Maaned" Day:day:"Dag" Hour:hour:"Time" Min:min:"Min" Sec:sec:"Sekund" Ns:f:"Nano sekunder"
                        //Lat:lat:"Breddegrad" Lon:lon:"Lengdegrad" Pk:f:"Beregnet maksstroem i kA" Fd:f:"Multiplisitet for <<flash>> data(1 - 99) eller 0 for <<strokes>>" No_sens:f:"Antall sensorer med bidrag til beregningen" Df:f:"Antall frihetsgrader" Ea:r:"Ellipse vinkel fra 0 grader Nord" Major:r:"Lengste hovedakse i km" Minor:r:"Minste hovedakse i km" Chi:r:"Chi-kvadrat fra posisjons beregningen, 0-3.0 bra, 3.0-10.0 akseptabelt, >10.0 daarlig" Rise:r:"Stigetid for boelgeformen i mikrosekunder" Pz:r:"Tiden fra maks- til 0-stroem for boelgeformen i mikrosekunder" Mrr:r:"Maks stigningsforhold til boelgeformen i kA/us" Ci:f:"1 Sky-Sky, 0 sky-bakke" Ai:f:"1 vinkel data benyttet til aa beregne posisjonen, 0 ellers" Sig_i:f:"1 hvis sensor signal data er brukt til aa beregne posisjonen. 0 ellers" Ti:f:"1 hvis timing data er brukt til aa beregne posisjonen, 0 elles."
			//DeltaTime:deltatime:"Delta Time"]
			//[DATA]
			// Just in case ...
			lines.clear();
			std::string line;
			line = "[NAME MORA_OBSERVATIONS]";
			lines.push_back(line);
			line = "[COLUMNS";
			lines.push_back(line);
			// The fixed part..
			line = "Name:id: Date:d: Time:t: Lat:lat: Lon:lon: ";
			// the dynamic part
			// the data value parameters
		for (i = 0; i < params->size(); i++)
		{
			std::string name = (*params)[i].diananame();
				miutil::trim(name);
				line = line + name + ":r:" + (*params)[i].unit() + " ";
  }
			// The encloseing bracket
			line += "]";
			//cerr << line << endl;
			lines.push_back(line);
			// The fixed data line tells decode header to stop parsing
			line = "[DATA]";
			lines.push_back(line);
			// Now we should be ready to decode it...
		} // end if theresult
	} // end if true
#ifdef DEBUGPRINT
  std::cerr << "++ ObsRoad::readHeader()  done ++" << std::endl;
#endif
}

void ObsRoad::readData()
{
#ifdef DEBUGPRINT
	std::cerr<< "++ ObsRoad::readData( filename= " << filename_ << " databasefile= " << databasefile_ << " stationfile= " << stationfile_ << " headerfile= " << headerfile_ << " filetime= " << filetime_.isoTime() << " )++ " << std::endl;
#endif
  Roaddata::initRoaddata(databasefile_);
  diStation::initStations(stationfile_);
	// read the headerfile if needed
	readHeader();
	readRoadData();
#ifdef DEBUGPRINT
	std::cerr << "++ ObsRoad::readData()done ++ " << std::endl;
#endif
}

void ObsRoad::readRoadData()
{
#ifdef DEBUGPRINT
	std::cerr << "++ ObsRoad::readRoadData( filename= " << filename_ << " databasefile= " << databasefile_ << " stationfile= " << stationfile_ << " headerfile= " << headerfile_ << " filetime= " << filetime_.isoTime() << " )++ " << std::endl;
#endif
  // This defines a set of stations, eg synop,metar,ship observations
  std::vector<road::diStation> * stationlist = 0;
  
  Roaddata road = Roaddata(databasefile_, stationfile_, headerfile_, filetime_); 
  map<std::string, vector<diStation> * >::iterator its = diStation::station_map.find(stationfile_);
  if (its != diStation::station_map.end())
  {
	  stationlist = its->second;
  }
  // FIXME: For now, does nothing...
  road.open();
  
  map<int, std::string> lines_map;
  // Here, we always get all staions, so plot_stations will be empty..
  std::vector<int> plot_stations;
 
  road.getData(plot_stations, lines_map);
	// FIXME: For now, does nothing...
	road.close();

	// decode the data...
	std::string str;
	map<int, std::string>::iterator it=lines_map.begin();	
  for(;it!=lines_map.end(); it++) {
		//INDEX in station list
	  miutil::trim(str);
		// Append every line to 
		lines.push_back(str);
	}
  // write to disk
  // TBD: write to temporary name first
  std::ofstream ofs(filename_.c_str(),std::ofstream::out|std::ofstream::trunc);
	if (ofs.is_open())
	{ 
	  size_t size = lines.size();
    for (size_t i = 0; i < size; i++)
    {
      ofs << lines[i];
    }
		ofs.close();
  }
	
#ifdef DEBUGPRINT
	std::cerr << "++ ObsRoad::readRoadData()done ++ " << std::endl;
#endif
}




//#endif //ROADOBS
