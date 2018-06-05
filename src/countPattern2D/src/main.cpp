/***************************************************************************
 *   Copyright (C) 2016 by Romain Deville                                  *
 *   romain.deville[at]insa-lyon.fr                                        *
 * ----------------------------------------------------------------------- *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

//================================ INCLUDE ===================================//
// Include library class
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <sstream>
// Include project class
#include "global.hpp"
#include "grima/greader.hpp"
#include "grima/gglobal.hpp"
#include "grima/gdatabase.hpp"
#include "grima/grima.hpp"


//=============================== NAMESPACE ==================================//
using namespace std;

//============================ STRUCT & TYPEDEF ==============================//
//=============================== VARIABLES ==================================//
//================================ METHODS ===================================//
/**
 * @brief help
 * Print usage of the program if user input is invalid or directly called by
 * user
 */
void help()
{
  /*
   * TODO : RD
   * Write usage
   */
  cout << "TODO : RD " << endl
       << "Write HELP !!!" << endl <<
          "{           \"debug\",       no_argument, 0, 'D'}," << endl <<
          "{         \"warning\",       no_argument, 0, 'W'}," << endl <<
          "{             \"tor\",       no_argument, 0, 't'}," << endl <<
          "{            \"hole\",       no_argument, 0, 'h'}," << endl <<
          "{             \"csv\", required_argument, 0, 'c'}," << endl <<
          "{       \"occurency\", required_argument, 0, 'o'}," << endl <<
          "{     \"noedgelabel\",       no_argument, 0, 'n'}," << endl <<
          "{\"initialTimeStart\", required_argument, 0, 'I'}," << endl <<
          "{  \"sizeTimeToRead\", required_argument, 0, 's'}," << endl <<
          "{         \"pattern\", required_argument, 0, 'p'}," << endl <<
          "{            \"help\",       no_argument, 0,  0 }," << endl;
}
// End of help()

/**
 * @brief parse argument give by user
 * @param argc: # of argument
 * @param argv: vector of argument
 * @return 0 if no error, 1 otherwise
 */
void parseArg( int argc, char **argv, param &PARAM )
{
  string   infile;
  int      c;

  while ( true )
  {
    static struct option long_options[] =
    {
      // These options are distinguish by their indices.
    {           "debug",       no_argument, 0, 'D'},
    {         "warning",       no_argument, 0, 'W'},
    {             "tor",       no_argument, 0, 't'},
    {             "csv", required_argument, 0, 'c'},
    {           "label", required_argument, 0, 'l'},
    {    "indirecthole",       no_argument, 0, 'h'},
    {      "directhole",       no_argument, 0, 'H'},
    {       "occurency", required_argument, 0, 'o'},
    {     "noedgelabel",       no_argument, 0, 'n'},
    {"initialTimeStart", required_argument, 0, 'I'},
    {  "sizeTimeToRead", required_argument, 0, 's'},
    {         "pattern", required_argument, 0, 'p'},
    {         "PPEFile", required_argument, 0, 'P'},
    {           "NBPPE", required_argument, 0, 'N'},
    {   "graphIDoffset", required_argument, 0, 'O'},
    {            "help",       no_argument, 0,  0 },
    {                 0,                 0, 0,  0 }
  };
    // getopt_long stores the option index here.
    int option_index = 0;
    c = getopt_long( argc, argv, "DWtHhnc:p:o:s:I:l:N:P:O:R:F:", long_options, &option_index );
    // Detect the end of the options.
    if ( c == -1 )
      break;
    switch ( c )
    {
    case 0:
      if ( (string)long_options[option_index].name == "help" )
      {
        help();
        exit( EXIT_SUCCESS );
      }
      break;
    case 'D':
      PARAM.DEBUG_MODE = true;
      break;
    case 'W':
      PARAM.WARNING_MODE = true;
      break;
    case 't':
      PARAM.TORGRAPH = true;
      break;
    case 'h':
      PARAM.INDIRECT_HOLEMINING = true;
      break;
    case 'H':
      PARAM.DIRECT_HOLEMINING = true;
      break;
    case 'c':
      PARAM.OUTHISTFILE = optarg;
      break;
    case 'o':
      PARAM.OCCFILE = optarg;
      break;
    case 'l':
      PARAM.OUTLBLFILE = optarg;
      break;
    case 's':
      PARAM.NBTIMETOREAD = atoi(optarg);
      break;
    case 'I':
      PARAM.INITIALTIME = atoi(optarg);
      break;
    case 'n':
      PARAM.NOEDGELABEL = true;
      break;
    case 'p':
      PARAM.PATFILE  = optarg;
      break;
    case 'N':
      PARAM.NBPPE = atoi(optarg);
      break;
    case 'P':
      PARAM.PPEFILE = optarg;
      break;
    case 'O':
      PARAM.GRAPHIDOFFSET = atoi(optarg);
      break;
    default:
      help();
      exit( EXIT_FAILURE );
    }
  }
#ifdef _DEBUG
  PARAM.DEBUG_MODE = true;
  PARAM.WARNING_MODE = true;
#endif
  // Print help if to wrong number of arg
  if ( argc - optind != 1 )
  {
    help();
    exit( EXIT_FAILURE );
  }


  // Check if input file exist and is openable, if not exist, else set
  // INPUTFILE
  infile = argv[optind];
  if ( ! ifstream( infile ) )
  {
    cerr << "ERROR - file " << infile << " does not exist ." << endl;
    exit ( EXIT_FAILURE );
  }
  else
    PARAM.INFILE=infile;

  // Check if output file is define, if not, then set output file equal to raw
  // input file but change extension
  if ( PARAM.PATFILE == "" )
  {
    cerr << "ERROR - A pattern file must be specified with option -p <fileName>" << endl;
    help();
    exit( EXIT_FAILURE );
  }

  if ( PARAM.OUTHISTFILE== "" )
  {
    cerr << "ERROR - An output histogram file must be specified with option -c <fileName>" << endl;
    help();
    exit( EXIT_FAILURE );
  }

  if ( PARAM.PPEFILE != "" && ! ifstream( PARAM.PPEFILE ) )
  {
    cerr << "ERROR - file " << PARAM.PPEFILE << " does not exist ." << endl;
    exit ( EXIT_FAILURE );
  }
  else if ( PARAM.PPEFILE != "" && PARAM.NBPPE == 0 )
  {
    cerr << "WARNING : No number of selected pattern to use is set. Thus will "
            "use all of selected pattern with post processing" << endl;
  }
  //  if ( PARAM.OUTLBLFILE== "" )
  //  {
  //    cerr << "ERROR - An output histogram file must be specified with option -h <fileName>" << endl;
  //    help();
  //    exit( EXIT_FAILURE );
  //  }
}
// End of parseArg( int argc, char **argv )

