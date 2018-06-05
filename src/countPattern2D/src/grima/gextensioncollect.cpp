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
// Include project class

#include "global.hpp"
#include "gextensioncollect.hpp"

//#include "../holeG/hgrimaextensioncollect.hpp"
//#include "../holeG/hgrimagraphecode.hpp"
//#include "../holeG/hgrimaangle.hpp"

//=============================== NAMESPACE ==================================//
//============================ STRUCT & TYPEDEF ==============================//
//=============================== VARIABLES ==================================//
//================================ METHODS ===================================//

//================================= CLASS ====================================//
//---- PUBLIC --------------------------------------------------------------//
// Public CONSTANTS ________________________________________________________//
// Public Constructor/Desctructor __________________________________________//
GExtensionCollect::GExtensionCollect( GGlobFreq minFG ):
  minFreqG( minFG )
{
  // Default Constructor
  // Nothing to do here
}
// End of GExtensionCollect::GExtensionCollect( GGlobFreq minFG )

GExtensionCollect::~GExtensionCollect()
{
  // Default Destructor
  m_Extensions.clear();
}
// End of GExtensionCollect::~GExtensionCollect()

// Accessor ________________________________________________________________//
// Mutator _________________________________________________________________//
// Public Methods __________________________________________________________//
void GExtensionCollect::process( GSubgraphIso &subGraphIso )
{
  /*
   * TODO : RD
   * Copy Desc
   */

  // Get the current pattern
  GPattern *pPattern = subGraphIso.pPattern;
  // Get the nodes in the pattern
  vector<GGraphNode> &v_PatternNodes = pPattern->pGraph->v_Nodes;
  // Get the nodes of the current large graph in the database
  vector<GGraphNode> &v_LargeNodes   = subGraphIso.largeGraph->v_Nodes;
  // List of possible extensions
  list< pair<GToken, GExtensionData> > lp_ExtList;
  // Integer to know position of the edge in the list v_Edges
  uint prevEdgeFrom = 0;

  /*************** STEP ONE : COMPUTE ALL EXTENSION *************************/
  for ( uint iNode = 0 ; iNode < pPattern->pGraph->v_Nodes.size(); iNode++)
  {
    // For each node i, build a new extension if possible
    // New extension
    pair<GToken,GExtensionData> p_NewExt;

    // Get the label of the first active node
    p_NewExt.first.nodeLabelFrom = pPattern->pGraph->v_Nodes.at(iNode).label;
    p_NewExt.first.nodeFrom      = iNode;
    p_NewExt.first.direction     = gForward;

    // Get the node id from of the previous edge
    GNodeID nodeJ;
    GNodeID nodeLargeI = subGraphIso.v_LargeGraphIdx->at(subGraphIso.offset+iNode);
    GNodeID nodeLargeJ;
    if ( subGraphIso.v_PatternIdx.at(nodeLargeI) == 0 )
    {
      // If search extension from node 0 of pattern
      nodeLargeJ = subGraphIso.v_LargeGraphIdx->at(
            subGraphIso.offset+pPattern->v_Tokens.at(0).nodeDest);
    }
    else
    {
      uint iToken = 0;
      // Find previous edge
      while ( pPattern->v_Tokens.at(iToken).nodeDest != iNode )
        iToken++;
      nodeLargeJ
          = subGraphIso.v_LargeGraphIdx->at(
            subGraphIso.offset+pPattern->v_Tokens.at(iToken).nodeFrom);
    }
    prevEdgeFrom = subGraphIso.largeGraph->getEdgePositionFrom( nodeLargeI, nodeLargeJ );

    GGraphEdge edge;
    for ( uint iEdge = 0 ; iEdge < v_LargeNodes.at(nodeLargeI).v_Edges.size(); iEdge++)
    {
      // For each edge from nodeLargeI
      /* Get the next possible edge (nodeLargeI,nodeLargeJ) in the
       * current large graph in the database for the extension */
      edge = subGraphIso.largeGraph->v_Nodes.at(nodeLargeI).v_Edges.at(iEdge);

      if ( edge.destNodeId != GNOEDGEID )
      {
        // Get the dest node in the large graph for the next edge
        nodeLargeJ = edge.destNodeId;
        // Set the label in the code
        p_NewExt.first.edgeLabel = edge.label;
        // Set the destination label node in the code
        p_NewExt.first.nodeLabelDest = v_LargeNodes.at(nodeLargeJ).label;

        // Set the angle
        p_NewExt.first.angle = subGraphIso.getTokenAngleValue(
              prevEdgeFrom, nodeLargeI, nodeLargeJ );
        /*
         * TODO : RD
         * Manage prev Temporal angle
         */
        if ( pPattern->v_Tokens.at(0).angle < 0 )
        {
          if (  p_NewExt.first.angle < 0 && p_NewExt.first.angle  != GNOANGLE )
          {
            nodeJ = subGraphIso.v_PatternIdx.at(nodeLargeJ);

            if ( nodeJ == GNONODEID )
            {
              // Edge is gForward
              if ( !pPattern->pGraph->edgeExist(
                     p_NewExt.first.nodeFrom, v_PatternNodes.size() ) )
              {
                p_NewExt.first.nodeDest  = v_PatternNodes.size() ;
                p_NewExt.first.direction = gForward;
                p_NewExt.second.v_OccList.push_back(
                      subGraphIso.v_LargeGraphIdx->at(
                        subGraphIso.offset+pPattern->v_Tokens.at(0).nodeFrom ) );
                p_NewExt.second.v_OccList.push_back(
                      subGraphIso.v_LargeGraphIdx->at(
                        subGraphIso.offset+pPattern->v_Tokens.at(0).nodeDest ) );
                lp_ExtList.push_back( p_NewExt );
              }
              p_NewExt.first.nodeLabelFrom = pPattern->pGraph->v_Nodes[iNode].label;
              p_NewExt.first.nodeFrom = iNode;
              p_NewExt.first.nodeDest = GNONODEID;
              p_NewExt.second.v_OccList.clear();
              // In fact, we don't know yet. Will be used for the while loop
            }
            else if ( !pPattern->pGraph->edgeExist(
                        p_NewExt.first.nodeFrom, nodeJ ) )
            {
              cerr << "ERROR - Extension collect backward edge while extending "
                   << "pattern that start with temporal edge ! " << endl;
            }
          }
        }
        else if ( p_NewExt.first.angle != GNOANGLE )
        {
          nodeJ = subGraphIso.v_PatternIdx.at(nodeLargeJ);

          if ( nodeJ == GNONODEID )
          {
            // Edge is gForward
            if ( !pPattern->pGraph->edgeExist(
                   p_NewExt.first.nodeFrom, v_PatternNodes.size() ) )
            {
              p_NewExt.first.nodeDest  = v_PatternNodes.size() ;
              p_NewExt.first.direction = gForward;
              p_NewExt.second.v_OccList.push_back(
                    subGraphIso.v_LargeGraphIdx->at(
                      subGraphIso.offset+pPattern->v_Tokens.at(0).nodeFrom ) );
              p_NewExt.second.v_OccList.push_back(
                    subGraphIso.v_LargeGraphIdx->at(
                      subGraphIso.offset+pPattern->v_Tokens.at(0).nodeDest ) );
              lp_ExtList.push_back( p_NewExt );
            }
            p_NewExt.first.nodeLabelFrom = pPattern->pGraph->v_Nodes[iNode].label;
            p_NewExt.first.nodeFrom = iNode;
            p_NewExt.first.nodeDest = GNONODEID;
            p_NewExt.second.v_OccList.clear();
            // In fact, we don't know yet. Will be used for the while loop
          }
          else if ( ! PARAM.INDUCED )
          {
            // Edge if gBackward
            if ( !pPattern->pGraph->edgeExist(
                   p_NewExt.first.nodeFrom, nodeJ ) )
            {
              p_NewExt.first.nodeDest  = nodeJ ;
              p_NewExt.first.direction = gBackward;
              p_NewExt.second.v_OccList.push_back(
                    subGraphIso.v_LargeGraphIdx->at(
                      subGraphIso.offset+pPattern->v_Tokens.at(0).nodeFrom ) );
              p_NewExt.second.v_OccList.push_back(
                    subGraphIso.v_LargeGraphIdx->at(
                      subGraphIso.offset+pPattern->v_Tokens.at(0).nodeDest ) );
              lp_ExtList.push_back( p_NewExt );
            }
            p_NewExt.first.nodeLabelFrom = pPattern->pGraph->v_Nodes[iNode].label;
            p_NewExt.first.nodeFrom = iNode;
            p_NewExt.first.nodeDest = GNONODEID;
            p_NewExt.second.v_OccList.clear();
            // In fact, we don't know yet. Will be used for the while loop
          }
        }
        else
        {
          cerr << "ERROR - Error, extensionCollect find an edge with GNOANGLE"  << endl;
          exit( EXIT_FAILURE );
        }
      }
    }
  }

  /*************** STEP TWO : INSERT EXTENSION IN MAP ***********************/
  long int nbExt = lp_ExtList.size();

  if ( nbExt > 0 )
  {
    // If there is valid extension
    list<pair<GToken,GExtensionData> >::iterator it;
    for ( it = lp_ExtList.begin(); it != lp_ExtList.end(); it++)
    {
      // For each possible extension
      // Search in the extension already found
      //      if ( it->first.direction == gBackward )
      //        cout << "----" << endl;
      //      else
      //        cout << "!!!!" << endl;

      pair<map<GToken,GExtensionData,GTokenGt>::iterator,bool> ext
          = m_Extensions.insert( *it );
      if ( ext.second )
      {
        /* If the extension does not exist yet, thus just added in
           * the map */
        ext.first->second.frequency = 1 ;
        ext.first->second.nbOcc = 1 ;
        ext.first->second.tId = (GTid) subGraphIso.largeGraph;
        ext.first->second.v_Graphs.push_back( (GTid) subGraphIso.largeGraph );
        ext.first->second.v_OccList.clear();
        ext.first->second.v_OccList.push_back( subGraphIso.largeGraph->graphID );
        ext.first->second.v_OccList.push_back( it->second.v_OccList.at(0) );
        ext.first->second.v_OccList.push_back( it->second.v_OccList.at(1) );
      }
      else
      {
        /* Extension already belong the map, just need to update
         * occurences and frequence if this occurence is in a new
         * graph */
        ext.first->second.nbOcc++;
        ext.first->second.v_OccList.push_back( subGraphIso.largeGraph->graphID );
        ext.first->second.v_OccList.push_back( it->second.v_OccList.at(0) );
        ext.first->second.v_OccList.push_back( it->second.v_OccList.at(1) );
        if ( ext.first->second.tId != (GTid) subGraphIso.largeGraph )
        {
          ext.first->second.tId = (GTid) subGraphIso.largeGraph;
          ext.first->second.frequency++;
          ext.first->second.v_Graphs.push_back( (GTid) subGraphIso.largeGraph );

        }
      }
    }
  }
}
// End of GExtensionCollect::process( GSubgraphIso* subGraphIso )

//---- PROTECTED  ----------------------------------------------------------//
// Protected CONSTANTS _____________________________________________________//
// Protected Methods _______________________________________________________//

//---- PRIVATE -------------------------------------------------------------//
// Private CONSTANTS _______________________________________________________//
// Private Methods _________________________________________________________//

//============================== OPERATOR OVERLOAD  ==========================//

