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

//================================ INCLUDE ===================================//
// Include library class
#include <vector>
#include <sstream>
#include <algorithm>
// Include prohect class
#include "global.hpp"
#include "gvocab.hpp"
#include "gsubgraphiso.hpp"

//=============================== NAMESPACE ==================================//
//============================ STRUCT & TYPEDEF ==============================//
//=============================== VARIABLES ==================================//
//================================ METHODS ===================================//

//================================= CLASS ====================================//
//---- PUBLIC --------------------------------------------------------------//
// Public CONSTANTS ________________________________________________________//
// Public Constructor/Desctructor __________________________________________//
GVocab::GVocab()
{
  // Default constructor
  // Nothing to do here
}
// End of GVocab::GVocab()

GVocab::~GVocab()
{
  // Default destructor
  for ( uint i = 0; i < v_Patterns.size(); i++ )
    delete v_Patterns.at(i);
  v_Patterns.clear();
}
// End of GVocab::~GVocab()

// Accessor ________________________________________________________________//
// Mutator _________________________________________________________________//
// Public Methods __________________________________________________________//
bool GVocab::comp( GPattern* pPat1, GPattern* pPat2 )
{
  /*
   * @brief comp
   * Method that compare 2 pattern (pPat1 and pPat2).
   * @param pPat1 ; First pattern to compare
   * @param pPat2 : Second pattern to compare
   * @return TRUE if pPat1 < pPat2, FALSE if pPat2 < pPat1
   */
  for ( uint i = 0 ; i < pPat1->v_Tokens.size(); i++)
  {
    if ( cmpGToken( pPat1->v_Tokens[i], pPat2->v_Tokens[i] ) != 0 )
      return false;
  }
  return true;
}
// End of GVocab::comp( GPattern* pPat1, GPattern* pPat2 )

void GVocab::process( GDatabase &graphDB )
{
  /*
   * @brief process
   * Method that count occurence of each pattern in v_Patterns for each
   * first possible edge m_TokenData
   * @param m_TokenData
   * @param offset
   */
  if ( PARAM.DEBUG_MODE )
    cout << "Process counting" << endl;
  GClassDB *classDB;
  clock_t ticker = clock();
  for ( uint iDB = 0; iDB < graphDB.v_GClassDB.size(); iDB++ )
  {
    classDB      = graphDB.v_GClassDB.at(iDB);
    idxCurrClass = iDB;
    for ( uint iGraph = 0; iGraph < classDB->v_ClassGraphs.size(); iGraph++ )
      classDB->v_ClassGraphs.at(iGraph)->v_NbOccPattern.resize( v_Patterns.size(), 0 );
    map<GToken,GTokenData,GTokenGt> &m_TokenData = classDB->m_TokenData;
    uint count = 0;
    for ( uint i = 0 ; i < v_Patterns.size(); i++ )
    {
      if ( PARAM.DEBUG_MODE && v_Patterns.size() > 10
           && ( i % ( v_Patterns.size() / 10 ) == 0 ) )
      {
        cout << "Pattern Counting " << count*10 << "% ("
             << i << "/" << v_Patterns.size() << ")" << endl;
        count++;
      }
      else if ( PARAM.DEBUG_MODE && v_Patterns.size() == 10 )
      {
        cout << "Pattern Counting " << count*10 << "% ("
             << i << "/" << v_Patterns.size() << ")" << endl;
        count++;
      }
      bool find = false;
      map<GToken,GTokenData,GTokenGt>::iterator it;
      it = m_TokenData.begin();
      while ( it != m_TokenData.end() && !find )
      {
        if ( cmpGToken( v_Patterns[i]->v_Tokens[0], it->first ) == 0 )
        {
          find = true;
          run( i, it );
        }
        it++;
      }
    }
  }
  cout << "## Counting time : " << (double)(clock() - ticker)/CLOCKS_PER_SEC << endl;
}
// End of GVocab::process( map<GToken,GTokenData,GTokenGt> &m_TokenData,
//                             uint offset )

