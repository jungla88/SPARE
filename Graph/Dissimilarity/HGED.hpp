//  HGED class, part of the SPARE library.
//  Copyright (C) 2011 Guido Del Vescovo
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

/** @brief File HGED.hpp, that contains the GED algorithm based on the Hungarian assignment algorithm.
 *
 * Contains the declaration and the implementation of the (weighted) HGED class.
 *
 * @file HGED.hpp
 * @author Lorenzo Livi
 */

#ifndef HGED_HPP
#define HGED_HPP

// BOOST INCLUDES
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>

//STD INCLUDES
#include <list>
#include <utility>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <cmath>


namespace spare {


///////////////////// MATRIX AUXILIARY CLASS

template <class T>
class Matrix {
public:
    Matrix();
    Matrix(int rows, int columns);
    Matrix(const Matrix<T> &other);
    Matrix<T> & operator= (const Matrix<T> &other);
    ~Matrix();
    // all operations except product modify the matrix in-place.
    void resize(int rows, int columns);
    void identity(void);
    void clear(void);
    T& operator () (int x, int y);
    T trace(void);
    Matrix<T>& transpose(void);
    Matrix<T> product(Matrix<T>& other);
    inline int minsize(void) {
        return ((m_rows < m_columns) ? m_rows : m_columns);
    }
    inline int columns(void) {
        return m_columns;
    }
    inline int rows(void) {
        return m_rows;
    }
private:
    T **m_matrix;
    int m_rows;
    int m_columns;
};


template <class T>
Matrix<T>::Matrix() {
    m_rows = 0;
    m_columns = 0;
    m_matrix = NULL;
}

template <class T>
Matrix<T>::Matrix(const Matrix<T> &other) {
    if ( other.m_matrix != NULL ) {
        // copy arrays
        m_matrix = NULL;
        resize(other.m_rows, other.m_columns);
        for ( int i = 0 ; i < m_rows ; i++ )
            for ( int j = 0 ; j < m_columns ; j++ )
                m_matrix[i][j] = other.m_matrix[i][j];
    } else {
        m_matrix = NULL;
        m_rows = 0;
        m_columns = 0;
    }
}

template <class T>
Matrix<T>::Matrix(int rows, int columns) {
    m_matrix = NULL;
    resize(rows, columns);
}

template <class T>
Matrix<T>& Matrix<T>::operator= (const Matrix<T> &other) {
    if ( other.m_matrix != NULL ) {
        // copy arrays
        resize(other.m_rows, other.m_columns);
        for ( int i = 0 ; i < m_rows ; i++ )
            for ( int j = 0 ; j < m_columns ; j++ )
                m_matrix[i][j] = other.m_matrix[i][j];
    } else {
        // free arrays
        for ( int i = 0 ; i < m_columns ; i++ )
            delete [] m_matrix[i];

        delete [] m_matrix;

        m_matrix = NULL;
        m_rows = 0;
        m_columns = 0;
    }

    return *this;
}

template <class T>
Matrix<T>::~Matrix() {
    if ( m_matrix != NULL ) {
        // free arrays
        for ( int i = 0 ; i < m_rows ; i++ )
            delete [] m_matrix[i];

        delete [] m_matrix;
    }
    m_matrix = NULL;
}

template <class T>
void Matrix<T>::resize(int rows, int columns) {
    if ( m_matrix == NULL ) {
        // alloc arrays
        m_matrix = new T*[rows]; // rows
        for ( int i = 0 ; i < rows ; i++ )
            m_matrix[i] = new T[columns]; // columns

        m_rows = rows;
        m_columns = columns;
        clear();
    } else {
        // save array pointer
        T **new_matrix;
        // alloc new arrays
        new_matrix = new T*[rows]; // rows
        for ( int i = 0 ; i < rows ; i++ ) {
            new_matrix[i] = new T[columns]; // columns
            for ( int j = 0 ; j < columns ; j++ )
                new_matrix[i][j] = 0;
        }

        // copy data from saved pointer to new arrays
        int minrows = std::min<int>(rows, m_rows);
        int mincols = std::min<int>(columns, m_columns);
        for ( int x = 0 ; x < minrows ; x++ )
            for ( int y = 0 ; y < mincols ; y++ )
                new_matrix[x][y] = m_matrix[x][y];

        // delete old arrays
        if ( m_matrix != NULL ) {
            for ( int i = 0 ; i < m_rows ; i++ )
                delete [] m_matrix[i];

            delete [] m_matrix;
        }

        m_matrix = new_matrix;
    }

    m_rows = rows;
    m_columns = columns;
}

template <class T>
void Matrix<T>::identity() {
    assert( m_matrix != NULL );

    clear();

    int x = std::min<int>(m_rows, m_columns);
    for ( int i = 0 ; i < x ; i++ )
        m_matrix[i][i] = 1;
}

template <class T>
void Matrix<T>::clear() {
    assert( m_matrix != NULL );

    for ( int i = 0 ; i < m_rows ; i++ )
        for ( int j = 0 ; j < m_columns ; j++ )
            m_matrix[i][j] = 0;
}

template <class T>
T Matrix<T>::trace() {
    assert( m_matrix != NULL );

    T value = 0;

    int x = std::min<int>(m_rows, m_columns);
    for ( int i = 0 ; i < x ; i++ )
        value += m_matrix[i][i];

    return value;
}

template <class T>
Matrix<T>& Matrix<T>::transpose() {
    assert( m_rows > 0 );
    assert( m_columns > 0 );

    int new_rows = m_columns;
    int new_columns = m_rows;

    if ( m_rows != m_columns ) {
        // expand matrix
        int m = std::max<int>(m_rows, m_columns);
        resize(m,m);
    }

    for ( int i = 0 ; i < m_rows ; i++ ) {
        for ( int j = i+1 ; j < m_columns ; j++ ) {
            T tmp = m_matrix[i][j];
            m_matrix[i][j] = m_matrix[j][i];
            m_matrix[j][i] = tmp;
        }
    }

    if ( new_columns != new_rows ) {
        // trim off excess.
        resize(new_rows, new_columns);
    }

    return *this;
}

template <class T>
Matrix<T> Matrix<T>::product(Matrix<T> &other) {
    assert( m_matrix != NULL );
    assert( other.m_matrix != NULL );
    assert ( m_columns == other.m_rows );

    Matrix<T> out(m_rows, other.m_columns);

    for ( int i = 0 ; i < out.m_rows ; i++ ) {
        for ( int j = 0 ; j < out.m_columns ; j++ ) {
            for ( int x = 0 ; x < m_columns ; x++ ) {
                out(i,j) += m_matrix[i][x] * other.m_matrix[x][j];
            }
        }
    }

    return out;
}

template <class T>
inline T& Matrix<T>::operator ()(int x, int y) {
    assert ( x >= 0 );
    assert ( y >= 0 );
    assert ( x < m_rows );
    assert ( y < m_columns );
    assert ( m_matrix != NULL );
    return m_matrix[x][y];
}

//////////////////////// ASSIGNMENT ALGO IMPL.

//Algorithm implemented by John Weaver
class Munkres {
public:
    void solve(Matrix<double> &m);
private:
    static const int NORMAL = 0;
    static const int STAR = 1;
    static const int PRIME = 2;
    inline bool find_uncovered_in_matrix(double,int&,int&);
    inline bool pair_in_list(const std::pair<int,int> &, const std::list<std::pair<int,int> > &);
    int step1(void);
    int step2(void);
    int step3(void);
    int step4(void);
    int step5(void);
    int step6(void);
    Matrix<int> mask_matrix;
    Matrix<double> matrix;
    bool *row_mask;
    bool *col_mask;
    int saverow, savecol;
};


bool Munkres::find_uncovered_in_matrix(double item, int &row, int &col) {
  for ( row = 0 ; row < matrix.rows() ; row++ )
    if ( !row_mask[row] )
      for ( col = 0 ; col < matrix.columns() ; col++ )
        if ( !col_mask[col] )
          if ( matrix(row,col) == item )
            return true;

  return false;
}

bool Munkres::pair_in_list(const std::pair<int,int> &needle, const std::list<std::pair<int,int> > &haystack) {
  for ( std::list<std::pair<int,int> >::const_iterator i = haystack.begin() ; i != haystack.end() ; i++ ) {
    if ( needle == *i )
      return true;
  }

  return false;
}

int Munkres::step1(void) {
  for ( int row = 0 ; row < matrix.rows() ; row++ )
    for ( int col = 0 ; col < matrix.columns() ; col++ )
      if ( matrix(row,col) == 0 ) {
        bool isstarred = false;
        for ( int nrow = 0 ; nrow < matrix.rows() ; nrow++ )
          if ( mask_matrix(nrow,col) == STAR ) {
            isstarred = true;
            break;
          }

        if ( !isstarred ) {
          for ( int ncol = 0 ; ncol < matrix.columns() ; ncol++ )
            if ( mask_matrix(row,ncol) == STAR ) {
              isstarred = true;
              break;
            }
        }

        if ( !isstarred ) {
          mask_matrix(row,col) = STAR;
        }
      }

  return 2;
}

int Munkres::step2(void) {
  int rows = matrix.rows();
  int cols = matrix.columns();
  int covercount = 0;
  for ( int row = 0 ; row < rows ; row++ )
    for ( int col = 0 ; col < cols ; col++ )
      if ( mask_matrix(row,col) == STAR ) {
        col_mask[col] = true;
        covercount++;
      }

  int k = matrix.minsize();

  if ( covercount >= k ) {
      return 0;
  }

  return 3;
}

int Munkres::step3(void) {
  /*
  Main Zero Search

   1. Find an uncovered Z in the distance matrix and prime it. If no such zero exists, go to Step 5
   2. If No Z* exists in the row of the Z', go to Step 4.
   3. If a Z* exists, cover this row and uncover the column of the Z*. Return to Step 3.1 to find a new Z
  */
  if ( find_uncovered_in_matrix(0, saverow, savecol) ) {
    mask_matrix(saverow,savecol) = PRIME; // prime it.
  } else {
    return 5;
  }

  for ( int ncol = 0 ; ncol < matrix.columns() ; ncol++ )
    if ( mask_matrix(saverow,ncol) == STAR ) {
      row_mask[saverow] = true; //cover this row and
      col_mask[ncol] = false; // uncover the column containing the starred zero
      return 3; // repeat
    }

  return 4; // no starred zero in the row containing this primed zero
}

int Munkres::step4(void) {
  int rows = matrix.rows();
  int cols = matrix.columns();

  std::list<std::pair<int,int> > seq;
  // use saverow, savecol from step 3.
  std::pair<int,int> z0(saverow, savecol);
  std::pair<int,int> z1(-1,-1);
  std::pair<int,int> z2n(-1,-1);
  seq.insert(seq.end(), z0);
  int row, col = savecol;
  /*
  Increment Set of Starred Zeros

   1. Construct the ``alternating sequence'' of primed and starred zeros:

         Z0 : Unpaired Z' from Step 4.2
         Z1 : The Z* in the column of Z0
         Z[2N] : The Z' in the row of Z[2N-1], if such a zero exists
         Z[2N+1] : The Z* in the column of Z[2N]

      The sequence eventually terminates with an unpaired Z' = Z[2N] for some N.
  */
  bool madepair;
  do {
    madepair = false;
    for ( row = 0 ; row < rows ; row++ )
      if ( mask_matrix(row,col) == STAR ) {
        z1.first = row;
        z1.second = col;
        if ( pair_in_list(z1, seq) )
          continue;

        madepair = true;
        seq.insert(seq.end(), z1);
        break;
      }

    if ( !madepair )
      break;

    madepair = false;

    for ( col = 0 ; col < cols ; col++ )
      if ( mask_matrix(row,col) == PRIME ) {
        z2n.first = row;
        z2n.second = col;
        if ( pair_in_list(z2n, seq) )
          continue;
        madepair = true;
        seq.insert(seq.end(), z2n);
        break;
      }
  } while ( madepair );

  for ( std::list<std::pair<int,int> >::iterator i = seq.begin() ;
      i != seq.end() ;
      i++ ) {
    // 2. Unstar each starred zero of the sequence.
    if ( mask_matrix(i->first,i->second) == STAR )
      mask_matrix(i->first,i->second) = NORMAL;

    // 3. Star each primed zero of the sequence,
    // thus increasing the number of starred zeros by one.
    if ( mask_matrix(i->first,i->second) == PRIME )
      mask_matrix(i->first,i->second) = STAR;
  }

  // 4. Erase all primes, uncover all columns and rows,
  for ( int row = 0 ; row < mask_matrix.rows() ; row++ )
    for ( int col = 0 ; col < mask_matrix.columns() ; col++ )
      if ( mask_matrix(row,col) == PRIME )
        mask_matrix(row,col) = NORMAL;

  for ( int i = 0 ; i < rows ; i++ ) {
    row_mask[i] = false;
  }

  for ( int i = 0 ; i < cols ; i++ ) {
    col_mask[i] = false;
  }

  // and return to Step 2.
  return 2;
}

int Munkres::step5(void) {
  int rows = matrix.rows();
  int cols = matrix.columns();
  /*
  New Zero Manufactures

   1. Let h be the smallest uncovered entry in the (modified) distance matrix.
   2. Add h to all covered rows.
   3. Subtract h from all uncovered columns
   4. Return to Step 3, without altering stars, primes, or covers.
  */
  double h = 0;
  for ( int row = 0 ; row < rows ; row++ ) {
    if ( !row_mask[row] ) {
      for ( int col = 0 ; col < cols ; col++ ) {
        if ( !col_mask[col] ) {
          if ( (h > matrix(row,col) && matrix(row,col) != 0) || h == 0 ) {
            h = matrix(row,col);
          }
        }
      }
    }
  }

  for ( int row = 0 ; row < rows ; row++ )
    if ( row_mask[row] )
      for ( int col = 0 ; col < cols ; col++ )
        matrix(row,col) += h;

  for ( int col = 0 ; col < cols ; col++ )
    if ( !col_mask[col] )
      for ( int row = 0 ; row < rows ; row++ )
        matrix(row,col) -= h;

  return 3;
}

void Munkres::solve(Matrix<double> &m) {
  // Linear assignment problem solution
  // [modifies matrix in-place.]
  // matrix(row,col): row major format assumed.

  // Assignments are remaining 0 values
  // (extra 0 values are replaced with -1)

  double highValue = 0;
  for ( int row = 0 ; row < m.rows() ; row++ ) {
    for ( int col = 0 ; col < m.columns() ; col++ ) {
      if ( m(row,col) != INFINITY && m(row,col) > highValue )
        highValue = m(row,col);
    }
  }
  highValue++;

  for ( int row = 0 ; row < m.rows() ; row++ )
    for ( int col = 0 ; col < m.columns() ; col++ )
      if ( m(row,col) == INFINITY )
        m(row,col) = highValue;

  bool notdone = true;
  int step = 1;

  this->matrix = m;
  // STAR == 1 == starred, PRIME == 2 == primed
  mask_matrix.resize(matrix.rows(), matrix.columns());

  row_mask = new bool[matrix.rows()];
  col_mask = new bool[matrix.columns()];
  for ( int i = 0 ; i < matrix.rows() ; i++ ) {
    row_mask[i] = false;
  }

  for ( int i = 0 ; i < matrix.columns() ; i++ ) {
    col_mask[i] = false;
  }

  while ( notdone ) {
    switch ( step ) {
      case 0:
        notdone = false;
        break;
      case 1:
        step = step1();
        break;
      case 2:
        step = step2();
        break;
      case 3:
        step = step3();
        break;
      case 4:
        step = step4();
        break;
      case 5:
        step = step5();
        break;
    }
  }

  // Store results
  for ( int row = 0 ; row < matrix.rows() ; row++ )
    for ( int col = 0 ; col < matrix.columns() ; col++ )
      if ( mask_matrix(row,col) == STAR )
        matrix(row,col) = 0;
      else
        matrix(row,col) = -1;

  m = matrix;

  delete [] row_mask;
  delete [] col_mask;
}





/** @brief HGED dissimilarity measure for labeled graphs.
 *
 * This class implements the @a Dissimilarity concept for labeled graphs.
 * This class contains the algorithm for the computation of the dissimilarity of two arbitrary-labeled graphs following the Graph Edit Distance (GED) algorithmic scheme.
 * The matching method is based on the assignment problem, which is solved optimally by means of the well-known Hungarian method.
 * The computational complexity of the adopted Hungarian method is cubic in the order of the input graphs.
 * The GED weighting scheme is based on three weights, like the TWEC algorithm (insertions and deletions are considered to be equivalent).
 */
template <class VerticesDiss, class EdgesDiss>
class HGED {
public:

    /**
     * Default constructor.
     */
    HGED()
    {
        //vertices
        //default weighting factors
        mPAlpha=0.5;
        mPBeta=0.5;
        mPGamma=0.5;
        //default vertex/edge operation cost (insertion or deletion)
        mVertexOPCost=1;
        mEdgeOPCost=1;
    }

    /**
     * Hash Set Type
     */
    typedef boost::unordered_set<NaturalType> HashSetType;

    /**
     * hash Map Type
     */
    typedef boost::unordered_map<NaturalType, NaturalType> HashMapType;

    /**
     * Vertex dissimilarity type
     */
    typedef VerticesDiss VerticesDissType;

    /**
     * Edge dissimilarity type
     */
    typedef EdgesDiss EdgesDissType;


    /**
     * Main graph dissimilarity method
     * @param[in] g1 The first graph
     * @param[in] g2 The second graph
     * @return The dissimilarity value
     */
    template<class BoostGraphType >
    RealType Diss(const BoostGraphType& g1, const BoostGraphType& g2) const;


//ACCESS

    /**
     * Real/Write access to the alpha weighting parameter.
     */
    RealType& PAlpha() { return mPAlpha; }

    /**
     * Real-only access to the alpha weighting parameter.
     */
    const RealType& PAlpha() const { return mPAlpha; }

