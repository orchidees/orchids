/**
 *
 *  @file			matrix.hpp
 *  @brief          matrixPtr class
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__matrix__
#define __Orchids__matrix__

#include "Orchids.h"
#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/unordered_set.hpp>


namespace boost {
    template< typename T >
    size_t hash_value(const boost::reference_wrapper< T >& v) {
        return boost::hash_value(v.get());
    }
    template< typename T >
    bool operator==(const boost::reference_wrapper< T >& lhs, const boost::reference_wrapper< T >& rhs) {
        return lhs.get() == rhs.get();
    }
}

/**
 *  @class Matrix
 */
template<typename T>
class TArray2
{
private:
    int     m_Rows;
    int     m_Cols;
    bool    m_OwnsData;
    T       *p_Data;
    
public:
    
    // Typedefs
    typedef T value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    
    TArray2(){}
    
    /**
     *  @brief Constructor
     */
    TArray2(int rows, int cols)
    {
        if (rows * cols != 0)
        {
            p_Data = (T *)calloc(rows * cols, sizeof(T));
            //memset(p_Data, 0, sizeof(T) * rows * cols);
        }
        m_Rows = rows;
        m_Cols = cols;
        m_OwnsData = true;
    }
    
    /**
     *  @brief Construct given external data
     */
    TArray2(T* pAry, bool ownsData, int rows=1, int cols=1)
    : m_Rows(rows),
    m_Cols(cols),
    m_OwnsData(ownsData),
    p_Data(pAry)
    {
        //assert(0 != p_Data);
    }
    
    /**
     *  @brief Copy constructor
     */
    TArray2(TArray2<T> const &a)
    {
        p_Data = (T *)malloc(a.rows() * a.cols() * sizeof(T));
        std::copy(a.begin(), a.end(), p_Data);
        m_Rows = a.rows();
        m_Cols = a.cols();
        m_OwnsData = true;
    }
    
    /**
     *  @brief Copy constructor
     */
    TArray2(const boost::shared_ptr<TArray2<T> > a)
    {
        p_Data = (T *)malloc(a->rows() * a->cols() * sizeof(T));
        std::copy(a->begin(), a->end(), p_Data);
        m_Rows = a->rows();
        m_Cols = a->cols();
        m_OwnsData = true;
    }
    
    /**
     *  @brief Assignment operation
     */
    /*
    TArray2& operator = (const TArray2<T>& a)
    {
        if (size() != 0)
            free(p_Data);
        p_Data = (T *)malloc(a.rows() * a.cols() * sizeof(T));
        m_Rows = a.rows();
        m_Cols = a.cols();
        std::copy(a.begin(), a.end(), p_Data);
        m_OwnsData = true;
        return *this;
    }
    */
    /**
     *  @brief Destructor
     */
    ~TArray2()
    {
        if (m_OwnsData && (m_Rows * m_Cols) != 0) { free(p_Data); }
    }
    
    /**
     *  @brief Deallocation of p_data
     */
    void deallocate()
    {
        delete [] p_Data;
        p_Data = NULL;
    }
    
    /**
     *  @brief Set number of rows
     */
    inline void setRows(int rows) { m_Rows = rows; }
    /**
     *  @brief Set number of cols
     */
    inline void setCols(int cols) { m_Cols = cols; }
    /**
     *  @brief Set data
     */
    void setData(T *data, int size)
    {
        p_Data = (T *)malloc(size * sizeof(T));
        std::copy(&data[0], &data[size], p_Data);
    }
    /**
     *  @brief Set ownsData
     */
    inline void setOwnsData(bool ownsData) { m_OwnsData = ownsData; }

    
    /**
     *  @brief Number of rows retrieval
     */
    inline int rows() const { return m_Rows; }
    /**
     *  @brief Number of cols retrieval
     */
    inline int cols() const { return m_Cols; }
    /**
     *  @brief Size retrieval
     */
    inline int size() const { return m_Rows * m_Cols; }
    /**
     *  @brief Bytes retrieval
     */
    inline int bytes() const { return size()*sizeof(value_type); }
    
