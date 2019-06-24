/**
 *
 *  @file			bdbInterface.h
 *  @brief			Interface to handle a database and perform queries
 *  @version        0.14
 *  @author			Antoine Bouchereau
 *	@date			25-02-2014
 *
 */

#ifndef __Orchids__bdbInterface__
#define __Orchids__bdbInterface__

#include "Orchids.h"
#include "query.h"
#include "databaseUtils.h"
#include "indexing/indexNode.h"
#include "indexing/indexRepresentation.h"
#include "indexing/indexTree.h"
#include "indexing/constructIndexesFromDB.h"

/**
 *  @brief Initiate and store value in a Dbt structure
 *
 *  @param dbt Structure to fill
 *  @param data The data to store
 *  @param size Size of the data
 */
void            init_dbt            ( Dbt *dbt, const void *data, uint32_t size );

/**
 *  @brief BDB comparison function for float keys
 *
 *  @param dbp Db handle
 *  @param a First db key
 *  @param b Second db key
 *  @param t Size of key
 */
int             compare_float_keys  ( DB *dbp, const DBT *a, const DBT *b, size_t *t );

/**
 *  @brief BDB comparison function for int keys
 *
 *  @param dbp Db handle
 *  @param a First db key
 *  @param b Second db key
 *  @param t Size of key
 */
int             compare_int_keys    ( DB *dbp, const DBT *a, const DBT *b, size_t *t );

/**
 *  @brief Count number of elements in Db
 *
 *  @param db Db handle
 *  @param txn Db transaction handle
 */
extern uint32_t bdb_key_count       ( Db *db, DbTxn *txn );

/**
 *  @brief Overwrite of bdb allocation
 *
 *  @param size Bytes to allocate
 */
static void*    bdbcAlloc           ( size_t size );

/**
 *  @brief Overwrite of bdb reallocation
 *
 *  @param mem Data to reallocate
 *  @param size Data size
 */
static void*    bdbcRealloc         ( void* mem, size_t size );

/**
 *  @brief Overwrite of bdb free
 *
 *  @param mem Data to free
 */
static void     bdbcFree            ( void* mem );

/**
 *  @brief Users specific error handler
 *
 *  @param env Environment handle
 *  @param errpfx Error prefix
 *  @param msg Error message
 */
static void     errorPrinter        ( const DbEnv * env, const char * errpfx, const char * msg );

/**
 *  @brief Users specific warning message handler
 *
 *  @param env Environment handle
 *  @param msg Warning message
 */
static void     messagePrinter      ( const DbEnv * env, const char * msg );

/**
 *  @brief Thread callback that throws checkpoint every minute
 *
 *  @param arg Environment handle
 */
void*           checkpoint_thread   (void *arg);

/**
 *  @brief Log file removal
 *
 *  @param arg Environment handle
 */
void*           logfile_thread      (void *arg);



//---------------Secondary keys extractor------------------
/**
 *  @brief Definition of extractor function for secondary index
 */
typedef int     (BDBExtractorFunc)      ( Db*, const Dbt*, const Dbt*, Dbt* );

/**
 *  @brief Extracts note from a symbolics structure 
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             noteExtractor           ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts name from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             nameExtractor           ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts instrument from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             instrumentExtractor     ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts source from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             sourceExtractor         ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts dynamics from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             dynamicsExtractor       ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts playingStyle from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             playingStyleExtractor   ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts stringMute from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             stringMuteExtractor   ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts brassMute from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             brassMuteExtractor   ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts family from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             familyExtractor         ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts duration from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             durationExtractor       ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts MIDIcent from a symbolics structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             MIDIcentExtractor       ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts mean from a descriptor structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             meanExtractor           ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts loudness from a mel bands structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             loudnessFactorExtractor ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );

/**
 *  @brief Extracts partials mean energy from a partials structure
 *
 *  @param db Secondary db handle
 *  @param pkey Primary db record's key
 *  @param pdata Primary db record's data
 *  @param skey Secondary db record's key
 */
