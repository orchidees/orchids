/**
 *
 *  @file			math_utils.hpp
 *  @brief          Mathematical utility functions
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__math_utils__
#define __Orchids__math_utils__

#include "Orchids.h"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

template <typename T, size_t N>
T* begin(T(&arr)[N]) { return &arr[0]; }
template <typename T, size_t N>
T* end(T(&arr)[N]) { return &arr[0] + N; }

template <typename T>
bool IsInBounds(const T& value, const T& low, const T& high)
{
    return !(value < low) && !(high < value);
}


/**
 *  @brief Comparison struct used by sort
 */
template<typename T> struct index_cmp
{
    index_cmp(const T arr) : arr(arr) {}
    bool operator()(const int a, const int b) const
    {
        return arr[a] < arr[b];
    }
    const T arr;
};

/**
 *  @brief Act like matlab's Y = X[I] where I contains a vector of indices so that after, Y[j] = X[I[j]] for index j this implies that Y.size() == I.size().
 *   X and Y are allowed to be the same reference.
 *   This implementation is O(n), but also uses O(n) extra memory
 */
template<typename T>
void reorder(
                    vector<T> & unordered,
                    vector<int> const & index_map,
                    vector<T> & ordered)
{
    int i;
    // copy for the reorder according to index_map, because unsorted may also be sorted
    vector<T> copy = unordered;
    ordered.resize(index_map.size());
    for (i = 0; i < index_map.size();i++)
        ordered[i] = copy[index_map[i]];
}

/**
 *  @brief Same as the vector reorder function but for arrays
 */
template<typename T>
void reorder(
                    T* &unordered,
                    int size,
                    vector<int> const & index_map,
                    T* &ordered)
{
    int i;
    // copy for the reorder according to index_map, because unsorted may also be sorted
    T *copy = (T *)malloc(size * sizeof(T));
    std::copy(unordered, unordered + size, copy);
    free(unordered);
    ordered = (T*)malloc(index_map.size() * sizeof(T));
    
    for (i = 0; i < index_map.size(); i++)
    {
        //if (index_map[i] < size)
            ordered[i] = copy[index_map[i]];
    }
    free(copy);
}

/**
 *  @brief Act like matlab's [Y,I] = SORT(X)
 *  @param unsorted Unsorted vector
 *  @param sorted Sorted vector, allowed to be same as unsorted
 *  @param index_map An index map such that sorted[i] = unsorted[index_map[i]]
 */
template <typename T>
void mySort(
                   T* & unsorted,
                   int size,
                   T* & sorted,
                   vector<int> & index_map)
{
    int i;
    // Original unsorted index map
    index_map.resize(size);
    for (i = 0; i < size; i++)
    {
        index_map[i] = i;
    }
    // Sort the index map, using unsorted for comparison
    sort(
         index_map.begin(),
         index_map.end(),
         index_cmp<T* &>(unsorted));
    
    reorder<T>(unsorted,size,index_map,sorted);
}

/**
 *  @brief Act like matlab's [Y,I] = SORT(X)
 *  @param unsorted Unsorted vector
 *  @param sorted Sorted vector, allowed to be same as unsorted
 *  @param index_map An index map such that sorted[i] = unsorted[index_map[i]]
 */
template <typename T>
void mySort_noFree(
            T* & unsorted,
            int size,
            T* & sorted,
            vector<int> & index_map)
{
    int i;
    // Original unsorted index map
    index_map.resize(size);
    for (i = 0; i < size; i++)
    {
        index_map[i] = i;
    }
    // Sort the index map, using unsorted for comparison
    sort(
         index_map.begin(),
         index_map.end(),
         index_cmp<T* &>(unsorted));
    
    //reorder<T>(unsorted,size,index_map,sorted);
    // copy for the reorder according to index_map, because unsorted may also be sorted
//    T *copy = (T *)malloc(size * sizeof(T));
//    std::copy(unsorted, unsorted + size, copy);
//    free(unordered);
    for (i = 0; i < index_map.size(); i++)
        sorted[i] = unsorted[index_map[i]];
}

/**
 *  @brief Act like matlab's [Y,I] = SORT(X)
 *  @param unsorted Unsorted vector
 *  @param sorted Sorted vector, allowed to be same as unsorted
 *  @param index_map An index map such that sorted[i] = unsorted[index_map[i]]
 */
template <typename T>
void mySort(
            vector<T>& unsorted,
            int size,
            vector<T>& sorted,
            vector<int> & index_map)
{
    int i;
    // Original unsorted index map
    index_map.resize(size);
    for (i = 0; i < size; i++)
    {
        index_map[i] = i;
    }
    // Sort the index map, using unsorted for comparison
    sort(
         index_map.begin(),
         index_map.end(),
         index_cmp<vector<T> &>(unsorted));
    
    reorder<T>(unsorted,index_map,sorted);
}