void GVocab::readVocab( string filename )
{
  /**
   * This function read lines of the input graph file to construct database
   * @param : String filename
   */
  if ( v_Patterns.size() != 0 )
    for ( uint i = 0; i < v_Patterns.size(); i ++ )
      delete v_Patterns.at(i);

  // Line of the file
  vector<string> v_Tokens;
  // Integer that store graphID
  uint graphId = 0;
  // Data object
  GReader *data = new GReader( filename );
  // Read first line of type "t <tId> <tClassName>
  data->getLine( &v_Tokens );

  while ( v_Tokens[0] != "NbFreqPattern")
    data->getLine( &v_Tokens );

  uint nbFreqPattern = atoi(v_Tokens.at(2).c_str());

  if ( v_Tokens.at(1) != "0" )
  {
    while ( v_Tokens.at(0) != "p" )
    {
      // Read next line
      data->getLine( &v_Tokens );
    }

    // While file not finished
    while ( !data->finished() )
    {
      GPattern* tmpGPat = new GPattern();
      tmpGPat->pGraph->graphID = atoi(v_Tokens.at(1).c_str());
      // === Go to DFS Code
      while ( v_Tokens[0] != "c" )
      {
        data->getLine( &v_Tokens );
      }
      tmpGPat->read( data, v_Tokens );
      v_Patterns.push_back( tmpGPat );
      graphId++;
    }
    delete data;

  }
  sort( v_Patterns.begin(), v_Patterns.end(), GpatComptLt() );
  if ( nbFreqPattern != v_Patterns.size() )
    cerr << "ERROR - Nb Frequent Pattern define at the first line does not "
            "correspond to the number of read pattern." << endl;
}


vector<int> GVocab::getLineIntoIntTokens( istream& str )
{
  vector<string> v_StrToken;
  vector<int>    v_IntToken;
  string         line;
  string         cell;

  getline( str, line );

  stringstream lineStream( line );

  while( getline( lineStream, cell, ',' ) )
    v_StrToken.push_back( cell );
  // This checks for a trailing comma with no data after it.
  if ( !lineStream && cell.empty() )
    // If there was a trailing comma then add an empty element.
    v_StrToken.push_back("");

  for ( uint iCol = 0; iCol < v_StrToken.size(); iCol++ )
    v_IntToken.push_back( atoi( v_StrToken.at(iCol).c_str() ) );
  return v_IntToken;
}
// End of vector<string> getNextLineAndSplitIntoTokens(istream& str)

void GVocab::readPPEIdx( string file )
{
  ifstream stream;
  vector<int> v_IdxPPE;
  vector<GPattern*> v_AllPatterns;
  for ( uint i = 0; i < v_Patterns.size(); i++ )
    v_AllPatterns.push_back( v_Patterns.at(i) );
  if ( PARAM.DEBUG_MODE )
    cout << "Reading PPE Idx file" << endl;
  stream.open( file );
  v_IdxPPE = getLineIntoIntTokens( stream );
  stream.close();
  if ( PARAM.NBPPE > (int)v_IdxPPE.size() )
  {
    cerr << "WARNING - There is less post-process pattern that the number of pat to keep set with option -N." << endl
         << "Will take all pat." << endl;
    PARAM.NBPPE = v_IdxPPE.size();
  }
  v_Patterns.clear();
  for ( int idxPat = 0; idxPat < PARAM.NBPPE; idxPat++ )
    v_Patterns.push_back( v_AllPatterns.at( v_IdxPPE.at(idxPat) ) );
  sort( v_Patterns.begin(), v_Patterns.end(), GpatComptLt() );
}


void GVocab::saveVocab( string filename )
{
  ofstream patFile;
  patFile.open( filename );
  patFile << "#NbFreqPattern : " << v_Patterns.size() << endl;
  for ( uint iPat = 0; iPat < v_Patterns.size(); iPat++ )
    patFile << v_Patterns.at(iPat);
  patFile.close();
}

void GVocab::saveHist(string filename, vector<GClassDB *> &v_ClassDB )
{
  ofstream outHist;
  outHist.open( filename );
  GGraph graph;
  int nbOcc;
  for ( uint iClassDB = 0; iClassDB < v_ClassDB.size(); iClassDB++ )
  {
    for ( uint iGraph = 0; iGraph < v_ClassDB.at(iClassDB)->v_ClassGraphs.size(); iGraph++ )
    {
      for ( uint iPat = 0; iPat < v_Patterns.size(); iPat++ )
      {
        nbOcc = v_ClassDB.at(iClassDB)->v_ClassGraphs.at(iGraph)->v_NbOccPattern.at(iPat)
             / v_Automorphism.at(iPat);
        if ( iPat != v_Patterns.size() - 1 )
          outHist << nbOcc << ",";
        else
          outHist << nbOcc;
      }
      outHist << endl;
    }
  }
  outHist.close();
}