int             partialsMeanEnergyExtractor ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey );


//----------------Virtual database interface----------------
/**
 *  @class BDBIface
 *
 *  @brief Virtual interface for principals database functions
 */
class BDBIface
{
public:
    /**
     *  @brief Insert symbolics structure in db
     */
    virtual void putSymbolics   ( string table, Dbt *key, symbolics val, bool update = true ) = 0;
    /**
     *  @brief Insert melBands structure in db
     */
    virtual void putMelBands    ( string table, Dbt *key, melStruct& val, bool update = true ) = 0;
    /**
     *  @brief Insert partials structure in db
     */
    virtual void putPartials    ( string table, Dbt *key, partials& val, bool update = true ) = 0;
    /**
     *  @brief Insert descriptor structure in db
     */
    virtual void putDescriptor  ( string table, Dbt *key, feature<float>& val, bool update = true ) = 0;
    
    /**
     *  @brief Delete a record in db
     */
    virtual void del            ( string table, Dbt *key ) = 0;
    
    /**
     *  @brief Get symbolics structure in db
     */
    virtual bool getSymbolics   ( string table, Dbt *key, symbolics& val ) = 0;
    /**
     *  @brief Get melBands structure in db
     */
    virtual bool getMelBands    ( string table, Dbt *key, melStruct& val ) = 0;
    /**
     *  @brief Get partials structure in db
     */
    virtual bool getPartials    ( string table, Dbt *key, partials& val ) = 0;
    /**
     *  @brief Get descriptor structure in db
     */
    virtual bool getDescriptor  ( string table, Dbt *key, feature<float>& val ) = 0;
    
    /**
     *  @brief Create a primary table
     */
    virtual void createTable    ( string table, bool allowDuplicates = false, bool set_float_compare = false, bool set_int_compare = false ) = 0;
    /**
     *  @brief Create a secondary index
     */
    virtual void createIndex    ( string index, string table, BDBExtractorFunc extractor, bool allowDuplicates = false, bool set_int_compare = false, bool set_string_compare = false ) = 0;
    
    /**
     *  @brief Analyze and insert a sound in db
     */
    virtual bool addSound           ( string filepath, string source, string instru ) = 0;
    /**
     *  @brief Analyze and insert a directory in db
     */
    virtual bool addSoundDirectory  ( string path, string source, string instru ) = 0;
};

//---------------Database connexion class-----------

class Query;
class Session;

/**
 *  @class BDBConnector
 *
 *  @brief Class that handles the connexion to a database
 */
class BDBConnector : public BDBIface
{
public:
    /**
     *  @brief Default constructor
     */
    BDBConnector(): mEnv(DB_CXX_NO_EXCEPTIONS) {};
    
    /**
     *  @brief Constructor
     *  @param env Name of the environment
     *  @param dbFile Name of the db
     */
    BDBConnector( string env, string dbFile, Session* sessObj );
    /**
     *  @brief Destructor
     */
    ~BDBConnector();
    
    /**
     *  @brief Initiate the db structure with predefined tables and indexes
     */
    void initCoreDB( DbTxn* txn );
    /**
     *  @brief Close db and env
     */
    void close();
    /**
     *  @brief Connect to a table and returns a unintialized cursor
     *  @param table Name of the table
     *  @param txn Transaction handle
     */
    Dbc* getCursor( string table, DbTxn* txn );
    /**
     *  @brief Returns the last stored index in the database
     *  @param txn Transaction handle
     */
    int  get_current_index( DbTxn* txn );
    
    
    /**
     *  @brief Analyze and insert a sound in db
     *  @param path Full path
     *  @param file Sound name
     *  @param source Library name of the sound 
     *  @param instru Name of the sound instrument
     */
    bool addSound           ( string filepath, string source, string instru );
    /**
     *  @brief Analyze and insert a directory in db
     *  @param path Full path of the directory
     *  @param source Library name of the directory
     *  @param instru Name of the sound instrument
     */
    bool addSoundDirectory  ( string path, string source, string instru );
    
