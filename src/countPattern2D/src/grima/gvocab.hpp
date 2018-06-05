/***************************************************************************
 *   Copyright (C) 2005 by Siegfried Nijssen                               *
 *   snijssen[at]informatik.uni-freiburg.de                                *
 * ----------------------------------------------------------------------- *
 *   Copyright (C) 2010,2011 by Adriana Prado and Baptiste Jeudy           *
 *   baptiste.jeudy[at]univ-st-etienne fr                                  *
 * ----------------------------------------------------------------------- *
 *   Copyright (C) 2014 by Romain Deville                                  *
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

//================================= IFDEF ====================================//
#ifndef GRIMAVOCAB_HPP
#define GRIMAVOCAB_HPP

//================================ INCLUDE ===================================//
// Include library class
// Include project class
#include "gglobal.hpp"
#include "gdatabase.hpp"
#include "gpattern.hpp"
//#include "../holeG/hgrimaglobal.hpp"
//#include "../holeG/hgrimagraph.hpp"
//#include "../holeG/hgrimagraphecode.hpp"
//#include "../holeG/hgrimapattern.hpp"
//#include "../holeG/hgrimaangle.hpp"
//#include "../holeG/hsparseset.hpp"
//#include "hgrimadatabase.hpp"

//=============================== NAMESPACE ==================================//
//========================== STRUCT & TYPEDEF ==============================//
/**
 * @brief The GExtToken struct
 * Token of an extension with the codeToken, the nodeFrom in the large graph
 * NodeLargeI and the nodeDest in the large graph NodeLargeJ
 */
struct GExtToken {
  GToken codeToken;
  GNodeID nodeLargeI;
  GNodeID nodeLargeJ;
};

/**
 * @brief The GExtTokenLt struct
 * Struct for comparaison between two GExtToken
 */
struct GExtTokenLt {
  bool operator()(const GExtToken &t1, const GExtToken &t2 ) const
  {
    return cmpGToken ( t1.codeToken, t2.codeToken ) < 0;
    // The order of the nodeId does not matter.
  }
};

//=============================== VARIABLES ==================================//
//================================ METHODS ===================================//

//================================= CLASS ====================================//
/**
 * @brief The GVocab class
 * Class that store a vector of pattern found, can compare two pattern and
 * process of patternCounting in largeGraph from the vector of pattern.
 */
class GVocab
{
  //---- PUBLIC --------------------------------------------------------------//
public:
  // Public CONSTANTS ________________________________________________________//
  // Public Structure & Typedef ______________________________________________//
  // Public Variables ________________________________________________________//
  /// Vector of graphs
  vector<GPattern*> v_Patterns;
  /// Vector that list the number of automorphism
  vector<int> v_Automorphism;
  /// Index of current class on which pattern are counted
  uint idxCurrClass;

  // Public Constructor/Desctructor __________________________________________//
  /// Default constructor
  GVocab();
  /// Default destructor
  ~GVocab();

  // Accessor ________________________________________________________________//
  // Mutator _________________________________________________________________//
  // Public Methods __________________________________________________________//
  /**
   * @brief comp
   * Method that compare 2 pattern (pPat1 and pPat2).
   * @param pPat1 ; First pattern to compare
   * @param pPat2 : Second pattern to compare
   * @return TRUE if pPat1 < pPat2, FALSE if pPat2 < pPat1
   */
  bool comp( GPattern* pPat1, GPattern* pPat2 );

  /**
   * @brief process
   * Method that count occurence of each pattern in v_Patterns for each
   * first possible edge m_TokenData
   * @param m_TokenData
   * @param offset
   */
  void process(GDatabase &graphDB );

  void saveVocab( string filename );

  void readVocab( string filename );

  vector<int> getLineIntoIntTokens( istream& str );

  void readPPEIdx( string filename );

  void saveHist( string filename , vector<GClassDB*> &v_ClassDB );

  void saveLbl( string filename , vector<GClassDB*> &v_ClassDB );

  void saveOcc( string filename );

  void computeAutomorphism();

//  void saveAutomorphism( string fileName, vector<int> v_Automorphism );
//  void saveAutomorphismCSV( string fileName, vector<int> v_Automorphism );

//  vector<int> loadAutomorphism( string fileName );

  //---- PROTECTED  ----------------------------------------------------------//
protected:
  // Protected CONSTANTS _____________________________________________________//
  // Protected Variables _____________________________________________________//
  // Protected Methods _______________________________________________________//

  //---- PRIVATE -------------------------------------------------------------//
private:
  // Private CONSTANTS _______________________________________________________//
  // Private Structure _______________________________________________________//
  // Private Variables _______________________________________________________//
  // Private Methods _________________________________________________________//
  /**
   * @brief run
   * Private methods used by process that actually run the ismorphisms test
   * for the pattern i and for each canonical first edge in the sparseSet that
   * are stored in object GTokenData in the iterator
   * @param i : ID of the pattern to count
   * @param it : iterator that allow access to canonical first edge sparseset
   * @param offset
   */
  void run( uint i, map<GToken,GTokenData,GTokenGt>::iterator it );
};

//============================== OPERATOR OVERLOAD  ==========================//
//================================= END IFDEF ================================//
#endif // GRIMAVOCAB_HPP