    /**
     *  @brief Check if empty matrix
     */
    inline bool isEmpty() { return size() == 0; }
    
    /**
     *  @brief Fill matrixPtr with a value
     */
    void fill(value_type val)
    {
        std::uninitialized_fill_n( p_Data, size(), val );
    }
    
    /**
     *  @brief Swap contents with another matrix
     */
    void swap(TArray2<T>& ary)
    {
        std::swap( m_Rows, ary.m_Rows );
        std::swap( m_Cols, ary.m_Cols );
        std::swap( m_OwnsData, ary.m_OwnsData );
        std::swap( p_Data, ary.p_Data );
    }

    /**
     *  @brief Resize data bank
     */
    void resizeTo(int rows, int cols, bool keepData)
    {
        // Sanity check
        int nElements = rows * cols;
        //assert( nElements > 0 );
        
        // No change, return
        if (rows == m_Rows && cols == m_Cols)
            return;
        
        // If we want to keep existing data
        if (keepData)
        {
            p_Data = (T *)realloc(p_Data, nElements * sizeof(T)); // Reallocate data bank
        }
        else
        {
            deallocate();
            p_Data = (T *)malloc(nElements * sizeof(T)); // Allocate data bank
        }
        
        m_Rows = rows;
        m_Cols = cols;
    }
    
    /**
     *  @brief Return the kth row
     */
    inline T* getRow(int k)
    {
        /*
        int i;
        //assert(k < rows());
        T *arr1d = (T *)malloc(m_Cols * sizeof(T));
        for (i = 0; i < m_Cols; i++)
            arr1d[i] = p_Data[ m_Cols * k + i ];
        
        return arr1d;
        */
        return p_Data + (m_Cols * k);
    }
    
    /**
     *  @brief Return the kth row
     */
    inline vector<T> getRowVec(int k)
    {
        int i;
        vector<T> arr1d;
        for (i = 0; i < m_Cols; i++)
            arr1d.push_back(p_Data[ m_Cols * k + i ]);
        
        return arr1d;
    }
    
    /**
     *  @brief Return the kth column
     */
    T* getCol(int k)
    {
        int i;
        //assert(k < cols());
        T *arr1d = (T *)malloc(m_Rows * sizeof(T));
        for (i = 0; i < m_Rows; i++)
            arr1d[i] = p_Data[ m_Cols * i + k ];
        
        return arr1d;
    }
    
    /**
     *  @brief Return values between row1 and row2 at the kth column
     */
    T* getCol(int k, int row1, int row2)
    {
        int i;
        T *arr1d = (T *)malloc((row2 - row1 + 1) * sizeof(T));
        int index = 0;
        for (i = row1; i <= row2; i++)
        {
            arr1d[index] = p_Data[ m_Cols * i + k ];
            index++;
        }
        
        return arr1d;
    }
    
    /**
     *  @brief Delete the ith row
     */
    void delRow(int irow)
    {
        int i, j;
        int n = rows();
        int m = cols() ;
        
        if ( n <= 1 || m == 0 ) return ;
        
        TArray2<T> B(n - 1, m) ;
        for (i = 0; i < irow; i++ )
            for (j = 0; j < m; j++)
                B(i, j) = p_Data[ m_Cols * i + j ] ;
        for (i = irow + 1; i < n; i++ )
            for (j = 0; j < m; j++)
                B(i - 1, j) = p_Data[ m_Cols * i + j ] ;
        
        resizeTo(n-1, m, false);
        for (i = 0; i < n - 1; i++ )
            for (j = 0; j < m; j++)
                p_Data[ m_Cols * i + j ] = B(i, j) ;
    }
    