    bool add_i_soundDirectory_OSC_style( string path, string source, string instru, string instruShort, string pStyle, DbTxn* txn );
    bool add_i_soundDirectory_OSC_recurse( string path, string source, string instru, string instruShort, DbTxn* txn );
    
    /**
     *  @brief Insert symbolics structure in db
     *  @param table Add symbolics structure to the table 'name'
     *  @param key Key of the record
     *  @param val Symbolics structure to insert
     *  @param update If true, the db will automatically update the record
     */
    void putSymbolics   ( string table, Dbt *key, symbolics val, bool update = true );
    /**
     *  @brief Insert melBands structure in db
     *  @param table Add melBands structure to the table 'name'
     *  @param key Key of the record
     *  @param val MelBands structure to insert
     *  @param update If true, the db will automatically update the record
     */
    void putMelBands    ( string table, Dbt *key, melStruct& val, bool update = true );
    /**
     *  @brief Insert partials structure in db
     *  @param table Add partials structure to the table 'name'
     *  @param key Key of the record
     *  @param val Partials structure to insert
     *  @param update If true, the db will automatically update the record
     */
    void putPartials    ( string table, Dbt *key, partials& val, bool update = true );
    /**
     *  @brief Insert descriptor structure in db
     *  @param table Add descriptor structure to the table 'name'
     *  @param key Key of the record
     *  @param val Descriptor structure to insert
     *  @param update If true, the db will automatically update the record
     */
    void putDescriptor  ( string table, Dbt *key, feature<float>& val, bool update = true );
    

    /**
     *  @brief Delete a record in db
     *  @param table Delete the record from the 'table'
     *  @param key Key of the record to delete
     */
    void del                    ( string table, Dbt *key );
    /**
     *  @brief Remove a sound in db by index
     *  @param soundID Index of the sound to remove
     */
    void removeSound            ( int soundID );
    /**
     *  @brief Remove a family of sound
     *  @param family Family to remove
     */
    void removeFamily           ( char* family );
    /**
     *  @brief Iterate through a directory recursively and remove all sounds from db
     *  @param path Path of the directory
     */
    void removeSoundDirectory   ( char* path );
    /**
     *  @brief Iterate through a directory recursively and add sounds to the database that doesn't exists yet
     *  @param path Path of the directory to check
     *  @param source Add the sounds to this 'source'
     */
    bool refreshSoundDirectory  ( char* path, char* source, vector<string> &filenames );
    /**
     *  @brief Delete a record from a database
     *  @param table Name of the database
     *  @param txn Transaction handle
     *  @param key Key of the record to delete
     */
    void del_i                      ( string table, DbTxn* txn, Dbt *key );
    