    /**
     * Real/Write access to the beta weighting parameter.
     */
    RealType& PBeta() { return mPBeta; }

    /**
     * Real-only access to the beta weighting parameter.
     */
    const RealType& PBeta() const { return mPBeta; }

    /**
     * Real/Write access to the gamma weighting parameter.
     */
    RealType& PGamma() { return mPGamma; }

    /**
     * Real-only access to the gamma weighting parameter.
     */
    const RealType& PGamma() const { return mPGamma; }

    /**
     * Real/Write access to the vertices dissimilarity agent.
     */
    VerticesDiss& VerticesDissAgent() { return mVerticesDissimilarity; }

    /**
     * Real-only access to the vertices dissimilarity agent.
     */
    const VerticesDiss& VerticesDissAgent() const { return mVerticesDissimilarity; }

    /**
     * Real/Write access to the edges dissimilarity agent.
     */
    EdgesDiss& EdgesDissAgent() { return mEdgesDissimilarity; }

    /**
     * Real-only access to the edges dissimilarity agent.
     */
    const EdgesDiss& EdgesDissAgent() const { return mEdgesDissimilarity; }


private:

    /**
     * Constant operation cost for vertices (this value is fixed)
     */
    RealType mVertexOPCost;

    /**
     * Constant operation cost for edges (this value is fixed)
     */
    RealType mEdgeOPCost;