void GVocab::saveLbl(string filename, vector<GClassDB *> &v_ClassDB )
{
  ofstream outHist;
  outHist.open( filename );
  for ( uint iClassDB = 0; iClassDB < v_ClassDB.size(); iClassDB++ )
    for ( uint iGraph = 0; iGraph < v_ClassDB.at(iClassDB)->v_ClassGraphs.size(); iGraph++ )
      outHist << iClassDB << endl;
  outHist.close();
}

void GVocab::saveOcc( string filename )
{
  ofstream outOcc;
  outOcc.open( filename );
  outOcc << "NbFreqPattern : " << v_Patterns.size() << endl;

  for ( uint iPat = 0; iPat < v_Patterns.size(); iPat++ )
  {
    outOcc << v_Patterns.at(iPat);
    v_Patterns.at(iPat)->printOcc( outOcc );
    outOcc << "#Nb Auto : " << v_Automorphism.at(iPat) << endl;
  }
  outOcc.close();
}


void GVocab::computeAutomorphism()
{
  map<GToken,GTokenData,GTokenGt> m_TokenData;
  vector<GGraphNode> v_GraphNodes;
  GGraphNode node;
  vector<GGraphEdge> v_Edges;
  uint k;
  uint nbEdge;

  v_Automorphism.resize( v_Patterns.size(), 0);
  for ( uint i = 0; i < v_Patterns.size(); i++ )
  {
    m_TokenData.clear();
    v_GraphNodes.clear();
    v_GraphNodes = v_Patterns.at(i)->pGraph->v_Nodes;
    for ( uint j = 0; j < v_GraphNodes.size() ; j++)
    {
      // For each node of the graph
      node = v_GraphNodes.at(j);
      v_Edges = v_GraphNodes.at(j).v_Edges;
      k = 0;
      nbEdge = 4;
      if ( v_Patterns.at(i)->v_Tokens.at(0).angle < 0 )
      {
        k = 4;
        nbEdge = 6;
      }

      for ( ; k < nbEdge ; k++ )
      {
        // For each edges from node
        if ( v_Edges.at(k).destNodeId != GNONODEID )
        {
          // If the edge is not null
          pair<GToken,GTokenData> p_Data;
          p_Data.second.freq         = 0 ;
          p_Data.first.nodeFrom      = 0 ;
          p_Data.first.nodeDest      = 1 ;
          if ( v_Patterns.at(i)->v_Tokens.at(0).angle < 0 )
          {
            if ( k == 4 )
              p_Data.first.angle         = -1 ;
            else if ( k == 5 )
              p_Data.first.angle         = -2 ;
          }
          else
            p_Data.first.angle         = 0 ;
          p_Data.first.nodeLabelFrom = node.label;
          p_Data.first.edgeLabel     = v_Edges.at(k).label ;
          p_Data.first.nodeLabelDest = v_GraphNodes[v_Edges.at(k).destNodeId].label;
          p_Data.first.direction     = gForward;

          if ( p_Data.first.nodeLabelFrom >= p_Data.first.nodeLabelDest )
          {
            /* If the code is canonical in the semantic way (i.e label
             * of node from is greater or equal than label of node
             * destination
             */
            // Try to insert the edge code in the map of all edge code.
            pair<map<GToken,GTokenData,GTokenGt>::iterator,bool>
                p = m_TokenData.insert( p_Data );
            /* Create edge information that will be stored in the map of
             * the sparse set of this graph
             */
            GSparseSet::mapEdge edge;
            edge.nodeFrom = j;
            edge.nodeDest = v_GraphNodes.at(j).v_Edges.at(k).destNodeId;
            edge.edgeId   = k;
            if ( edge.nodeDest == GNONODEID )
              cout << "ERROR - Node edge dest defined in hgrimavocab ! " << endl;
            if ( p.first->second.v_SparseOcc.size() == 0
                 || p.first->second.v_SparseOcc.back().graphID
                 != v_Patterns.at(i)->pGraph->graphID )
            {
              // If there's no sparse set yet or last occurence is in another graph
              GSparseSet sparse( v_Patterns.at(i)->pGraph->graphID, v_Patterns.at(i)->pGraph );
              p.first->second.v_SparseOcc.push_back(sparse);
              p.first->second.freq++;
            }
            // Put edge in map and identifiant in domain
            p.first->second.v_SparseOcc.back().add(edge);
          }
        }
      }
    }
    map<GToken,GTokenData,GTokenGt>::iterator it;
    it = m_TokenData.begin();
    while ( it != m_TokenData.end() )
    {
      if ( cmpGToken( v_Patterns[i]->v_Tokens[0], it->first ) == 0 )
      {
        vector<GNodeID> v_OccList;
        GPattern * pTmpPat = v_Patterns[i];
        GSubgraphIso subGraphIso( pTmpPat, &v_OccList );
        /* Number of sparse set (i.e. number of graphs in which first edge of pattern
       * is frequent */
        for ( uint iSparse = 0 ; iSparse  <  it->second.v_SparseOcc.size(); iSparse ++ )
        {
          // For each sparseset
          // As the sparse set is modified, just create copy for the FOR LOOP
          GSparseSet SparseOccTmp = it->second.v_SparseOcc.at(iSparse);
          //cout << "--> Occ " << k << " / " << it->second.v_SparseOcc.size() << endl;
          // Get domain size
          uint domSize = SparseOccTmp.size;
          for ( uint j = 0; j < domSize  ; ++j )
          {
            // For each edge in the domain of the sparse set
            // Get map value of edge wearing pattern
            uint map = SparseOccTmp.atDom(j);
            // Find if this edge wear an occurence of the pattern
            bool findOcc = subGraphIso.run( SparseOccTmp.pGraph,
                                            SparseOccTmp.atMap(map).nodeFrom,
                                            SparseOccTmp.atMap(map).edgeId,
                                            true );

            if ( findOcc )
            {
              v_Automorphism.at(i)++;
            }
          }
        }
      }
      it++;
    }
  }
}