    /**
     *  @brief Delete the ith column
     */
    void delCol(int icol)
    {
        int i, j;
        int n = rows();
        int m = cols() ;
        
        if ( n == 0 || m <= 1 ) return ;
        
        TArray2<T> B(n, m - 1) ;
        for (i = 0; i < n; i++ )
            for (j = 0; j < icol; j++)
                B(i, j) = p_Data[ m_Cols * i + j ] ;
        for (i = 0; i < n; i++ )
            for (j = icol+1; j < m; j++)
                B(i, j - 1) = p_Data[ m_Cols * i + j ] ;
        
        resizeTo(n, m-1, false);
        for (i = 0; i < n; i++ )
            for (j = 0; j < m-1; j++)
                p_Data[ m_Cols * i + j ] = B(i, j) ;
    }
    
    /**
     *  @brief Optimized version of merge
     */
    void merge2(boost::shared_ptr<TArray2<T> > y)
    {
        int i, j;

            if (y->rows() > 1)
            {
                //Merging a column Nx1
                //Current matrixPtr NxM
                TArray2<T> res(y->rows(), m_Cols + y->cols());
                //Copy current matrixPtr into res
                for (i = 0; i < m_Rows; i++)
                    for (j = 0; j < m_Cols; j++)
                        res(i, j) = p_Data[ m_Cols * i + j ];
                
                //Copy matrixPtr to merge into res
                for (i = 0; i < y->rows(); i++)
                    for (j = cols(); j < res.cols(); j++)
                        res(i, j) = y->operator()(i, j - m_Cols);
                
                swap(res);
            }
            else
            {
                //Merging a row 1xN or 1x1
                //Current matrixPtr MxN
                resizeTo(m_Rows + 1, y->cols(), true);
                
                //Add the row under the matrix
                for (i = m_Rows - 1; i < m_Rows; i++)
                    for (j = 0; j < m_Cols; j++)
                        p_Data[ m_Cols * i + j ] = y->operator()(i - m_Rows + 1, j);
            }
    }
    
    /**
     *  @brief Merge matrixPtr with another matrix
     */
    void mergeArray(boost::shared_ptr<TArray2<T> > m)
    {
        int i, j;
        int row = m_Rows;
        resizeTo(m_Rows + m->rows(), m_Cols, true);
        //Add the array under the matrix
        for (i = 0; i < m->rows(); i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * (row + i) + j ] = m->operator()(i, j);
    }
    
    /**
     *  @brief Add a row under the matrix
     */
    void quickmerge(T *y)
    {
        int j;
        resizeTo(m_Rows + 1, m_Cols, true);
        //Add the row under the matrix
        for (j = 0; j < m_Cols; j++)
            p_Data[ m_Cols * (m_Rows - 1) + j ] = y[j];
        
        //free(y);
    }
    
    /**
     *  @brief Return the transposed current matrix
     */
    void transpose()
    {
        int i, j;
        TArray2<T> t(m_Cols, m_Rows);
        for (i = 0; i < m_Cols; i++)
            for (j = 0; j < m_Rows; j++)
                t(i, j) =  p_Data[ m_Cols * j + i ];

        deallocate();
        swap(t);
    }
    
    /**
     *  @brief Retrieve cols between col1 and col2 of the current matrix
     */
    void resizeMatrixByCol(int col1, int col2)
    {
        int i, j;
        TArray2<T> savedValues(m_Rows, col2 - col1 + 1);
        for (i = 0; i < m_Rows; i++) {
            j = 0;
            do {
                savedValues(i, j) = p_Data[ m_Cols * i + col1 + j - 1 ];
                j++;
            } while (j <= col2 - col1);
        }
        
        deallocate();
        swap(savedValues);
    }
    
    /**
     *  @brief Retrieve rows between row1 and row2 of the current matrix
     */
    TArray2 resizeMatrixByRow(int row1, int row2)
    {
        int i, j;
        TArray2<T> savedValues(row2 - row1 + 1, m_Cols);
        for (i = row1; i <= row2; i++)
            for (j = 0; j < m_Cols; j++)
                savedValues(i - row1, j) = p_Data[ m_Cols * i + j ];
        
        return savedValues;
    }
    
