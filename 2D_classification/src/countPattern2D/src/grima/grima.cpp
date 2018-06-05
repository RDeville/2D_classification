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

//================================ INCLUDE ===================================//
// Include library class
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <algorithm>
// Include project class
#include "grima.hpp"
#include "global.hpp"

//=============================== NAMESPACE ==================================//
using namespace std;

//============================ STRUCT & TYPEDEF ==============================//

//=============================== VARIABLES ==================================//
// Will be used in cmpGToken when extending spatFirst or tempFirst
GToken tmpT1 = GToken();
GToken tmpT2 = GToken();

//================================= CLASS ====================================//
//---- PUBLIC ----------------------------------------------------------------//
// Public CONSTANTS __________________________________________________________//
// Public Constructor/Desctructor ____________________________________________//
Grima::Grima():
  freqPatternId(0)
{
  /*
   * TODO : RD
   * Copy Desc
   */
  vocabPattern = new GVocab();
}

Grima::~Grima()
{
  /*
   * TODO : RD
   * Copy Desc
   */
  delete vocabPattern;
}

// Accessor __________________________________________________________________//
// Mutator ___________________________________________________________________//
// Public Methods ____________________________________________________________//
//===========================================================================//
//                                   MAIN                                    //
//===========================================================================//
int Grima::processMining( float minF, GClassDB *pClassDB )
{
  /*
   * TODO : RD
   * Copy desc
   */

  // Initialize pattern id to zero
  int minFreq      = 0;
  int returnStatus = 0;
  //firstTick    = clock();

  /*************************************************************************/
  /*   REAL STUFF BEGIN HERE                                               */
  /*************************************************************************/
  // ============ ALL MINING STUFF BEGIN HERE !!!!!
  // Begin of recursive cal
  //  int nbPattern = 0;
  //  uint lastSizePattern = vocabPattern->v_Patterns.size() ;

  if ( minF <= 1 )
    minFreq = (int) round( minF * pClassDB->v_ClassGraphs.size() );
  else
    minFreq = minF;


  returnStatus = search( pClassDB->v_ClassGraphs, pClassDB->m_TokenData, minFreq );

  sort( vocabPattern->v_Patterns.begin(), vocabPattern->v_Patterns.end(), GpatComptLt() );

  for ( uint iPat = 0; iPat < vocabPattern->v_Patterns.size(); iPat++ )
    vocabPattern->v_Patterns.at(iPat)->pGraph->graphID = iPat;

  if ( returnStatus != 0 )
  {
    cout << "#TODO : Manage PARAM & search return status" << endl;
    //    nbPattern = vocabPattern->v_Patterns.size() - lastSizePattern;
    //    lastSizePattern = vocabPattern->v_Patterns.size();
  }
  return returnStatus;
}
// End of Grima::processMining( double minF , GClassDB *pClassDB )

int Grima::search( vector<GGraph*>                   &v_Graphs,
                   map<GToken, GTokenData, GTokenGt> &m_TokenData,
                   GGlobFreq                         minFreq )
{
  /*
   * TODO : RD
   * Copy Desc
   */

  int returnStatus = 0;
  // Instanciate iterator
  map<GToken, GTokenData, GTokenGt>::iterator it = m_TokenData.begin();
  // Instanciate current pattern object
  GPattern currentPattern;
  currentPattern.pGraph->graphID   = freqPatternId;
  currentPattern.pGraph->className = "FrequentPattern";
  while ( it != m_TokenData.end() )
  {
    // For all canonical code of 1-edge store in database
    if ( it->second.freq >= minFreq )
    {
      GExtensionData tmp;
      tmp.nbOcc     = 0;
      tmp.frequency = it->second.freq;
      for ( uint iGraph = 0 ; iGraph < it->second.v_SparseOcc.size() ; iGraph++ )
        tmp.nbOcc += it->second.v_SparseOcc.at(iGraph).size;

      // Apply recursive call
      returnStatus = search( v_Graphs, minFreq, &currentPattern,
                             it->first, it->second, tmp );

      if ( returnStatus != 0 )
        return returnStatus;
    }
    // Go to next edge possible edge
    it++;
  }
  return returnStatus;
}
// End of Grima::search( vector<GGraph*>                   &v_Graphs,
//                       map<GToken, GTokenData, GTokenGt> &m_TokenData,
//                       GGlobFreq                         minFreq )