    /**
     *  @brief Search index of a sound by its filename
     *  @param filename Name of the sound file
     */
    int             getIndex                    ( string filename );
    /**
     *  @brief Search a sound with a unique key and return its symbolics
     *  @param table Table to search in
     *  @param key Unique key
     *  @param val Symbolics struct to fill
     */
    bool            getSymbolics                ( string table, Dbt *key, symbolics& val );
    /**
     *  @brief Search multiple sounds for one key and return theirs indexes
     *  @param table Table to search in
     *  @param key Key that can be a duplicate
     *  @param IDs Vector of index to fill
     */
    bool            getSymbolics                ( string table, Dbt *key, vector<int>& IDs );
    /**
     *  @brief Search sounds with a member between two values and return theirs indexex
     *  @param table Table to search in
     *  @param start Start value of the interval
     *  @param end End value of the interval
     *  @param soundIDs Vector of index to fill
     */
    bool            getSymbolicsByRange         ( string table, float start, float end, vector<int>& soundIDs );
    /**
     *  @brief Search a sound with a unique key and return its melBands
     *  @param table Table to search in
     *  @param key Unique key
     *  @param val Melbands struct to fill
     */
    bool            getMelBands                 ( string table, Dbt *key, melStruct& val );
    /**
     *  @brief Search a sound with a unique key and return its partials
     *  @param table Table to search in
     *  @param key Unique key
     *  @param val Partials struct to fill
     */
    bool            getPartials                 ( string table, Dbt *key, partials& val );
    /**
     *  @brief Search a sound with a unique key and return its descriptor
     *  @param table Table to search in
     *  @param key Unique key
     *  @param val Descriptor struct to fill
     */
    bool            getDescriptor               ( string table, Dbt *key, feature<float>& val );
    /**
     *  @brief Search sounds with a descriptor's mean between two values and return theirs indexes
     *  @param table Table to search in
     *  @param start Start value of the interval
     *  @param end End value of the interval
     *  @param soundIDs Vector of index to fill
     */
    bool            getDescriptorByMeanRange    ( string table, float start, float end, vector<int>& soundIDs );
    /**
     *  @brief Return the minimal and maximal mean value for a descriptor
     *  @param descriptor Table to search in
     *  @param min Minimal mean value
     *  @param max Maximal mean value
     */
    bool            getDescriptorMinMax         ( string descriptor, float& min, float& max );
    /**
     *  @brief Main query function. Each query struct executes a different function and fills the vector of sound indexes
     *  @param queries Vector of query structures to execute in order
     *  @param soundIDs Vector of sound indexes to fill
     */
    bool            getSoundsQuery              ( vector<Query> queries, vector<int>& soundIDs );
    /**
     *  @brief Compute the distance between an array of float and a descriptor of each sound in soundIDs. Return the vector of indexes ordered by distance
     *  @param descValue Array of float
     *  @param descName Descriptor to check
     *  @param soundIDs Vector of sound indexes to order
     */
    bool            getTemporalQueryDTW         ( vector<float>& descValue, string descName, vector<int>& soundIDs );
    /**
     *  @brief Retrieve all values from one symbolic information
     *  @param table Symbolic to retrieve (only strings)
     *  @param list Vector of values to fill
     */
    bool            getSecondaryIndexList       ( string table, vector<string>& list );
    /**
     *  @brief Take sound indexes and return their names
     *  @param soundIDs Vector of sound indexes
     */
    vector<string>  getNames                    ( vector<int>& soundIDs );
    /**
     *  @brief Take sound indexes and return their path
     *  @param soundIDs Vector of sound indexes
     *  @param isSOL Indicates if sounds are from SOL or User library
     */
    vector<string>  getPaths                    ( vector<int>& soundIDs, vector<bool>& isSOL );
    
    
    //Create primary table/secondary index
    /**
     *  @brief Create a primary database
     *  @param table Name of the database
     *  @param allowDuplicates Allow duplicates keys
     *  @param set_float_compare Sort floating keys
     *  @param set_int_compare Sort int keys
     */
    void createTable ( string table, bool allowDuplicates = false, bool set_float_compare = false, bool set_int_compare = false );
    /**
     *  @brief Create a secondary index
     *  @param index Name of the secondary index
     *  @param table Name of the primary database
     *  @param extractor Extractor callback to retrieve the secondary keys
     *  @param allowDuplicates Allow duplicates keys
     *  @param set_float_compare Sort floating keys
     *  @param set_int_compare Sort int keys
     */
    void createIndex ( string index, string table, BDBExtractorFunc extractor, bool allowDuplicates = false, bool set_float_compare = false, bool set_int_compare = false );
    
    /**
     *  @brief Return the list of pointer to primary tables
     */
    map<string, Db*> getTables()    { return mTables; };
    /**
     *  @brief Return the list of pointer to seconday indexes
     */
    map<string, Db*> getIndexes()   { return mIndexes; };
    
protected:
    friend class BDBTxn;
    