    /**
     *  @brief Print datas as a matrix
     */
    void print()
    {
        int i, j;
        for (i = 0; i < m_Rows; ++i)
        {
            for (j = 0; j < m_Cols; ++j)
                cout << p_Data[ m_Cols * i + j ] << " ";
            cout << endl;
        }
    }
    
    void reorder(vector<int> &indexes)
    {
        int i, j;
        TArray2<T> res((int)indexes.size(), m_Cols);
        for (i = 0; i < res.rows(); i++)
            for (j = 0; j < m_Cols; j++)
                res(i, j) = p_Data[ m_Cols * indexes[i] + j ];
        m_Rows = res.rows();
        m_Cols = res.cols();
        //deallocate();
        setData(res.c_data(), res.size());
    }
    
    boost::shared_ptr<TArray2<T> > reorder2(vector<int> &indexes)
    {
        int i, j;
        boost::shared_ptr<TArray2<T> > res(new TArray2<T>((int)indexes.size(), m_Cols));
        for (i = 0; i < res->rows(); i++)
            for (j = 0; j < m_Cols; j++)
                res->operator()(i, j) = p_Data[ m_Cols * indexes[i] + j ];
        return res;
    }
    
    //Mathematicals functions
    /**
     *  @brief Return the sum values for elements along the dimension dim
     *  dim = 1 -> vector containing the sum value of each column
     *  dim = 2 -> vector containing the sum value of each row
     */
    T* getSum(int dim)
    {
        int i, j;
        T*  res;
        T   sum;
        switch (dim) {
            case 1:
                res = (T *)malloc(m_Cols * sizeof(T));
                for (i = 0; i < m_Cols; i++)
                {
                    sum = 0;
                    for (j = 0; j < m_Rows; j++)
                        sum += p_Data[ m_Cols * j + i ];
                    res[i] = sum;
                }
                break;
                
            case 2:
                res = (T *)malloc(m_Rows * sizeof(T));
                for (i = 0; i < m_Rows; i++)
                {
                    sum = 0;
                    for (j = 0; j < m_Cols; j++)
                        sum += p_Data[ m_Cols * i + j ];
                    res[i] = sum;
                }
                break;
                
            default:
                return 0;
                break;
        }

        return res;
    }
    
    vector<T> getSumVector(int dim)
    {
        int i, j;
        vector<T>   res;
        T           sum;
        switch (dim)
        {
            case 1:
                for (i = 0; i < m_Cols; i++)
                {
                    sum = 0;
                    for (j = 0; j < m_Rows; j++)
                        sum += p_Data[ m_Cols * j + i ];
                    res.push_back(sum);
                }
                break;
                
            case 2:
                for (i = 0; i < m_Rows; i++)
                {
                    sum = 0;
                    for (j = 0; j < m_Cols; j++)
                        sum += p_Data[ m_Cols * i + j ];
                    res.push_back(sum);
                }
                break;
        }
        return res;
    }
    
    /**
     *  @brief Return the norm of the current matrix
     */
    float norm()
    {
        int     i, j;
        float   sum = 0.0, nrm;
        
        for(i = 0; i < m_Rows; i++)
            for(j = 0; j < m_Cols; j++)
                sum += p_Data[ m_Cols * i + j ] * p_Data[ m_Cols * i + j ];

        //sqrt(parameter) function for square root
        nrm = sqrt(sum);
        
        // return float value to function call
        return nrm;
    }
    
    
    /**
     *  @brief Return the mean values for elements along the dimension dim
     *  dim = 1 -> vector containing the mean value of each column
     *  dim = 2 -> vector containing the mean value of each row
     */
    T*  getMean(int dim)
    {
        int i, j;
        T*  res;
        T   sum;
        switch (dim) {
            case 1:
                res = (T *)malloc(m_Cols * sizeof(T));
                for (i = 0; i < m_Cols; i++)
                {
                    sum = 0;
                    for (j = 0; j < m_Rows; j++)
                        sum += p_Data[ m_Cols * j + i ];
                    res[i] = sum / m_Rows;
                }
                break;
                
            case 2:
                res = (T *)malloc(m_Rows * sizeof(T));
                for (i = 0; i < m_Rows; i++)
                {
                    sum = 0;
                    for (j = 0; j < m_Cols; j++)
                        sum += p_Data[ m_Cols * i + j ];
                    res[i] = sum / m_Cols;
                }
                break;
                
            default:
                return 0;
                break;
        }
        
        return res;
    }
    