//---- PROTECTED  ----------------------------------------------------------//
// Protected CONSTANTS _____________________________________________________//
// Protected Methods _______________________________________________________//

//---- PRIVATE -------------------------------------------------------------//
// Private CONSTANTS _______________________________________________________//
// Private Methods _________________________________________________________//
void GVocab::run( uint i, map<GToken,GTokenData,GTokenGt>::iterator it )
{
  /*
   * @brief run
   * Private methods used by process that actually run the ismorphisms test
   * for the pattern i and for each canonical first edge in the sparseSet that
   * are stored in object GTokenData in the iterator
   * @param i : ID of the pattern to count
   * @param it : iterator that allow access to canonical first edge sparseset
   * @param offset
   */

  vector<GNodeID> v_OccList;
  GPattern * pTmpPat = v_Patterns[i];
  // For each extension of P
  GSubgraphIso subGraphIso( pTmpPat, &v_OccList );
  /* Number of sparse set (i.e. number of graphs in which first edge of pattern
   * is frequent */
  for ( uint k = 0 ; k <  it->second.v_SparseOcc.size(); k++ )
  {
    // For each sparseset
    // As the sparse set is modified, just create copy for the FOR LOOP
    GSparseSet SparseOccTmp = it->second.v_SparseOcc.at(k);
    //cout << "--> Occ " << k << " / " << it->second.v_SparseOcc.size() << endl;
    // Get domain size
    uint domSize = SparseOccTmp.size;
    for ( uint j = 0; j < domSize  ; ++j )
    {
      // For each edge in the domain of the sparse set
      // Get map value of edge wearing pattern
      uint map = SparseOccTmp.atDom(j);
      // Find if this edge wear an occurence of the pattern
      bool findOcc = subGraphIso.run( SparseOccTmp.pGraph,
                                      SparseOccTmp.atMap(map).nodeFrom,
                                      SparseOccTmp.atMap(map).edgeId,
                                      false );

      if ( findOcc )
      {
        it->second.v_SparseOcc.at(k).pGraph->v_NbOccPattern[i]++;
      }
    }
  }
  for ( uint iOcc = 0; iOcc < subGraphIso.v_LargeGraphIdx->size()
        && subGraphIso.v_LargeGraphIdx->at(iOcc) != GNONODEID; )
  {
    pTmpPat->v_OccList->push_back(idxCurrClass);
    pTmpPat->v_OccList->push_back( subGraphIso.v_LargeGraphIdx->at(iOcc));
    iOcc++;
    for ( uint iNode = 0; iNode < pTmpPat->pGraph->v_Nodes.size(); iNode++ )
    {
      pTmpPat->v_OccList->push_back( subGraphIso.v_LargeGraphIdx->at(iOcc) );
      iOcc++;
    }
  }
}
// End of GVocab::run( uint i, map<GToken,GTokenData,GTokenGt>::iterator it )

//============================== OPERATOR OVERLOAD  ==========================//