int Grima::search (vector<GGraph*> &v_Graphs,
                   GGlobFreq       &minFreq,    //Mininmum global frequency
                   GPattern*        pPattern,   //Current pattern
                   const GToken&    lastExt,    //Last extension that will be adde
                   GTokenData      &tokenData,  //Struct with freq,v_TId & sparseset
                   GExtensionData &suppData )  // Tmp variable, supposed frequency
{
  /*
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

  /*************************************************************************/
  // === First step : Add extension
  // Nb of occurence of Pattern ( To check if P is close)
  GGlobFreq nbOcc        = 0;
  // current frequency of P
  GGlobFreq currentFreq  = 0;
  // Store last graph tID to compute frequency
  bool firstOcc          = true;
  uint lastOccGraphID    = 0;
  GTid lastOccGraphMemId = NULL;

  // Add the extension to the pattern
  pPattern->push_back( lastExt, false );

  // === Second step : Check if P is canonical
  bool isCanonical = pPattern->isCanonincal();

  if ( !isCanonical )
  {
    // If extension is not canonical, we should not go deeper
    pPattern->pop_back( false );
    // Exit search. We will find this pattern later
    return 0;
  }

//  if ( PARAM.TIMELIMIT != -1
//       && ( pPattern->maxTCoord - pPattern->minTCoord > PARAM.TIMELIMIT ) )
//  {
//    // If extension is not canonical, we should not go deeper
//    pPattern->pop_back( false );
//    // Exit search. We will find this pattern later
//    return 0;
//  }

//  if ( PARAM.NODELIMIT != -1
//       && (int)pPattern->pGraph->v_Nodes.size() > PARAM.NODELIMIT )
//  {
//    // If extension is not canonical, we should not go deeper
//    pPattern->pop_back( false );
//    // Exit search. We will find this pattern later
//    return 0;
//  }

//  if ( PARAM.GAMEOFLIFE && lastExt.nodeLabelFrom == 0 && lastExt.nodeLabelDest == 0 )
//  {
//    // If extension is not canonical, we should not go deeper
//    pPattern->pop_back( false );
//    // Exit search. We will find this pattern later
//    return 0;
//  }

//  if ( PARAM.TIMEOUT != -1
//       && ( clock() - firstTick ) / CLOCKS_PER_SEC >= round(PARAM.TIMEOUT*3600) )
//  {
//    cout << " Mining timeout reached during class : " << v_Graphs.at(0)->className << endl;
//    return -1;
//  }

//  if ( PARAM.NBPATLIMIT != -1
//       && (int)vocabPattern->v_Patterns.size() > PARAM.NBPATLIMIT )
//  {
//    cout << " Limit of number of pattern reached during class : " << v_Graphs.at(0)->className << endl;
//    return -1;
//  }

  /*************************************************************************/
  // === Third step : Compute all the extension of P in the whole database
  // Object with map of possible extensions
  GExtensionCollect extCollect( minFreq );
  // Object that store pattern and occurences
  GSubgraphIso subGraphIso( pPattern, pPattern->v_OccList );
  // Clear occurence list before searching new occurency as pPattern should
  // already be stored in GVocab.
  subGraphIso.clearOccList();
  /* Number of sparse set (i.e. number of graphs in which first edge of pattern
   * is frequent */
  int nbGraphSparse = tokenData.v_SparseOcc.size();
  for ( int iGraph = 0 ; iGraph < nbGraphSparse; iGraph++ )
  {
    // For each sparseset
    // As the sparse set is modified, just create copy for the FOR LOOP
    GSparseSet SparseOccTmp = tokenData.v_SparseOcc.at(iGraph);
    // Get domain size
    uint domainSize = SparseOccTmp.size;
    for ( uint iDom = 0; iDom < domainSize  ; iDom++ )
    {
      // For each edge in the domain of the sparse set
      // Get map value of edge wearing pattern
      uint map                  = SparseOccTmp.atDom(iDom);
      GSparseSet::mapEdge mEdge = SparseOccTmp.atMap(map);
      // Find if this edge wear an occurence of the pattern
      bool findOcc = subGraphIso.run( SparseOccTmp.pGraph,
                                      mEdge.nodeFrom,
                                      mEdge.edgeId,
                                      false );
      if ( findOcc )
      {
        // If edge wears pattern
        // Increment occurences counter
        nbOcc++;
        if ( firstOcc )
        {
          firstOcc = false;
          // If first occurenc of pattern in graph
          // Increment frequency counter to 1
          currentFreq++;
          // Get graph ID of this occurence
          lastOccGraphID    = SparseOccTmp.graphID;
          lastOccGraphMemId = SparseOccTmp.pGraph;

        }
        else if ( lastOccGraphID != SparseOccTmp.graphID
                  && lastOccGraphMemId == SparseOccTmp.pGraph )
        {
          cerr << "ERROR - Error While managing graphID in sparseset" << endl;
        }
        else if ( lastOccGraphID != SparseOccTmp.graphID
                  && lastOccGraphMemId != SparseOccTmp.pGraph )
        {
          // If last occurence was in another graph
          // Increment frequency counter
          currentFreq++;
          // Get graph ID of this occurence
          lastOccGraphID    = SparseOccTmp.graphID;
          lastOccGraphMemId = SparseOccTmp.pGraph;
        }
        // Compute all extension of this occurence
        extCollect.process( subGraphIso );
      }
      else
      {
        // If edge does not wear pattern "Remove" edge from domain in the sparse set
        tokenData.v_SparseOcc.at(iGraph).remove(map) ;
      }
    }
  }

  if ( pPattern->v_Tokens.size() > 1 )
  {
    if ( currentFreq != suppData.frequency )
    {
      cerr << "Computed Frequency not the same as supposed one" << endl;
      cerr << "# Suppose freq : " << suppData.frequency << endl;
      cerr << "# Suppose occ  : " << suppData.nbOcc     << endl;
      cerr << pPattern;
      cerr << endl;
      exit( EXIT_FAILURE );
    }
    if ( nbOcc != suppData.nbOcc && pPattern->v_Tokens.at(0).angle > 0 )
    {
      cerr << "Computed occurency not the same as supposed one" << endl;
      cerr << "# Suppose freq : " << suppData.frequency << endl;
      cerr << "# Suppose occ  : " << suppData.nbOcc     << endl;
      uint i = 0;
      while ( i < suppData.v_OccList.size() )
      {
        cerr << "o "
             << suppData.v_OccList.at(i) << " "
             << suppData.v_OccList.at(i+1) << " "
             << suppData.v_OccList.at(i+2) << " " << endl;
        i+=3;
      }
      cerr << pPattern;
      cerr << endl;
      exit( EXIT_FAILURE );
    }
  }
  /*************************************************************************/
  // === Fourth step : Output pattern and occurences in output file
  // Increment pattern Id
  uint countPat = 0;
  bool find     = false;
  while ( countPat < vocabPattern->v_Patterns.size() && !find )
  {
    find = vocabPattern->comp( pPattern, vocabPattern->v_Patterns[countPat] );
    if ( !find )
      countPat++;
  }
  if ( !find )
  {
    pPattern->pGraph->graphID = freqPatternId;
    freqPatternId++;
    GPattern * tmpPat = new GPattern( *pPattern );
    tmpPat->pGraph = new GGraph( *pPattern->pGraph );
    vocabPattern->v_Patterns.push_back( tmpPat );
    //vocabPattern->createPatternMap( pPattern );
  }

  if ( PARAM.PATFILE == "" )
  {
    // Write pattern in output file
  cout << "# Suppose freq : " << suppData.frequency << endl;
  cout << "# Suppose occ  : " << suppData.nbOcc     << endl;
  cout << pPattern << endl;
  }

  /********************* Make recurive call to extend P ********************/
  int isLeaf = 1;
  for ( map<GToken, GExtensionData, GTokenGt>::iterator it
        = extCollect.m_Extensions.begin();
        it != extCollect.m_Extensions.end(); it++ )
  {
    if ( it->second.frequency >= minFreq )
    {
      // Save size of sparce set before recursive call
      vector<uint> v_SparseSizes;
      for ( uint i = 0 ; i < tokenData.v_SparseOcc.size(); i++ )
        v_SparseSizes.push_back( tokenData.v_SparseOcc.at(i).size );
      // Recursive call
      if ( it->first.angle != GNOANGLE )
        isLeaf = search( v_Graphs, minFreq, pPattern,
                         it->first, tokenData, it->second );
      // Restore size after recursive call
      if ( isLeaf == -1 || isLeaf == -2 )
        return isLeaf;

      for ( uint i = 0 ; i < v_SparseSizes.size(); i++ )
        tokenData.v_SparseOcc.at(i).setSize( v_SparseSizes.at(i) );

    }
  }
  // Restore the value of pPattern
  pPattern->pop_back( false );
  return 0;
}