    vector<T> Min(int dim)
    {
        int i;
        vector<T> res;
        switch (dim) {
            case 1:
                for (i = 0; i < m_Cols; i++)
                {
                    T* col = getCol(i);
                    res.push_back(getMin(col, m_Rows));
                    free(col);
                }
                break;
                
            case 2:
                for (i = 0; i < m_Rows; i++)
                {
                    T* row = getRow(i);
                    res.push_back(getMin(row, m_Cols));
                    //free(row);
                }
                break;
        }
        return res;
    }
    
    void Min(int dim, vector<T>& mins, vector<int>& indexes)
    {
        int i;
        switch (dim) {
            case 1:
                for (i = 0; i < m_Cols; i++)
                {
                    T*  col = getCol(i);
                    T   min;
                    int pos;
                    getMin(col, m_Rows, min, pos);
                    mins.push_back(min);
                    indexes.push_back(pos);
                    free(col);
                }
                break;
                
            case 2:
                for (i = 0; i < m_Rows; i++)
                {
                    T*  row = getRow(i);
                    T   min;
                    int pos;
                    getMin(row, m_Cols, min, pos);
                    mins.push_back(min);
                    indexes.push_back(pos);
                    //free(row);
                }
                break;
        }
    }
    
    vector<T> Max(int dim)
    {
        int i;
        vector<T> res;
        switch (dim) {
            case 1:
                for (i = 0; i < m_Cols; i++)
                {
                    T* col = getCol(i);
                    res.push_back(getMax(col, m_Rows));
                    free(col);
                }
                break;
                
            case 2:
                for (i = 0; i < m_Rows; i++)
                {
                    T* row = getRow(i);
                    res.push_back(getMax(row, m_Cols));
                    //free(row);
                }
                break;
        }
        return res;
    }
    
    vector<int> uniqueRows()
    {
        int i, j;
        vector<int> index;
        //TArray2 to vector<vector> >
        vector<vector<T> > m(m_Rows, vector<T>(m_Cols, 0));
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                m[i][j] = p_Data[ m_Cols * i + j ];
        
        //Search unique vectors
        boost::unordered_set< boost::reference_wrapper< std::vector< T > const > > unique(m.size());
        for (BOOST_AUTO(it, m.begin()); it != m.end(); )
        {
            if (unique.insert(boost::cref(*it)).second)
            {
                index.push_back((int)(it - m.begin()));
                ++it;
            }
            else
            {
                //m(it);
                ++it;
            }
        }
        
        //vector<vector> > to TArray2
        resizeTo((int)index.size(), m_Cols, false);
        for (i = 0; i < index.size(); i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = m[index[i]][j];

        return index;
    }
    
    void sqrt_()
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = sqrt(p_Data[ m_Cols * i + j ]);
    }
    