    /**
     * Alpha weighting parameter
     */
    RealType mPAlpha;

    /**
     * Beta weighting parameter
     */
    RealType mPBeta;

    /**
     * Gamma weighting parameter
     */
    RealType mPGamma;

    /**
     * vertices dissimilarity function
     */
    VerticesDiss mVerticesDissimilarity;

    /**
     * edges dissimilarity function
     */
    EdgesDiss mEdgesDissimilarity;
};


//IMPLEMENTATION


template <class VerticesDiss, class EdgesDiss>
template <class BoostGraphType >
RealType HGED<VerticesDiss, EdgesDiss>::Diss(const BoostGraphType& g1, const BoostGraphType& g2) const
{
    //some boost types
    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;
    typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor BoostVertexDesc;
    typedef typename boost::graph_traits<BoostGraphType>::out_edge_iterator BoostOutEdgeIter;
    typedef typename boost::graph_traits<BoostGraphType>::edge_descriptor BoostEdgeDesc;

    //vars
    RealType totESubsCost= 0., totEOPCost=0., totVSubsCost= 0., totVOPCost=0., CND=0., CN=0., CED=0., CE=0.,
            minDiss=0., absOrderDiff, totDiss=0., vertexDiss=0.;
    NaturalType countG1=0, indexG1=0, row=0, col=0,
                orderG1=boost::num_vertices(g1), orderG2=boost::num_vertices(g2),
                sizeG1=boost::num_edges(g1), sizeG2=boost::num_edges(g2), assignedIndexG2,
                edgesOPCount=0, ESubsCount=0, ETotOPCount=0, edgesIndex1, edgesIndex2;


    //vertices and iterators/descriptors for graphs
    BoostVertexIter itG1=boost::vertices(g1).first;
    BoostVertexIter itG1End=boost::vertices(g1).second;
    BoostVertexIter itG2, itG2End;
    BoostVertexDesc v1G1, v2G1, v1G2, v2G2;
    BoostEdgeDesc edgeDescG1, edgeDescG2;

    //vertices assignments
    HashMapType assignments;
    HashMapType::iterator assIt, assIt2;
    //cost matrix
    Matrix<RealType> matrix(orderG1, orderG2);


    //absolute difference between the two orders (cost for vertex insertion/deletion)
    absOrderDiff=std::abs(static_cast<RealType>(orderG1)-static_cast<RealType>(orderG2));


    //compute the matrix of costs (vertex dissimilarity values)
    while(itG1!=itG1End)
    {
        col=0;
        indexG1=(NaturalType)*itG1;
        //vertices iterators for the second graph
        itG2=boost::vertices(g2).first;
        itG2End=boost::vertices(g2).second;

        while(itG2!=itG2End)
        {

            vertexDiss=mVerticesDissimilarity.Diss(boost::get(v_info_t(), g1, boost::vertex(*itG1, g1)),
                    boost::get(v_info_t(), g2, boost::vertex(*itG2, g2)) );

            //add into the cost matrix structure...
            matrix(row, col)=vertexDiss;

            col++;
            itG2++;
        }

        row++;
        itG1++;
    }

    //compute the optimal vertex assignment by means of the Hungarian algorithm
    Munkres m;
    m.solve(matrix);

    row=0;
    col=0;

    //define the assignments
    for(;row<orderG1;row++)
    {
        for(col=0;col<orderG2;col++)
        {
            if(matrix(row, col)==0)
            {
                //assign vertex row of G1 to vertex col of G2
                assignments[row]=col;
                break;
            }
        }
    }

    //additional constant operations
    totVOPCost=mVertexOPCost*absOrderDiff;

    //normalized vertex operations and substitution costs (the input graphs are assumed to contain at least one vertex)
    CND=( (orderG1+orderG2)-(2*std::min(orderG1, orderG2)) )/(orderG1+orderG2);
    //normalized vertex substitution cost
    CN=totVSubsCost/(RealType)std::min(orderG1, orderG2);


    /////////////////////////////
    //induced edit operations on edges
    /////////////////////////////

    assIt=assignments.begin();
    edgesIndex1=0;
    while(assIt!=assignments.end())
    {
        v1G1=boost::vertex((*assIt).first, g1);
        v1G2=boost::vertex((*assIt).second, g2);
        assIt2=assignments.begin();
        edgesIndex2=0;

        while(assIt2!=assignments.end())
        {
            bool edgeFoundG2=false, edgeFoundG1=false;

            //check condition
            if(assIt!=assIt2&&edgesIndex2<=edgesIndex1)
            {
                v2G1=boost::vertex((*assIt2).first, g1);
                v2G2=boost::vertex((*assIt2).second, g2);

                //check the edge on G1
                if(boost::edge(v1G1, v2G1, g1).second)
                {
                    edgeDescG1=boost::edge(v1G1, v2G1, g1).first;
                    edgeFoundG1=true;
                }

                //check the edge on G2
                if(boost::edge(v1G2, v2G2, g2).second)
                {
                    edgeDescG2=boost::edge(v1G2, v2G2, g2).first;
                    edgeFoundG2=true;
                }

                //edges operations

                //substitution
                if(edgeFoundG1&&edgeFoundG2)
                {
                    totESubsCost+=mEdgesDissimilarity.Diss(
                            boost::get(e_info_t(), g1, edgeDescG1),
                                boost::get(e_info_t(), g2, edgeDescG2) );
                    ESubsCount++;
                }
                //edge operation
                else if(edgeFoundG1||edgeFoundG2)
                {
                    edgesOPCount++;
                    ETotOPCount++;
                }
            }//IF

            assIt2++;
            edgesIndex2++;
        }

        assIt++;
        edgesIndex1++;
    }

    //edges operation
    totEOPCost=mEdgeOPCost*edgesOPCount;

    //normalized vertices operations and substitutions cost
    CE=(ESubsCount==0)?0:totESubsCost/(RealType)ESubsCount;
    RealType sizeTot=sizeG1+sizeG2;
    CED=(sizeTot==0)?0:(RealType)ETotOPCount/sizeTot;

    //return the final dissimilarity value
    return (1.0-mPAlpha)*( ((1.0-mPBeta)*CN) + ( mPBeta*(( (1.0-mPGamma)*CE) + (mPGamma*CED))) )+mPAlpha*CND;
}

}

#endif