    /**
     *  @brief Return a pointer to a database handler
     *  @param table Name of the table
     */
    Db*     getTable_i  ( string table );
    /**
     *  @brief Create/open a table and return a pointer to it
     *  @param Name of the secondary index
     *  @param table Name of the database
     *  @param txn Transaction handle
     *  @param allowDuplicates Allow duplicates keys
     *  @param set_float_compare Sort floating keys
     *  @param set_int_compare Sort int keys
     */
    Db*     openTable_i ( string table, DbTxn* txn, bool allowDuplicates = false, bool set_float_compare = false, bool set_int_compare = false );
    /**
     *  @brief Close a database
     *  @param db Pointer to the db handle
     */
    void    closeTable_i( Db* db );
    /**
     *  @brief Launch transaction and return a pointer to the transaction handle
     */
    DbTxn*  startTxn_i();
    
    /**
     *  @brief Create a database and add it to the database list
     *  @param table Name of the database
     *  @param txn Transaction handle
     *  @param allowDuplicates Allow duplicates keys
     *  @param set_float_compare Sort floating keys
     *  @param set_int_compare Sort int keys
     */
    void createTable_i( string table, DbTxn* txn, bool allowDuplicates = false, bool set_float_compare = false, bool set_int_compare = false );
    /**
     *  @brief Create a secondary index and add it to the secondary indexes list
     *  @param index Name of the secondary index
     *  @param table Name of the database to link with
     *  @param txn Transaction handle
     *  @param extractor Secondary index extractor callback function
     *  @param allowDuplicates Allow duplicates keys
     *  @param set_float_compare Sort floating keys
     *  @param set_int_compare Sort int keys
     */
    void createIndex_i( string index, string table, DbTxn* txn, BDBExtractorFunc extractor, bool allowDuplicates = false, bool set_float_compare = false, bool set_int_compare = false );
    
    /**
     *  @brief Add a sound to the database
     *  @param path Path to the sound file
     *  @param file Sound file name
     *  @param source Name of the sound file source
     *  @param instru Name of the sound instrument
     *  @param txn Transaction handle
     */
    bool add_i_sound            ( string filepath, string source, string instru, DbTxn* txn );
    
    bool add_i_sound_style      ( string soundfile, string source, string instru, string style, DbTxn *txn );
    
    /**
     *  @brief Add a directory to the database (recursive)
     *  @param path Directory path
     *  @param source Name of the source
     *  @param instru Name of the sound instrument
     *  @param txn Transaction handle
     */
    bool add_i_soundDirectory   ( string path, string source, string instru, DbTxn* txn );
    
    /**
     *  @brief Put symbolics to the database
     *  @param table Name of the database
     *  @param txn Transaction handle
     *  @param key Key of the record
     *  @param val Symbolics struct to add
     *  @param update Auto update enabled
     */
    void put_i_symbolics    ( string table, DbTxn* txn, Dbt *key, symbolics val, bool update = true );
    /**
     *  @brief Put melBands to the database
     *  @param table Name of the database
     *  @param txn Transaction handle
     *  @param key Key of the record
     *  @param val melBands struct to add
     *  @param update Auto update enabled
     */
    void put_i_melBands     ( string table, DbTxn* txn, Dbt *key, melStruct& val, bool update = true );
    /**
     *  @brief Put partials to the database
     *  @param table Name of the database
     *  @param txn Transaction handle
     *  @param key Key of the record
     *  @param val Partials struct to add
     *  @param update Auto update enabled
     */
    void put_i_partials     ( string table, DbTxn* txn, Dbt *key, partials& val, bool update = true );
    /**
     *  @brief Put descriptor to the database
     *  @param table Name of the database
     *  @param txn Transaction handle
     *  @param key Key of the record
     *  @param val Descriptor struct to add
     *  @param update Auto update enabled
     */
    void put_i_descriptor   ( string table, DbTxn* txn, Dbt *key, feature<float>& val, bool update = true );
    /**
     *  @brief Put ADSR to the database
     *  @param table Name of the database
     *  @param txn Transaction handle
     *  @param key Key of the record
     *  @param val Descriptor struct to add
     *  @param update Auto update enabled
     */
    void put_i_adsr         ( string table, DbTxn* txn, Dbt *key, envelope val, bool update = true );
    