    void floor_()
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = floor(p_Data[ m_Cols * i + j ]);
    }
    
    vector<int> isMember(boost::shared_ptr<TArray2<T> > arr2)
    {
        int i, j, k;
        vector<int> ids;
        for (i = 0; i < m_Rows; i++)
        {
            bool findRow;
            for (k = 0; k < arr2->rows(); k++)
            {
                bool equal = true;
                for (j = 0; j < m_Cols; j++)
                    if (p_Data[ m_Cols * i + j ] != arr2->operator()(k, j))
                    {
                        equal = false;
                        break;
                    }
                if (equal)
                {
                    ids.push_back(1);
                    findRow = true;
                    break;
                }
                else
                    findRow = false;
            }
            if (!findRow) ids.push_back(0);
        }
        return ids;
    }
    
    void normalize()
    {
        int i, j;
        T min = getMin(p_Data, size());
        T max = getMax(p_Data, size());
        if (min == 0 && max == 0)
            return;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = (p_Data[ m_Cols * i + j ] - min) / (max - min);
    }
    
    /**
     *  @brief Operator -
     */
    void operator - (float n)
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = p_Data[ m_Cols * i + j ] - n;
    }
    
    /**
     *  @brief Operator - between two matrices
     */
    void operator - (boost::shared_ptr<TArray2<T> > n)
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = p_Data[ m_Cols * i + j ] - n->operator()(i, j);
    }
    
    /**
     *  @brief Operator +
     */
    void operator + (float n)
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = p_Data[ m_Cols * i + j ] + n;
    }
    
    /**
     *  @brief Operator /
     */
    void operator / (float n)
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = p_Data[ m_Cols * i + j ] / n;
    }
    
    /**
     *  @brief Operator /
     */
    void operator / (boost::shared_ptr<TArray2<T> > n)
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = p_Data[ m_Cols * i + j ] / n->operator()(i, j);
    }
    
    /**
     *  @brief Operator *
     */
    void operator * (float n)
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = p_Data[ m_Cols * i + j ] * n;
    }
    
    /**
     *  @brief Operator * with another matrix. Matrices must be same dimension
     */
    void operator * (boost::shared_ptr<TArray2<T> > n)
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = p_Data[ m_Cols * i + j ] * n->operator()(i, j);
    }
    
    /**
     *  @brief Operator ^
     */
    void operator ^ (float n)
    {
        int i, j;
        for (i = 0; i < m_Rows; i++)
            for (j = 0; j < m_Cols; j++)
                p_Data[ m_Cols * i + j ] = std::pow(p_Data[ m_Cols * i + j ], n);
    }

    
    // STL style iterators
    /**
     *  @brief Const iterator begin
     */
    inline const_iterator begin() const { return p_Data; }
    /**
     *  @brief Const iterator end
     */
    inline const_iterator end() const { return p_Data + size(); }
    /**
     *  @brief Iterator begin
     */
    inline iterator begin() { return p_Data; }
    /**
     *  @brief Iterator end
     */
    inline iterator end() { return p_Data + size(); }
    
    // Array indexing operators
    /**
     *  @brief Array indexing const operator, matrix(i)
     */
    inline const T& operator () ( int i ) const { return p_Data[ i ]; }
    /**
     *  @brief Array indexing const operator, matrix(i, j)
     */
    inline const T& operator () ( int i, int j ) const { return p_Data[  m_Cols * i + j ]; }
    /**
     *  @brief Array indexing operator, matrix(i)
     */
    inline T& operator () ( int i ) { return p_Data[ i ]; }
    /**
     *  @brief Array indexing operator, matrix(i, j)
     */
    inline T& operator () ( int i, int j ) { return p_Data[ m_Cols * i + j ]; }
    
    // Get pointers to internal data
    /**
     *  @brief Get const pointer to internal data
     */
    inline const T* c_data() const { return p_Data; }
    /**
     *  @brief Get pointer to internal data
     */
    inline T* c_data() { return p_Data; }
    
private:
    
    inline int checkedIndex(int indx) const
    {
        assert( indx < size() );
        return indx;
    }
    
    inline int checkedIndex(int iRow, int jCol) const
    {
        int k = m_Cols*iRow + jCol;
        assert( k < size() );
        return k;
    }
    

private:

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & m_Rows;
        ar & m_Cols;
        ar & m_OwnsData;
        if (Archive::is_loading::value)
            p_Data = new T[m_Rows * m_Cols];
        ar & boost::serialization::make_array<T>(p_Data, m_Rows * m_Cols);
    }
    
}; // class TArray2

#endif // #if !defined(TARRAY2_INCLUDED)
