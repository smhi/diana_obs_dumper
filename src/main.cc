/** @file main.cc
 * Main class for coserver4. Only used for initialization.
 * Run with -gui for GUI.
 * @author Martin Lilleeng Sætra <martinls@met.no>
 */

#include <stdlib.h>

#include <puTools/miCommandLine.h>
#include <puTools/miStringFunctions.h>
#include <iostream>
#include <cstdlib>
#include "diObsRoad.h"

using namespace std;

#define _DEBUG

int main(int argc, char *argv[])
{
#ifdef _DEBUG
  std::cerr << "diana_obs_dumper called" << std::endl;
#endif

#ifdef _DEBUG
  std::cerr << "argc: " << argc << std::endl;

  for(int i=0;i<argc;i++) {
    std::cerr << "argv: " << string(argv[i]) << std::endl;
  }
#endif
// filename, databasefile, stationfile, headerfile, time, timeDiff
  // parsing commandline-arguments
  vector<miCommandLine::option> opt(7);

  opt[0].flag = 'f';
  opt[0].alias = "filename";
  opt[0].hasArg = true;

  opt[1].flag = 'd';
  opt[1].alias = "databasefile";
  opt[1].hasArg = true;

  opt[2].flag = 's';
  opt[2].alias = "stationfile";
  opt[2].hasArg = true;

  opt[3].flag = 'H';
  opt[3].alias = "headerfile";
  opt[3].hasArg = true;

  opt[4].flag = 't';
  opt[4].alias = "time";
  opt[4].hasArg = true;

  opt[5].flag = 'D';
  opt[5].alias = "timeDiff";
  opt[5].hasArg = true;
  
  opt[6].flag = 'h';
  opt[6].alias = "help";
  opt[6].hasArg = false;

  miCommandLine cl(opt, argc, argv);
  string filename, databasefile, stationfile, headerfile, timestring;
  int timeDiff;
  string usage("Arguments: -f <filename> -d <databasefile> (moraconf) -s <stationfile> -H <headerfile> -t <time> (yyyymmddHHMMSS) -D <timeDiff> (59)");
  if (cl.hasFlag('h')) {
    std::cerr << usage << std::endl;
    return 0;
  }

  if (cl.hasFlag('f')) {
    if (cl.arg('f').size() > 0) {
      filename = cl.arg('f')[0];
    } else {
      std::cerr << usage << std::endl;
      exit (1);
    }
  } else {
    std::cerr << usage << std::endl;
    exit (1);
  }
  
  if (cl.hasFlag('d')) {
    if (cl.arg('d').size() > 0) {
      databasefile = cl.arg('d')[0];
    } else {
      std::cerr << usage << std::endl;
      exit (1);
    }
  } else {
    std::cerr << usage << std::endl;
    exit (1);
  }
  
  if (cl.hasFlag('s')) {
    if (cl.arg('s').size() > 0) {
      stationfile = cl.arg('s')[0];
    } else {
      std::cerr << usage << std::endl;
      exit (1);
    }
  } else {
    std::cerr << usage << std::endl;
    exit (1);
  }
  
  if (cl.hasFlag('H')) {
    if (cl.arg('H').size() > 0) {
      headerfile = cl.arg('H')[0];
    } else {
      std::cerr << usage << std::endl;
      exit (1);
    }
  } else {
    std::cerr << usage << std::endl;
    exit (1);
  }
  
  if (cl.hasFlag('t')) {
    if (cl.arg('t').size() > 0) {
      timestring = cl.arg('t')[0];
    } else {
      std::cerr << usage << std::endl;
      exit (1);
    }
  } else {
    std::cerr << usage << std::endl;
    exit (1);
  }
  
  if (cl.hasFlag('D')) {
    if (cl.arg('D').size() > 0) {
      timeDiff = miutil::to_int(cl.arg('D')[0]);
    } else {
      std::cerr << usage << std::endl;
      exit (1);
    }
  } else {
    std::cerr << usage << std::endl;
    exit (1);
  }
  
  miutil::miTime time(timestring);
  ObsRoad obsRoad(filename, databasefile, stationfile, headerfile, time, timeDiff, true);
  return 0;
}
