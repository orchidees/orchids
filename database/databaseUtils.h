/**
 *
 *  @file			databaseUtils.h
 *  @brief			Collection of utilities for the database
 *  @version        0.14
 *  @author			Antoine Bouchereau
 *	@date			25-02-2014
 *
 */

#ifndef __Orchids__databaseUtils__
#define __Orchids__databaseUtils__

#include "Orchids.h"

class BDBTxn;

/**
 *  @brief Return a list of static descriptors
 */
vector<string>  getStaticDescriptors();
/**
 *  @brief Return a list of dynamic descriptors
 */
vector<string>  getDynamicDescriptors();
/**
 *  @brief Return a list of all database tables
 */
vector<string>  getTablesList();
/**
 *  @brief Return a list of database table types (string/float/array/array2D)
 */
vector<string>  getTypesList();


/**
 *  @brief Return a list of supported sound formats
 */
vector<string>  getSupportedFormats();

/**
 *  @brief Convert a sqlStruct to a symbolics structure
 *  @param sql sqlStruct to convert into symbolics struct
 */
symbolics       setSymbolics        (sqlStruct &sql);

/**
 *  @brief Return the size of a descriptor
 *  @param f Descriptor structure
 */
u_int32_t       getDescriptorSize   (feature<float> &f);
/**
 *  @brief Return the size of a partials structure
 *  @param f Partials structure
 */
u_int32_t       getPartialSize      (partials &p);
/**
 *  @brief Return the size of a melBands structure
 *  @param f melBands structure
 */
u_int32_t       getMelBandSize      (melStruct &m);

/**
 *  @brief Copy a buffer into another
 *  @param dest Buffer to fill
 *  @param data Buffer to copy
 *  @param size Size of the buffer to copy
 */
void            setData             (float* &dest, float *data, int size);
/**
 *  @brief Serialize a melBands structure into a buffer
 *  @param val MelBands structure
 *  @param buffer Buffer to fill
 */
int             recordMelBands      (melStruct &val,        char* &buffer);
/**
 *  @brief Serialize a partials structure into a buffer
 *  @param val Partials structure
 *  @param buffer Buffer to fill
 */
int             recordPartials      (partials &val,         char* &buffer);
/**
 *  @brief Serialize a descriptor structure into a buffer
 *  @param val Descriptor structure
 *  @param buffer Buffer to fill
 */
int             recordDescriptor    (feature<float> &val,   char* &buffer);

/**
 *  @brief Deserialize a buffer into a melBands structure
 *  @param val MelBands structure
 *  @param vald Data structure retrieved from the database
 */
void            restoreMelBands     (melStruct &val,        Dbt &vald);
/**
 *  @brief Deserialize a buffer into a partials structure
 *  @param val Partials structure
 *  @param vald Data structure retrieved from the database
 */
void            restorePartials     (partials &val,         Dbt &vald);
/**
 *  @brief Deserialize a buffer into a descriptor structure
 *  @param val Descriptor structure
 *  @param vald Data structure retrieved from the database
 */
void            restoreDescriptor   (feature<float> &val,   Dbt &vald);

/**
 *  @brief Check if quantiles has already been computed and return them
 *  @param desc Descriptor name
 *  @param N Number of quantiles to compute
 *  @param knowledge Pointer to the current knowledge
 */
vector<int>     getQuantiles(string desc, int N, BDBTxn* txn);

#endif /* defined(__Orchids__databaseUtils__) */
