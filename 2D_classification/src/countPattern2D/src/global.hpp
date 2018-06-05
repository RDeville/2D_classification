/***************************************************************************
 * Copyright (C) 2016 Deville Romain                                       *
 * Contact : romain.deville[at]insa-lyon.fr                                   *
 *-------------------------------------------------------------------------*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 *   or you can write to the                                               *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

//=== IFDEF ====================================================================
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

//================================ INCLUDE ===================================//
// Include library class
// -- C++ STD LIB
#include <string>
#include <vector>
#include <math.h>
// Include project class
//=============================== NAMESPACE ==================================//
using namespace std;

//========================== STRUCT & TYPEDEF ================================//
/// Shortcut for unsigned int
typedef unsigned int uint;

/**
 * @brief Store with option was called by user
 */
struct param {
  /// Store input file
  string INFILE; // REQUIRED
  /// Store output file were pattern will be saved
  string PATFILE;
  /// Store output file were histograms will be saved
  string OUTHISTFILE;
  /// Store output file were histograms will be saved
  string OUTLBLFILE;
  /// Store output file were occurences will be saved
  string OCCFILE;
  /// Specify that edge does not have label
  bool NOEDGELABEL;
  /// Store test file if specified
  bool TORGRAPH;
  /// Store boolean to know if grid will have hole
  bool INDIRECT_HOLEMINING;
  /// Store boolean to know if grid will have hole
  bool DIRECT_HOLEMINING;
  /// Used by mining to mine induced pattern
  bool INDUCED;
  /// Int to specify time limit to read for the graph
  int NBTIMETOREAD;
  /// Int to specify offset of graphID
  int GRAPHIDOFFSET;
  /// Int to specify temporal dimension to start
  int INITIALTIME;
  /// Show debug informations
  bool DEBUG_MODE;
  /// Show warning informations
  bool WARNING_MODE;
  int NBPPE;
  int NBKRAND;
  int NBKFIRST;
  string PPEFILE;
  ///
  int MAXLBL;
  bool SPAT_FIRST;
  bool TEMP_FIRST;


  param():
    INFILE(""),
    PATFILE(""),
    OUTHISTFILE(""),
    OUTLBLFILE(""),
    OCCFILE(""),
    NOEDGELABEL(false),
    TORGRAPH(false),
    INDIRECT_HOLEMINING(false),
    DIRECT_HOLEMINING(false),
    INDUCED(false),
    NBTIMETOREAD(-1),
    GRAPHIDOFFSET(0),
    INITIALTIME(0),
    DEBUG_MODE(false),
    WARNING_MODE(false),
    NBPPE(-1),
    NBKRAND(-1),
    NBKFIRST(-1),
    PPEFILE(""),
    MAXLBL(-1),
    SPAT_FIRST(true),
    TEMP_FIRST(false)
  {
    // Nothing to put here
  }

  ~param()
  {
    // Nothing to put here as variable deletion are already managed by C++ or std
  }
};
// End of struct param


//=============================== VARIABLES ==================================//
/// Boolean to know if output debug
extern struct param PARAM;

//================================ METHODS ===================================//
//=============================== CONSTANTS ==================================//
//=========================== OPERATOR OVERLOAD  =============================//
//================================= END IFDEF ================================//
#endif