/**
 *  @brief Version of mySort without the output index_map vector
 */
template <typename T>
void mySort(
                   T* & unsorted,
                   int size,
                   T* & sorted)
{
    int i;
    vector<int> index_map;
    // Original unsorted index map
    index_map.resize(size);
    for (i = 0; i < size; i++)
    {
        index_map[i] = i;
    }
    // Sort the index map, using unsorted for comparison
    sort(
         index_map.begin(),
         index_map.end(),
         index_cmp<T* &>(unsorted));
    
    reorder<T>(unsorted, size, index_map, sorted);
}

/**
 *  @brief Optimized sorting. Sorts into decreasing order. For decreasing order change the comparison in for-loop
 */
template<typename T>
void BubbleSort(T* num, int size)
{
    int i, j, flag = 1;    // set flag to 1 to start first pass
    int temp;             // holding variable
    int numLength = size;
    for(i = 1; (i <= numLength) && flag; i++)
    {
        flag = 0;
        for (j = 0; j < (numLength - 1); j++)
        {
            if (num[j+1] > num[j])      // ascending order simply changes to <
            {
                temp = num[j];             // swap elements
                num[j] = num[j+1];
                num[j+1] = temp;
                flag = 1;               // indicates that a swap occurred.
            }
        }
    }
    return;
}

template<typename ItemType>
void InsertionSort(ItemType* array, unsigned size)
{
    unsigned i, j;
    for (i = 1; i < size; ++i)
    {
        ItemType val = array[i];
        j = i;
        while(j > 0 && val < array[j-1])
        {
            array[j] = array[j-1];
            --j;
        }
        array[j] = val;
    }
}

template<typename ItemType>
unsigned Partition(ItemType* array, unsigned f, unsigned l, ItemType pivot)
{
    unsigned i = f-1, j = l+1;
    while(true)
    {
        // Dirty fix !
        while(pivot < array[--j])
            if (j == 0)
                break;
        // Dirty
        while(array[++i] < pivot);
        if(i<j)
        {
            ItemType tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
        }
        else
            return j;
    }
}

template<typename ItemType>
void MedianHybridQuickSortImpl(ItemType* array, unsigned f, unsigned l)
{
    while(f+16 < l)
    {
        ItemType v1 = array[f], v2 = array[l], v3 = array[(f+l)/2];
        ItemType median =
        v1 < v2 ?
        ( v3 < v1 ? v1 : std::min(v2, v3)
         ) :
        ( v3 < v2 ? v2 : std::min(v1, v3)
         );
        unsigned m = Partition(array, f, l, median);
        MedianHybridQuickSortImpl(array, f, m);
        f = m+1;
    }
}

template<typename ItemType>
void MedianHybridQuickSort(ItemType* array, unsigned size)
{
    MedianHybridQuickSortImpl(array, 0, size-1);
    InsertionSort(array, size);
}


/**
 *  @brief Return the maximal element of an array
 */
template<typename T>
float getMax(
              T *p,
              int size)
{
    int i;
    float max = 0;
    for (i = 0; i < size; i++)
        if(p[i] > max)
            max = p[i];
    return max;
}

/**
 *  @brief Return the maximal element of an array and its position
 */
template<typename T>
void getMax(
            T *p,
            int size,
            T &max,
            int &pos)
{
    int i;
    max = 0;
    pos = 0;
    for (i = 0; i < size; i++)
    {
        if(p[i] > max)
        {
            max = p[i];
            pos = i;
        }
    }
}

/**
 *  @brief Return the minimal element of an array
 */
template<typename T>
float getMin(
             T *p,
             int size)
{
    int i;
    float min = p[0];
    for (i = 0; i < size; i++)
        if(p[i] < min)
            min = p[i];
    
    return min;
}

/**
 *  @brief Return the minimum element of an array and its position
 */
template<typename T>
void getMin(
            T *p,
            int size,
            T &min,
            int &pos)
{
    int i;
    min = p[0];
    pos = 0;
    for (i = 0; i < size; i++)
    {
        if(p[i] < min)
        {
            min = p[i];
            pos = i;
        }
    }
}

/**
 *  @brief Return the median of an array
 */
template<typename T>
float getMedian(
                 T   *daArray,
                 int iSize)
{
    //quicksort<T>(daArray, iSize);
    MedianHybridQuickSort(daArray, iSize);

    // Middle or average of middle values in the sorted array.
    if ((iSize % 2) == 0)
        return (daArray[iSize/2] + daArray[(iSize/2) - 1]) / 2.0;
    else
        return daArray[iSize/2];
}

/**
 *  @brief Compute percentile of an array
 */
