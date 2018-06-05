/***************************************************************************
 *   Copyright (C) 2005 by Siegfried Nijssen                               *
 *   snijssen@informatik.uni-freiburg.de                                   *
 * ----------------------------------------------------------------------- *
 *   Copyright (C) 2010,2011 by Adriana Prado and Baptiste Jeudy           *
 *   baptiste.jeudy at univ-st-etienne fr                                  *
 * ----------------------------------------------------------------------- *
 *   Copyright (C) 2015 by Romain Deville                                  *
 *   romain.deville[at]insa-lyon.fr                                        *
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

//================================= IFDEF ====================================//
#ifndef GRIMA_HPP
#define GRIMA_HPP

//================================ INCLUDE ===================================//
// Include library class
// Include project class
#include "gglobal.hpp"
#include "ggraph.hpp"
#include "gdatabase.hpp"
#include "gvocab.hpp"
#include "gpattern.hpp"
#include "gextensioncollect.hpp"
#include "gsubgraphiso.hpp"
#include "gvocab.hpp"

//=============================== NAMESPACE ==================================//
//============================ STRUCT & TYPEDEF ==============================//
//=============================== VARIABLES ==================================//
//================================ METHODS ===================================//

//================================= CLASS ====================================//
/**
 * @brief The G class
 * Class that store vocabulary of pattern and allow to process grima mining
 * algorithm for each class.
 */
class Grima
{
  //---- PUBLIC --------------------------------------------------------------//
public:
  // Public CONSTANTS ________________________________________________________//
  // Public variables ________________________________________________________//
  /// Vocabulary of pattern
  GVocab* vocabPattern;
  /// Id of the last frequent pattern that will be incremented for each new pattern
  int     freqPatternId;

  // Public Structure & Typedef ______________________________________________//
  // Public Constructor/Desctructor __________________________________________//
  /**
   * @brief Grima
   * Default constructor
   * Initialize time log and create new GVocab object
   */
  Grima();

  /**
   * @brief ~G
   * Default Destructor
   * Free memory by deleting GVocab object
   */
  ~Grima();

  // Accessor ________________________________________________________________//
  // Mutator _________________________________________________________________//
  // Public Methods __________________________________________________________//
  /**
   * @brief processMining
   * Method that apply mining process algorithm by calling search trough all
   * images withing the same class in pClassDB for a user set min support.
   * @param minF
   * @param pClassDB
   */
  int processMining( float minF, GClassDB* pClassDB );

//  /**
//   * @brief processCount
//   * Methd that will find occurence of patterns in vocabPatter through all
//   * graphs v_Graphs.
//   * @param v_Graphs
//   * @param m_TokenData
//   */
//  void processCount( vector<GGraph*> v_Graphs ,
//                     map<GToken, GTokenData, GTokenGt> m_TokenData );

//  void savePatternVocab(string &vocabDirPath , bool timeOut, bool nbPatLimit );

  //---- PROTECTED  ----------------------------------------------------------//
protected:
  // Protected CONSTANTS _____________________________________________________//
  // Protected Structure & Typedef ___________________________________________//
  // Protected Variables _____________________________________________________//
  // Protected Methods _______________________________________________________//

  //---- PRIVATE -------------------------------------------------------------//
private:
  // Private CONSTANTS _______________________________________________________//
  // Private Structure & Typedef _____________________________________________//
  // Private Variables _______________________________________________________//
  // Private Methods _________________________________________________________//
  /**
   * TODO : RD
   * Write Desc
   */
  int search( vector<GGraph*>                   &v_Graphs,
              map<GToken, GTokenData, GTokenGt> &m_TokenData,
              GGlobFreq                         minFreq );

  /**
   * @brief search
   * Method recursivly called to mine graph
   * === First step  : Add extension
   * === Second step : Check if P is canonical
   * === Third step  : Compute all possible extension of P
   * === Fourth step : Output pattern and occurences in output file
   * === Fifth step  : If pattern is frequent, recursive call, else exit
   * @param v_Graphs
   * @param minFreq
   * @param pPattern
   * @param lastExt
   * @param tokenData
   * @param suppData
   * @return
   */
  int search ( vector<GGraph*> &v_Graphs,
               GGlobFreq       &minFreq,     //Mininmum global frequency
               GPattern*       pPattern,     //Current pattern
               const GToken    &lastExt,     //Last extension that will be adde
               GTokenData      &tokenData,   //Struct with freq,v_TId & sparseset
               GExtensionData  &suppData );  // Tmp variable, supposed frequency
};

//============================== OPERATOR OVERLOAD  ==========================//
//================================= END IFDEF ================================//
#endif // GRIMA_HPP