    /**
     *  @brief Check if a sound file has been deleted from the disk
     *  @param txn Transaction handle
     *  @param source Name of the source to check
     *  @param filenames List of sound file names to check
     */
    void check_sounds_to_delete     ( DbTxn* txn, string source, vector<string> &filenames );
    /**
     *  @brief Delete a sound from the database
     *  @param soundID ID of the sound to delete
     *  @param txn Transaction handle
     */
    void remove_i_sound             ( int soundID, DbTxn* txn );
    /**
     *  @brief Delete a family from the database
     *  @param family Name of the family to delete
     *  @param txn Transaction handle
     */
    void remove_i_family            ( char* family, DbTxn* txn );
    /**
     *  @brief Delete a directory from the database
     *  @param path Directory path to delete
     *  @param txn Transaction handle
     */
    void remove_i_soundDirectory    ( char* path, DbTxn* txn );
    /**
     *  @brief Check if sound files in a directory are in the database
     *  @param path Directory path to refresh
     *  @param source Name of the source if sound files are added
     *  @param filenames List of sound file names to fill
     *  @param txn Transaction handle
     */
    bool refresh_i_soundDirectory   ( string path, string source, vector<string> &filenames, DbTxn* txn );
    
    int             get_i_index                     ( string filename, DbTxn* txn );
    bool            get_i_symbolics                 ( string table, DbTxn* txn, Dbt *key, symbolics& val );
    vector<bool>    get_i_symbolicsOptimized        ( string table, DbTxn* txn, vector<int>& IDs, vector<symbolics>& values );
    bool            get_i_symbolics                 ( string table, DbTxn* txn, Dbt *key, vector<int>& IDs );
    bool            get_i_symbolicsByRange          ( string table, DbTxn* txn, float start, float end, vector<int>& soundIDs );
    bool            get_i_melBands                  ( string table, DbTxn* txn, Dbt *key, melStruct& val );
    vector<bool>    get_i_melBandsOptimized         ( string table, DbTxn* txn, vector<int>& IDs, vector<melStruct>& val );
    bool            get_i_partials                  ( string table, DbTxn* txn, Dbt *key, partials& val );
    vector<bool>    get_i_partialsOptimized         ( string table, DbTxn* txn, vector<int>& IDs, vector<partials>& values );
    bool            get_i_descriptor                ( string table, DbTxn* txn, Dbt *key, feature<float>& val );
    vector<bool>    get_i_descriptorOptimized       ( string table, DbTxn* txn, vector<int>& IDs, vector<feature<float> >& values );
    bool            get_i_descriptorByMeanRange     ( string table, DbTxn* txn, float start, float end, vector<int>& soundIDs );
    bool            get_i_descriptorMinMax          ( DbTxn* txn, string descriptor, float& min, float& max );
    bool            get_i_multipleFinalOptimized    ( DbTxn* txn, vector<int>& IDs, vector<string>& updateFeatures, vector<int>& typesIds, map<int, vector<matrixPtr> >& features, bool denormalized );
    bool            get_i_soundsQuery               ( DbTxn* txn, vector<Query> queries, vector<int>& soundIDs );
    bool            get_i_temporalQueryDTW          ( DbTxn* txn, vector<float>& descValue, string descName, vector<int>& soundIDs );
    bool            get_i_secondaryIndexList        ( DbTxn* txn, string table, vector<string>& list );
    bool            get_i_soundsDescriptorMean      ( DbTxn* txn, string table, vector<int>& soundIDs, vector<float>& means );
    bool            get_i_allSoundsDescriptorMean   ( DbTxn* txn, string table, vector<float>& means );
    vector<string>  get_i_names                     ( DbTxn* txn, vector<int>& soundIDs );
    vector<string>  get_i_paths                     ( DbTxn* txn, vector<int>& soundIDs, vector<bool>& isSOL );
    
    string mEnvName;
    string mDbFile;
    DbEnv  mEnv;
    bool   fDbEnvInit;
    
    map<string, Db*> mTables;
    map<string, Db*> mIndexes;
    
    //boost::thread boost_checkpoint_thread;
    //boost::thread boost_logfile_thread;
    
    Session* sSession;
};