template<typename T>
float percentile(T *v, int vSize, float p)
{    
    //quicksort<T>(v, vSize);
    MedianHybridQuickSort(v, vSize);
    
    unsigned    N = vSize;
    float       n = p * (float(N) - 1.0) + 1.0;
    unsigned    k = unsigned(floor(n));
    float       d = n - k;
    if(k == 0) return v[0];
    if(k == N) return v[N - 1];
    
    return v[k - 1] + d * (v[k] - v[k - 1]);
}

/**
 *  @brief Compute the norm of an array
 */
template<typename T>
float norm(T* v, int size)
{
    int i;
    float p = 0.0;
    for (i = 0; i < size; i++)
        p += v[i]*v[i];
    return sqrt(p);
}

/**
 *  @brief Return the sum of all elements of an array
 */
template<typename T>
float getSum(T* v, int size)
{
    int i;
    float sum = 0;
    for (i = 0; i < size; i++)
        sum += v[i];
    
    return sum;
}

/**
 *  @brief Return the mean value of an array
 */
template<typename T>
float getMean(
              T *daArray,
              int iSize)
{
    return getSum(daArray, iSize) / (float)iSize;
}

/**
 *  @brief Compute the standard deviation of an array
 */
template<typename T>
float std_dev(T *a, int n, float mean)
{
    int i;
    if(n == 0)
        return 0.0;

    float sq_diff_sum = 0;
    
    for (i = 0; i < n; ++i)
    {
        float diff = a[i] - mean;
        sq_diff_sum += diff * diff;
    }
    float variance = sq_diff_sum / (float)n;
    
    return sqrt(variance);
}

/**
 *  @brief Compute the absolute value of a complex
 */
template<typename T>
inline T absValue(float real, float im)
{
    return sqrt(real * real + im * im);
}

/**
 *  @brief Same function as the linspace MATLAB function
 *  Return a vector of N value going to x1 to x2
 */
template<typename T>
void linspace(T x0, T x1, int N, T* &val)
{
    val = (T *)malloc(N * sizeof(T));

    if (N == 1)
            val[0] = x0;

    if (N > 1)
        for (unsigned i = 0; i < N; i++)
            val[i] = x0 + (x1 - x0) * i / (N - 1);
}

/**
 *  @brief Same function as the linspace MATLAB function
 *  Return a vector of N value going to x1 to x2
 */
template<typename T>
vector<T> linspace(T x0, T x1, int N)
{
    vector<T> val;
    
    if (N == 1)
        val.push_back(x0);
    
    if (N > 1)
        for (unsigned i = 0; i < N; i++)
            val.push_back(x0 + (x1 - x0) * i / (N - 1));
            
    return val;
}

/**
 *  @brief Return the maximal absolute value of an array
 */
template<typename T>
float getMaxAbs(T *array, int size)
{
    int i;
    float *abs = (float *)malloc(size * sizeof(float));
    for (i = 0; i < size; i++)
        abs[i] = fabs(array[i]);
    float max_abs = getMax(abs, size);
    free(abs);
    return max_abs;
}

/**
 *  @brief Comparison callback function to sort two pairs
 */
template<typename T>
bool pairCompare(const std::pair<int, float>& firstElem, const std::pair<int, float>& secondElem)
{
    return firstElem.second < secondElem.second;
}

/**
 *  @brief Compute frequencies of each value in a vector
 */
template<typename T>
vector<int> histogram( vector<float> &array, int N )
{
    int i, j;
    vector<int> freq(N, 0);
    MedianHybridQuickSort(&array[0], (int)array.size());
    float min = array[0];
    float max = array[array.size() - 1];

    float interval = (max - min) / N;
    vector<pair<float, float> > intervals;
    for (i = 0; i < N; i++)
    {
        intervals.push_back(pair<float, float>(min, min + interval));
        min += interval;
    }
    
    //Increment the max value of the last interval, so the max sound mean is inside the last interval
    intervals[N - 1].second += 0.0001;
    //Compute frequencies
    for (i = 0; i < array.size(); i++)
        for (j = 0; j < N; j++)
            if (array[i] >= intervals[j].first && array[i] < intervals[j].second )
            {
                freq[j]++;
                break;
            }
    
    return freq;
}

/**
 *  @brief Compute modulo
 */
template<typename T>
int mod(int a, int b)
{
    return (a%b+b)%b;
}

/**
 *  @brief Return a vector with only unique values
 */
template<typename T>
void vecUnique(vector<T>& v)
{
    std::sort(v.begin(), v.end());
    typename std::vector<T>::iterator it;
    it = std::unique (v.begin(), v.end());
    v.resize( std::distance(v.begin(),it) );
}

/**
 *  @brief Return a vector of values that are in arr1 and arr2
 */