// ===== CONSTANT DEFINITION =================================================//
param PARAM = param();

//============================================================================//
//                                  MAIN                                      //
//============================================================================//
/**
  * @brief Main function
  * Read graph file
  *   - Determine number of graph
  *   - Determine max number of label for color
  * IF one outfile (i.e. no option '-s')
  *   - Write latex header to outfile.tex
  *   - Write color definition to outfile.tex
  *   - Write one graph per page in tikz format resize to fit page
  * ELSE
  *   - Write latex header to outfile-header.tex
  *   - Write color definition to outfile-header.tex
  *   FOR each graph
  *     - Write one graph in tikz format resize to fit page in outfile-i.tex
  *     - Write an input to this file to outfile-header.tex
  * @param argc # of arg
  * @param argv vector of arg
  * @return 0 if no issu, 1 otherwise
  */
int main( int argc, char **argv )
{
  if ( PARAM.DEBUG_MODE )
    cout << "#==== START OF COUNT PATTERN !" << endl;

  int returnStatus = 0;
  GDatabase graphDB;
  Grima     grima;

  parseArg( argc, argv, PARAM );

  graphDB.createGrapheDB( PARAM.INFILE );
  cout << PARAM.PATFILE << endl;
  grima.vocabPattern->readVocab( PARAM.PATFILE );
  if ( PARAM.PPEFILE != "" )
    grima.vocabPattern->readPPEIdx( PARAM.PPEFILE );
  grima.vocabPattern->process( graphDB );
  grima.vocabPattern->computeAutomorphism();
  grima.vocabPattern->saveHist( PARAM.OUTHISTFILE, graphDB.v_GClassDB );
  grima.vocabPattern->saveLbl( PARAM.OUTLBLFILE, graphDB.v_GClassDB );
  if ( PARAM.OCCFILE != "" )
    grima.vocabPattern->saveOcc( PARAM.OCCFILE );

  if ( PARAM.DEBUG_MODE )
    cout << "#==== END OF COUNT PATTERN !" << endl;
  return returnStatus;
}
// End of main( int argc, char **argv )