//---------------Transactions support class-----------
/**
 *  @class BDBTxn
 *
 *  @brief Class that handles the connexion to a database with transactions enabled
 */
class BDBTxn : public BDBIface
{
public:
    /**
     *  @brief Constructor
     *  @param dbc BDBConnector object to link to transaction
     */
    BDBTxn  ( BDBConnector& dbc );
    /**
     *  @brief Destructor
     */
    ~BDBTxn ();
    
    //Initialization
    /**
     *  @brief Initialize the database
     */
    void initCoreDB();
    /**
     *  @brief Close the database
     */
    void close();
    
    //Transactions
    /**
     *  @brief Commit last operations to the database
     */
    void commit();
    /**
     *  @brief Close the transacton handle
     */
    void abort();
    
    /**
     *  @brief Return a pointer the BDBConnector object
     */
    BDBConnector* getBDBConnector(){ return mDbc; };
    
    bool addSound           ( string filepath, string source, string instru );
    bool addSoundDirectory  ( string path, string source, string instru );
    bool addSoundDirectory_OSC_style( string path, string source, string instru, string instruShort, string pStyle);
    bool addSoundDirectory_OSC_recurse( string path, string source, string instru, string instruShort);
    int  get_current_index  ( );
    int  key_count          ( string dbName );
    
    //Put/Delete/Get
    void putSymbolics   ( string table, Dbt *key, symbolics val, bool update = true );
    void putMelBands    ( string table, Dbt *key, melStruct& val, bool update = true );
    void putPartials    ( string table, Dbt *key, partials& val, bool update = true );
    void putDescriptor  ( string table, Dbt *key, feature<float>& val, bool update = true );
    void putADSR        ( string table, Dbt *key, envelope val, bool update = true );
    
    void check_sounds_to_delete ( string source, vector<string> &filenames );
    void del                    ( string table, Dbt *key );
    void removeSound            ( int soundID );
    void removeFamily           ( char* family );
    void removeSoundDirectory   ( char* path );
    bool refreshSoundDirectory  ( string path, string source, vector<string> &filenames );
    
    int             getIndex                    ( string filename );
    bool            getSymbolics                ( string table, Dbt *key, symbolics& val );
    bool            getSymbolics                ( string table, Dbt *key, vector<int>& IDs );
    bool            getMelBands                 ( string table, Dbt *key, melStruct& val );
    bool            getPartials                 ( string table, Dbt *key, partials& val );
    bool            getDescriptor               ( string table, Dbt *key, feature<float>& val );
    bool            getDescriptorByMeanRange    ( string table, float start, float end, vector<int>& soundIDs );
    bool            getDescriptorMinMax         ( string descriptor, float& min, float& max );
    bool            getMultipleFinalOptimized   ( vector<int>& IDs, vector<string>& updateFeatures, vector<int>& typesIds, map<int, vector<matrixPtr> >& features, bool denormalized );
    bool            getSoundsQuery              ( vector<Query> queries, vector<int>& soundIDs );
    bool            getTemporalQueryDTW         ( vector<float>& descValue, string descName, vector<int>& soundIDs );
    bool            getSecondaryIndexList       ( string table, vector<string>& list );
    bool            getSoundsDescriptorMean     ( string table, vector<int> &soundIDs, vector<float> &means );
    bool            getAllSoundsDescriptorMean  ( string table, vector<float> &means );
    vector<string>  getNames                    ( vector<int> &soundIDs );
    vector<string>  getPaths                    ( vector<int> &soundIDs, vector<bool>& isSOL );
    
    //Create primary table/secondary index
    void createTable( string table, bool allowDuplicates = false, bool set_false_compare = false, bool set_int_compare = false );
    void createIndex( string index, string table, BDBExtractorFunc extractor, bool allowDuplicates = false, bool set_false_compare = false, bool set_int_compare = false );
    
protected:
    DbTxn*              mTxn;
    BDBConnector*       mDbc;
    bool                mDirty;
};

#endif /* defined(__Orchids__BDBInterface__) */