template<typename T>
vector<int> isMember(vector<T> arr1, vector<T> arr2)
{
    int i;
    vector<int> ids;
    typename vector<T>::iterator it;
    for (i = 0; i < arr1.size(); i++)
    {
        it = std::find(arr2.begin(), arr2.end(), arr1[i]);
        if (it != arr2.end())
            ids.push_back((int)(it - arr2.begin()));
    }
    return ids;
}

/**
 *  @brief Generate a random float between min and max
 */
template<typename T>
float gen_random_float(float min, float max)
{
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(min, max);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    return gen();
}

/**
 *  @brief Generate a vector of random float between min and max
 */
template<typename T>
vector<float> gen_random_float_vector(float min, float max, int size)
{
    int i;
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(min, max);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    vector<float> res;
    for (i = 0; i < size; i++)
        res.push_back(gen());
    
    return res;
}

/**
 *  @brief Generate an array of random float between min and max
 */
template<typename T>
float* gen_random_float_array(float min, float max, int size)
{
    int i;
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(min, max);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    float *res = (float*)malloc(size * sizeof(float));
    for (i = 0; i < size; i++)
        res[i] = gen();
    
    return res;
}

/**
 *  @brief Pad array with 0
 */
template<typename T>
void padarray(T* &array, int array_size, int padding_size, T value, string method)
{
    int i;
    if (method == "post")
    {
        array = (T*)realloc(array, (array_size + padding_size) * sizeof(T));
        for (i = array_size; i < array_size + padding_size; i++)
            array[i] = value;
    }
    else if (method == "pre")
    {
        T* copy = (T*)malloc(array_size * sizeof(T));
        std::copy(array, array + array_size, copy);
        free(array);
        array = (T*)malloc((array_size + padding_size) * sizeof(T));
        for (i = 0; i < padding_size; i++)
            array[i] = value;
        int index = 0;
        for (i = padding_size; i < array_size + padding_size; i++)
            array[i] = copy[index++];
        free(copy);
    }
    else
        cout << "Error: method not known";
}

/**
 *  @brief Return the bytes values of a number
 */
template<typename T>
vector<int> toBytes(T x, int size)
{
    vector<int> ret;
    while(x)
    {
        if (x&1)
            ret.push_back(1);
        else
            ret.push_back(0);
        x>>=1;
    }
    while (ret.size() < size)
        ret.push_back(0);

    reverse(ret.begin(),ret.end());
    return ret;
}

/**
 *  @brief Find the coefficients of a polynomial of degree nDegree that fits the data
 */
template<typename T, typename K>
std::vector<T> polyfit( const std::vector<K>& oX,
                       const std::vector<T>& oY, int nDegree )
{
    int i, nRow, nCol;
    using namespace boost::numeric::ublas;
    
    if ( oX.size() != oY.size() )
        throw std::invalid_argument( "X and Y vector sizes do not match" );
    
    // more intuative this way
    nDegree++;
    
    size_t nCount =  oX.size();
    matrix<T> oXMatrix( nCount, nDegree );
    matrix<T> oYMatrix( nCount, 1 );
    
    // copy y matrix
    for ( i = 0; i < nCount; i++ )
    {
        oYMatrix(i, 0) = oY[i];
    }
    
    // create the X matrix
    for ( nRow = 0; nRow < nCount; nRow++ )
    {
        T nVal = 1.0f;
        for ( nCol = 0; nCol < nDegree; nCol++ )
        {
            oXMatrix(nRow, nCol) = nVal;
            nVal *= oX[nRow];
        }
    }
    
    // transpose X matrix
    matrix<T> oXtMatrix( trans(oXMatrix) );
    // multiply transposed X matrixPtr with X matrix
    matrix<T> oXtXMatrix( prec_prod(oXtMatrix, oXMatrix) );
    // multiply transposed X matrixPtr with Y matrix
    matrix<T> oXtYMatrix( prec_prod(oXtMatrix, oYMatrix) );
    
    // lu decomposition
    permutation_matrix<int> pert(oXtXMatrix.size1());
    const std::size_t singular = lu_factorize(oXtXMatrix, pert);
    // must be singular
    BOOST_ASSERT( singular == 0 );
    
    // backsubstitution
    lu_substitute(oXtXMatrix, pert, oXtYMatrix);
    
    // copy the result to coeff
    std::vector<T> res( oXtYMatrix.data().begin(), oXtYMatrix.data().end() );
    std::reverse(res.begin(), res.end());
    return res;
}

/**
 *  @brief Round up to next higher power of two
 */
template<typename T>
inline int pow2roundup (T x)
{
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

/**
 *  @brief Compute diff of a vector (like MATLAB)
 */
template<typename T>
vector<T> diff(vector<T>& v)
{
    int i;
    vector<T> res;
    for (i = 0; i < v.size() - 1; i++)
        res.push_back(v[i + 1] - v[i]);
    return res;
}

#endif /* defined(__Orchids__myUtils__) */
