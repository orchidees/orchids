//
//  bdbInterface.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 25/02/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "bdbInterface.h"


void init_dbt(Dbt *dbt, const void *data, uint32_t size)
{
    memset(dbt, 0, sizeof(Dbt));
    if (data != 0) {
        dbt->set_data((void *)data);
        // for some APIs (get) we'll need size set,
        // for some (put), we need ulen set.
        // By setting both, this method can be used in either case.
        dbt->set_size(size);
        dbt->set_ulen(size);
    }
    dbt->set_flags(DB_DBT_APPMALLOC);
}

int compare_float_keys(DB *dbp, const DBT *a, const DBT *b, size_t *t)
{
    float ai, bi;
    memcpy(&ai, a->data, sizeof(float));
    memcpy(&bi, b->data, sizeof(float));
    
    float diff = ai - bi;
    if (diff > 0)
        return 1;
    else if (diff < 0)
        return -1;
    
    return 0;
}

int compare_int_keys(DB *dbp, const DBT *a, const DBT *b, size_t *t)
{
	int ai, bi;
    memcpy(&ai, a->data, sizeof(int));
    memcpy(&bi, b->data, sizeof(int));
    
    return (ai - bi);
}

extern uint32_t bdb_key_count(Db *db, DbTxn *txn)
{
    DB_BTREE_STAT *sp;
    uint32_t c = 0;
    
    if (db->stat(txn, &sp, 0) == 0)
    {
        c = sp->bt_nkeys;
        free(sp);
    }
    
    return c;
}

static void* bdbcAlloc( size_t size )
{
    return malloc( size );
}

static void* bdbcRealloc( void* mem, size_t size )
{
    return realloc( mem, size );
}

static void bdbcFree( void* mem )
{
    return free( mem );
}

static void errorPrinter(const DbEnv * env, const char * errpfx, const char * msg)
{
    printf("Berkeley DB error: %s\n", msg);
}

static void messagePrinter(const DbEnv * env, const char * msg)
{
    printf("Berkeley DB message: %s\n", msg);
}

int noteExtractor(Db *sdbp,          // secondary db handle
                  const Dbt *pkey,   // primary db record's key
                  const Dbt *pdata,  // primary db record's data
                  Dbt *skey)         // secondary db record's key
{    
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->note;

    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int nameExtractor(Db *sdbp,          // secondary db handle
                  const Dbt *pkey,   // primary db record's key
                  const Dbt *pdata,  // primary db record's data
                  Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->name;
    
    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int instrumentExtractor(Db *sdbp,    // secondary db handle
                  const Dbt *pkey,   // primary db record's key
                  const Dbt *pdata,  // primary db record's data
                  Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->instrument;
    
    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int dynamicsExtractor(Db *sdbp,    // secondary db handle
                      const Dbt *pkey,   // primary db record's key
                      const Dbt *pdata,  // primary db record's data
                      Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->dynamics;
    
    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int playingStyleExtractor(Db *sdbp,    // secondary db handle
                          const Dbt *pkey,   // primary db record's key
                          const Dbt *pdata,  // primary db record's data
                          Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->playingStyle;
    
    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int stringMuteExtractor(Db *sdbp,    // secondary db handle
                          const Dbt *pkey,   // primary db record's key
                          const Dbt *pdata,  // primary db record's data
                          Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->stringMute;
    
    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int brassMuteExtractor(Db *sdbp,    // secondary db handle
                          const Dbt *pkey,   // primary db record's key
                          const Dbt *pdata,  // primary db record's data
                          Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->brassMute;
    
    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int familyExtractor(Db *sdbp,    // secondary db handle
                    const Dbt *pkey,   // primary db record's key
                    const Dbt *pdata,  // primary db record's data
                    Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->family;
    
    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int sourceExtractor(Db *sdbp,    // secondary db handle
                    const Dbt *pkey,   // primary db record's key
                    const Dbt *pdata,  // primary db record's data
                    Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    char *key = ((symbolics*)pdata->get_data())->source;
    
    skey->set_data(key);
    skey->set_size((int)(strlen(key) + 1));
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int durationExtractor(Db *sdbp,      // secondary db handle
                  const Dbt *pkey,   // primary db record's key
                  const Dbt *pdata,  // primary db record's data
                  Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    symbolics s = *(symbolics*)pdata->get_data();
    
    void* duration = malloc( sizeof(float) );
    memcpy( duration, &s.duration, sizeof(float) );
    
    skey->set_data(duration);
    skey->set_size(sizeof(float));
    //Tells Berkeley DB to free the memory when it is done with this object
    skey->set_flags( DB_DBT_APPMALLOC );
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int MIDIcentExtractor ( Db* db, const Dbt* pkey, const Dbt* pdata, Dbt* skey )
{
    // Now set the secondary key's data
    symbolics s = *(symbolics*)pdata->get_data();
        
    void* MIDIcent = malloc( sizeof(float) );
    memcpy( MIDIcent, &s.MIDIcent, sizeof(float) );
    
    skey->set_data(MIDIcent);
    skey->set_size(sizeof(float));
    //Tells Berkeley DB to free the memory when it is done with this object
    skey->set_flags( DB_DBT_APPMALLOC );
        
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int meanExtractor(Db *sdbp,          // secondary db handle
                  const Dbt *pkey,   // primary db record's key
                  const Dbt *pdata,  // primary db record's data
                  Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    Dbt data = *pdata;
    feature<float> f;
    restoreDescriptor(f, data);
    
    void* mean = malloc( sizeof(float) );
    memcpy( mean, &f.mean, sizeof(float) );
    
    skey->set_data(mean);
    skey->set_size(sizeof(float));
    //Tells Berkeley DB to free the memory when it is done with this object
    skey->set_flags( DB_DBT_APPMALLOC );
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int loudnessFactorExtractor(Db *sdbp,          // secondary db handle
                            const Dbt *pkey,   // primary db record's key
                            const Dbt *pdata,  // primary db record's data
                            Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    Dbt data = *pdata;
    melStruct m;
    restoreMelBands(m, data);
    
    void* loudnessFactor = malloc( sizeof(float) );
    memcpy( loudnessFactor, &m.loudnessFactor, sizeof(float) );
    
    skey->set_data(loudnessFactor);
    skey->set_size(sizeof(float));
    //Tells Berkeley DB to free the memory when it is done with this object
    skey->set_flags( DB_DBT_APPMALLOC );
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}

int partialsMeanEnergyExtractor(Db *sdbp,          // secondary db handle
                                const Dbt *pkey,   // primary db record's key
                                const Dbt *pdata,  // primary db record's data
                                Dbt *skey)         // secondary db record's key
{
    // Now set the secondary key's data
    Dbt data = *pdata;
    partials p;
    restorePartials(p, data);
    
    void* meanEnergy = malloc( sizeof(float) );
    memcpy( meanEnergy, &p.partialsMeanEnergy , sizeof(float) );
    
    skey->set_data(meanEnergy);
    skey->set_size(sizeof(float));
    //Tells Berkeley DB to free the memory when it is done with this object
    skey->set_flags( DB_DBT_APPMALLOC );
    p.deallocate();
    
    // Return 0 to indicate that the record can be created/updated.
    return 0;
}
/*
void* checkpoint_thread(void *arg)
{
    DbEnv *dbenv = (DbEnv*)arg;
    int ret;
    
    //printf("Checkpoint thread: %lu\n", (u_long)pthread_self());

    // Checkpoint once a minute.
    while (1)
    {
        try
        {            
            if (dbenv != NULL)
            {
                if ((ret = dbenv->txn_checkpoint(0, 0, DB_FORCE)) != 0)
                {
                    dbenv->err(ret, "checkpoint thread");
                    exit (1);
                }
            }
            
            boost::this_thread::sleep(boost::posix_time::milliseconds(60000));
        }
        catch(boost::thread_interrupted&)
        {
            exit(0);
        }
    }
}

void *logfile_thread(void *arg)
{
	DbEnv *dbenv = (DbEnv*) arg;
	int ret;
	char **list;
    
	//printf("Log file removal thread: %lu\n", (u_long)pthread_self());

	// Check once every 5 minutes.
    while (1)
    {
        try
        {            
            if (dbenv != NULL)
            {
                // Get the list of log files.
                if ((ret = dbenv->log_archive(&list, DB_ARCH_REMOVE)) != 0)
                {
                    dbenv->err(ret, "DB_ENV->log_archive");
                    exit (1);
                }
            }
            
            boost::this_thread::sleep(boost::posix_time::milliseconds(300000));
        }
        catch(boost::thread_interrupted&)
        {
            exit(0);
        }
	}
}
*/
//--------------------BDBConnector--------------------
BDBConnector::BDBConnector( string env, string dbFile, Session* sessObj ) :
mEnvName( env ), mDbFile( dbFile ), mEnv( DB_CXX_NO_EXCEPTIONS )
{
    sSession = sessObj;
    // Create environment directory
    boost::filesystem::create_directories(boost::filesystem::path(mEnvName));
    
    mEnv.set_alloc( bdbcAlloc, bdbcRealloc, bdbcFree );
    mEnv.set_errcall(errorPrinter);
    mEnv.set_msgcall(messagePrinter);
    mEnv.set_verbose(DB_VERB_RECOVERY, 1);
    
    /* Bigger log files. */
    mEnv.set_lg_bsize(8 * 1024 * 1024);
    mEnv.set_lg_max(32 * 1024 * 1024); /* must be > 4 * lg_bsize */
    
    /* Write the log, but don't sync.  This protects transactions
     against application crashes, but if the system crashes, some
     transactions may be undone.  An acceptable risk, I think. */
    //mEnv.set_flags(DB_TXN_WRITE_NOSYNC | DB_LOG_AUTO_REMOVE, 1);
    mEnv.set_flags(DB_TXN_WRITE_NOSYNC, 1);
    mEnv.set_flags(DB_TXN_NOSYNC, 1);
    
    /* Increase the locking limits.  If you ever get `Dbc::get: Cannot
     allocate memory' or similar, just increase the following number, then
     run db_recover on the database to remove the existing DB
     environment (since changes only take effect on new
     environments). */
    mEnv.set_lk_max_locks(1000000);
    mEnv.set_lk_max_lockers(1000000);
    mEnv.set_lk_max_objects(1000000);
    mEnv.set_lk_detect(DB_LOCK_DEFAULT);
        
    // Set the size of the shared memory buffer pool to 128KB (needed for RECOVERY, less would be better)
    //mEnv.set_cachesize(0, 128 * 1024, 4);
    //mEnv.set_cachesize(2, 512 * 1024 * 1024, 4);
    
    // Initialize the database environment
    try {
        mEnv.open( mEnvName.c_str(),   // the name of the environment
                  DB_CREATE |          // create environment files if they don't exist
                  DB_INIT_LOCK |       
                  DB_INIT_LOG |
                  DB_INIT_MPOOL |
                  DB_INIT_TXN |        // to support transaction processing.
                  DB_THREAD |          // needed for multi-thread supported
                  DB_REGISTER |
                  DB_RECOVER,
                  0666 );
        
        // Start a checkpoint thread.
        //boost_checkpoint_thread = boost::thread(&checkpoint_thread, &mEnv);
        
        // Start a logfile removal thread.
        //boost_logfile_thread = boost::thread(&logfile_thread, &mEnv);

    } catch (DbException & e) {
        printf("Open environment failed: %s", e.what());
        throw e;
    }
    
    // Sets the maximum file size, in bytes, for a file to be mapped into the process address space
    // Default: 10MB
    mEnv.set_mp_mmapsize(2 * 1024 * 1024 * 1024);

    fDbEnvInit = true;
}

BDBConnector::~BDBConnector()
{
    //boost_checkpoint_thread.interrupt();
    //boost_logfile_thread.interrupt();
    
    this->close();
}

void BDBConnector::close()
{    
    if (!fDbEnvInit)
        return;
    
    fDbEnvInit = false;
    
    //Close secondary indexes
    std::map<string, Db*>::iterator it = mIndexes.begin();
    for ( it = mIndexes.begin(); it != mIndexes.end(); ++it )
        try {
            closeTable_i( it->second );
        } catch (DbException e) {
            throw e;
        }
    mIndexes.clear();
    
    //Close primary databases
    it = mTables.begin();
    for ( it = mTables.begin(); it != mTables.end(); ++it )
        try {
            closeTable_i( it->second );
        } catch (DbException e) {
            throw e;
        }
    mTables.clear();
    
    //Flush database and remove log files
    mEnv.txn_checkpoint(0, 0, DB_FORCE);
    char **list;
    mEnv.log_archive(&list, DB_ARCH_REMOVE);
    
    //Close the environment
    mEnv.close(0);
}

void BDBConnector::initCoreDB( DbTxn* txn )
{
    int i;
    vector<string> staticDescriptors    = getStaticDescriptors();
#ifndef DATABASE_LIGHT_VERSION
    vector<string> dynamicDescriptors   = getDynamicDescriptors();
#endif
    
    try {
        createTable_i("symbolics", txn);
        createIndex_i("note", "symbolics", txn, noteExtractor, true);
        createIndex_i("name", "symbolics", txn, nameExtractor);
        createIndex_i("instrument", "symbolics", txn, instrumentExtractor, true);
        createIndex_i("source", "symbolics", txn, sourceExtractor, true);
        createIndex_i("dynamics", "symbolics", txn, dynamicsExtractor, true);
        createIndex_i("playingStyle", "symbolics", txn, playingStyleExtractor, true);
        createIndex_i("stringMute", "symbolics", txn, stringMuteExtractor, true);
        createIndex_i("brassMute", "symbolics", txn, brassMuteExtractor, true);
        createIndex_i("family", "symbolics", txn, familyExtractor, true);
        createIndex_i("duration", "symbolics", txn, durationExtractor, true, true);
        createIndex_i("MIDIcent", "symbolics", txn, MIDIcentExtractor, true, true);
        createTable_i("ADSR", txn, false, false, true);
        createTable_i("partials", txn);
        createIndex_i("PartialsMeanEnergy", "partials", txn, partialsMeanEnergyExtractor, true, true);
        createTable_i("melbands", txn);
        createIndex_i("LoudnessFactor", "melbands", txn, loudnessFactorExtractor, true, true);

        for (i = 0; i < staticDescriptors.size(); i++)
        {
            createTable_i(staticDescriptors[i], txn);
            string name = staticDescriptors[i] + "Mean";
            createIndex_i(name, staticDescriptors[i], txn, meanExtractor, true, true);
        }
#ifndef DATABASE_LIGHT_VERSION
        for (i = 0; i < dynamicDescriptors.size(); i++)
            createTable_i(dynamicDescriptors[i], txn);
#endif
    } catch (DbException e) {
        throw e;
    }
}

Dbc* BDBConnector::getCursor( string table, DbTxn* txn )
{
    Db* db = getTable_i( table );
    if ( !db )
    {
        printf("Table '%s' not found\n", table.c_str());
        return NULL;
    }
    Dbc* dbc;
    db->cursor( txn, &dbc, 0 );
    return dbc;
}

int BDBConnector::get_current_index( DbTxn* txn )
{
    int index = 0;
    Dbt key, vald;
    Dbc* dbc = getCursor("ADSR", txn);
    //get the last one
    if ( !dbc->get( &key, &vald, DB_LAST ))
        index = *(int*)key.get_data();
    dbc->close();
    return ++index;
}

void BDBConnector::createTable( string table, bool allowDuplicates, bool set_float_compare, bool set_int_compare )
{
    try {
        createTable_i( table, NULL, allowDuplicates, set_float_compare, set_int_compare );
    } catch (DbException e) { throw e; }
}

void BDBConnector::createTable_i( string table, DbTxn* txn, bool allowDuplicates, bool set_float_compare, bool set_int_compare )
{
    Db* db;
    try {
        db = openTable_i( table, txn, allowDuplicates, set_float_compare, set_int_compare );
        mTables.insert( pair<string, Db*>(table, db) );
    } catch (DbException e) {
        delete db;
        throw e;
    }
}

Db* BDBConnector::openTable_i( string table, DbTxn* txn, bool allowDuplicates, bool set_float_compare, bool set_int_compare )
{
    // open the database
    Db* db = new Db( &mEnv, DB_CXX_NO_EXCEPTIONS );
    
    if ( allowDuplicates )
        db->set_flags( DB_DUP | DB_DUPSORT );
    
    if ( set_float_compare )
        db->set_bt_compare(compare_float_keys);
    
    if ( set_int_compare )
        db->set_bt_compare(compare_int_keys);

    int flags = DB_CREATE | DB_THREAD; // | DB_AUTO_COMMIT;
    
    try {
        db->open( txn, table.c_str(), table.c_str(),
                 DB_BTREE, flags, 0666 );
    } catch (DbException e) {
        delete db;
        throw e;
    }
    
    return db;
}

void BDBConnector::closeTable_i( Db* db )
{
    try {
        db->close( 0 );
        delete db;
    } catch (DbException e) { throw e; }
}

Db* BDBConnector::getTable_i( string table )
{
    std::map<string, Db*>::iterator it = mTables.find(table);
    if ( it != mTables.end() )
        return it->second;

    it = mIndexes.find(table);
    if ( it != mIndexes.end() )
        return it->second;

    return NULL;
}

bool BDBConnector::addSoundDirectory( string path, string source, string instru )
{
    return add_i_soundDirectory(path, source, instru, NULL);
}

bool BDBConnector::add_i_sound(string soundfile, string source, string instru, DbTxn *txn)
{
    int i;
    cout << "Analysis of : " << soundfile << endl;
    sqlStruct snd;
    if (analyzeSound(soundfile, defaultAnalysisParams(), snd, source))
    {
        if (fDbEnvInit)
        {
            cout << "Add sound " << soundfile << endl;
            //Get key count
            int c = get_current_index( txn );
            
            //Create primary key
            Dbt key;
            init_dbt(&key, &c, sizeof(int));
            
            //Put sqlStruct into tables
            if (snd.source != "SOL")
                snd.instrument = instru;
            symbolics s = setSymbolics(snd);
            try {
                this->put_i_symbolics("symbolics", txn, &key, s);
                for (i = 0; i < snd.descriptors.size(); i++)
                    if (snd.descriptors[i].resampled_val->size() != 0)
                        this->put_i_descriptor(snd.descriptors[i].name, txn, &key, snd.descriptors[i]);
                this->put_i_partials("partials", txn, &key, snd.partials);
                this->put_i_melBands("melbands", txn, &key, snd.melbands);
                this->put_i_adsr("ADSR", txn, &key, snd.ADSR);
            } catch (const char* e) {
                cout << e << endl;
                snd.deallocate();
                return false;
            }
        }
        else
        {
            printf("Error : Environment not opened yet...\n");
            return false;
        }
        //Free memory
        snd.deallocate();
    }
    else
    {
        printf("Error : Analysis went wrong...\n");
        return false;
    }
    
    return true;
}

bool BDBConnector::add_i_sound_style(string soundfile, string source, string instru, string style, DbTxn *txn)
{
    int i;
    cout << "Analysis of : " << soundfile << endl;
    sqlStruct snd;
    if (analyzeSound(soundfile, defaultAnalysisParams(), snd, source))
    {
        if (fDbEnvInit)
        {
            cout << "Add sound " << soundfile << endl;
            //Get key count
            int c = get_current_index( txn );
            
            //Create primary key
            Dbt key;
            init_dbt(&key, &c, sizeof(int));
            
            //Put sqlStruct into tables
            if (snd.source != "SOL")
                snd.instrument = instru;
            snd.playingStyle = style;
            symbolics s = setSymbolics(snd);
            try {
                this->put_i_symbolics("symbolics", txn, &key, s);
                for (i = 0; i < snd.descriptors.size(); i++)
                    if (snd.descriptors[i].resampled_val->size() != 0)
                        this->put_i_descriptor(snd.descriptors[i].name, txn, &key, snd.descriptors[i]);
                this->put_i_partials("partials", txn, &key, snd.partials);
                this->put_i_melBands("melbands", txn, &key, snd.melbands);
                this->put_i_adsr("ADSR", txn, &key, snd.ADSR);
            } catch (const char* e) {
                cout << e << endl;
                snd.deallocate();
                return false;
            }
        }
        else
        {
            printf("Error : Environment not opened yet...\n");
            return false;
        }
        //Free memory
        snd.deallocate();
    }
    else
    {
        printf("Error : Analysis went wrong...\n");
        return false;
    }
    
    return true;
}

bool BDBConnector::add_i_soundDirectory_OSC_style ( string path, string source, string instru, string instruShort, string pStyle, DbTxn* txn )
{
    boost::filesystem::recursive_directory_iterator it;
    try
    {
        it = boost::filesystem::recursive_directory_iterator(boost::filesystem::path(path));
    }
    catch(boost::filesystem::filesystem_error& fex)
    {
        std::cout << fex.what() << std::endl;
        return false;
    }
    boost::filesystem::recursive_directory_iterator end;
    
    while(it != end)
    {
        if(boost::filesystem::is_directory(*it) && boost::filesystem::is_symlink(*it))
            it.no_push();
        
        boost::filesystem::path sound = *it;
        if (!boost::filesystem::is_directory(sound))
        {
            vector<string> formats = getSupportedFormats();
            if (std::find(formats.begin(), formats.end(), sound.extension().string()) != formats.end())
            {
                if (!add_i_sound_style(sound.string(), source, instruShort, pStyle, txn))
                    cout << "Add " << sound.string() << " went wrong..." << endl;
            }
        }
        try
        {
            ++it;
        }
        catch(std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            it.no_push();
            try { ++it; } catch(...) { std::cout << "!!" << std::endl; return false; }
        }
    }
    
    return true;
}

bool BDBConnector::add_i_soundDirectory_OSC_recurse( string path, string source, string instru, string instruShort, DbTxn* txn )
{
    boost::filesystem::recursive_directory_iterator it;
    try
    {
        it = boost::filesystem::recursive_directory_iterator(boost::filesystem::path(path));
    }
    catch(boost::filesystem::filesystem_error& fex)
    {
        std::cout << fex.what() << std::endl;
        return false;
    }
    boost::filesystem::recursive_directory_iterator end;
    
    while(it != end)
    {
        if(boost::filesystem::is_directory(*it) && boost::filesystem::is_symlink(*it))
            it.no_push();
        
        boost::filesystem::path sound = *it;
        if (!boost::filesystem::is_directory(sound))
        {
            vector<string> formats = getSupportedFormats();
            if (std::find(formats.begin(), formats.end(), sound.extension().string()) != formats.end())
            {
                if (!add_i_sound(sound.string(), source, instruShort, txn))
                    cout << "Add " << sound.string() << " went wrong..." << endl;
            }
        }
        try
        {
            ++it;
        }
        catch(std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            it.no_push();
            try { ++it; } catch(...) { std::cout << "!!" << std::endl; return false; }
        }
    }
    
    return true;
}

bool BDBConnector::add_i_soundDirectory ( string path, string source, string instru, DbTxn* txn )
{
    boost::filesystem::recursive_directory_iterator it;
    try
    {
        it = boost::filesystem::recursive_directory_iterator(boost::filesystem::path(path));
    }
    catch(boost::filesystem::filesystem_error& fex)
    {
        std::cout << fex.what() << std::endl;
        return false;
    }
    boost::filesystem::recursive_directory_iterator end;
    
    while(it != end)
    {
        if(boost::filesystem::is_directory(*it) && boost::filesystem::is_symlink(*it))
            it.no_push();
        
        boost::filesystem::path sound = *it;
        if (!boost::filesystem::is_directory(sound))
        {
            vector<string> formats = getSupportedFormats();
            if (std::find(formats.begin(), formats.end(), sound.extension().string()) != formats.end())
            {
                if (!add_i_sound(sound.string(), source, instru, txn))
                    cout << "Add " << sound.string() << " went wrong..." << endl;
            }
        }
        try
        {
            ++it;
        }
        catch(std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            it.no_push();
            try { ++it; } catch(...) { std::cout << "!!" << std::endl; return false; }
        }
    }
    
    return true;
}

bool BDBConnector::addSound( string filepath, string source, string instru )
{
    return add_i_sound(filepath, source, instru, NULL);
}

void BDBConnector::putSymbolics( string table, Dbt *key, symbolics val, bool update )
{
    try {
        put_i_symbolics( table, NULL, key, val, update );
    } catch (DbException e) { throw e; };
}

void BDBConnector::putMelBands( string table, Dbt *key, melStruct& val, bool update )
{
    try {
        put_i_melBands( table, NULL, key, val, update );
    } catch (DbException e) { throw e; };
}

void BDBConnector::putPartials( string table, Dbt *key, partials& val, bool update )
{
    try {
        put_i_partials( table, NULL, key, val, update );
    } catch (DbException e) { throw e; };
}

void BDBConnector::putDescriptor( string table, Dbt *key, feature<float>& val, bool update )
{
    try {
        put_i_descriptor( table, NULL, key, val, update );
    } catch (DbException e) { throw e; };
}

void BDBConnector::put_i_symbolics( string table, DbTxn* txn, Dbt *key,
                                   symbolics val, bool update )
{
    Db* db = getTable_i( table );
    Dbt vald( &val, sizeof(val) );
    
    int ret;
    int flags = 0;
    if ( !update ) flags |= DB_NOOVERWRITE;
    
    try {
        ret = db->put( txn, key, &vald, flags );
    } catch (DbException e) {
        throw e;
    }

    if ( ret == DB_KEYEXIST ) printf("Key already exists");
}

void BDBConnector::put_i_melBands( string table, DbTxn* txn, Dbt *key,
                         melStruct& val, bool update )
{
    Db* db = getTable_i( table );

    u_int32_t size = 0;
    if ( (size = getMelBandSize(val)) == 0 )
        throw "Error: Wrong mel bands size";
    
    char *buffer = (char*)malloc(size);
    int buffer_length = recordMelBands(val, buffer);
    Dbt vald( buffer, buffer_length );
    
    int ret;
    int flags = 0;
    if ( !update ) flags |= DB_NOOVERWRITE;
    
    try {
        ret = db->put( txn, key, &vald, flags );
    } catch (DbException e) {
        throw e;
    }
    
    if ( ret == DB_KEYEXIST ) printf("Key already exists");
    free(buffer);
}

void BDBConnector::put_i_partials( string table, DbTxn* txn, Dbt *key,
                                  partials& val, bool update )
{
    Db* db = getTable_i( table );

    u_int32_t size = 0;
    if ( (size = getPartialSize(val)) == 0 )
        throw "Error: Wrong partials size";
    
    char *buffer = (char*)malloc(size);
    int buffer_length = recordPartials(val, buffer);
    Dbt vald( buffer, buffer_length );

    int ret;
    int flags = 0;
    if ( !update ) flags |= DB_NOOVERWRITE;
    
    try {
        ret = db->put( txn, key, &vald, flags );
    } catch (DbException e) {
        throw e;
    }
    
    if ( ret == DB_KEYEXIST ) printf("Key already exists");
    free(buffer);
}

void BDBConnector::put_i_descriptor( string table, DbTxn* txn, Dbt *key,
                                  feature<float>& val, bool update )
{
    Db* db = getTable_i( table );
    if (db == NULL)
        return;

    u_int32_t size = 0;
    if ( (size = getDescriptorSize(val)) == 0 )
        throw "Error: Wrong descriptor size";
    
    char *buffer = (char*)malloc(size);
    int buffer_length = recordDescriptor(val, buffer);
    Dbt vald( buffer, buffer_length );
    
    int ret;
    int flags = 0;
    if ( !update ) flags |= DB_NOOVERWRITE;
    
    try {
        ret = db->put( txn, key, &vald, flags );
    } catch (DbException e) {
        throw e;
    }
        
    if ( ret == DB_KEYEXIST ) printf("Key already exists");
    free(buffer);
}

void BDBConnector::put_i_adsr( string table, DbTxn* txn, Dbt *key, envelope val, bool update )
{
    Db* db = getTable_i( table );
    Dbt vald( &val, sizeof(val) );
    
    int ret;
    int flags = 0;
    if ( !update ) flags |= DB_NOOVERWRITE;
    
    try {
        ret = db->put( txn, key, &vald, flags );
    } catch (DbException e) {
        throw e;
    }
    
    if ( ret == DB_KEYEXIST ) printf("Key already exists");
}

void BDBConnector::del( string table, Dbt *key )
{
    try {
        del_i(table, NULL, key);
    } catch (DbException e) {
        throw e;
    }
}

void BDBConnector::del_i( string table, DbTxn* txn, Dbt *key )
{
    Db* db = getTable_i( table );

    try {
        db->del( txn, key, 0 );
    } catch (DbException e) {
        throw e;
    }
}

void BDBConnector::removeSound ( int soundID )
{
    remove_i_sound(soundID, NULL);
}

void BDBConnector::removeFamily( char* family )
{
    remove_i_family(family, NULL);
}

void BDBConnector::removeSoundDirectory( char* path )
{
    remove_i_soundDirectory(path, NULL);
}

bool BDBConnector::refreshSoundDirectory( char* path, char* source, vector<string> &filenames )
{
    return refresh_i_soundDirectory(path, source, filenames, NULL);
}

void BDBConnector::check_sounds_to_delete ( DbTxn* txn, string source, vector<string> &filenames )
{
    int i;
    //Retrieve all sound IDs for users sources
    vector<int> soundIDs;
    Dbt key;
    init_dbt(&key, source.c_str(), (int)strlen(source.c_str()) + 1);
    getSymbolics("source", &key, soundIDs);
    
    //If there is the same number of files in directories and database, then do nothing
    if (soundIDs.size() != filenames.size())
    {
        //For each sound, retrieve its name.
        //If sound name isn't in the vector of filenames, then remove the sound
        for (i = 0; i < soundIDs.size(); i++)
        {
            Dbt key;
            init_dbt(&key, &soundIDs[i], sizeof(int));
            symbolics s;
            getSymbolics("symbolics", &key, s);
            if (std::find(filenames.begin(), filenames.end(), s.name) == filenames.end())
                try {
                    this->remove_i_sound(soundIDs[i], txn);
                } catch (DbException e) {
                    throw e;
                }
        }
    }
}

void BDBConnector::remove_i_sound( int soundID, DbTxn* txn )
{
    int j;
    vector<string> staticDescriptors    = getStaticDescriptors();
#ifndef DATABASE_LIGHT_VERSION
    vector<string> dynamicDescriptors   = getDynamicDescriptors();
#endif

    Dbt key;
    init_dbt(&key, &soundID, sizeof(int));
    symbolics s;
    this->get_i_symbolics("symbolics", txn, &key, s);
    printf("Delete sound: %s\n", s.name);
    
    try {
        this->del_i("symbolics", txn, &key);
        this->del_i("partials", txn, &key);
        this->del_i("melbands", txn, &key);
        this->del_i("ADSR", txn, &key);
        for (j = 0; j < staticDescriptors.size(); j++)
            this->del_i(staticDescriptors[j], txn, &key);
#ifndef DATABASE_LIGHT_VERSION
        for (j = 0; j < dynamicDescriptors.size(); j++)
            this->del_i(dynamicDescriptors[j], txn, &key);
#endif
    } catch (DbException e) {
        throw e;
    }
}

void BDBConnector::remove_i_family ( char* family, DbTxn* txn )
{
    int         i;
    vector<int> soundIDs;
    Dbt         key;
    init_dbt(&key, family, (int)(strlen(family) + 1));
    this->get_i_symbolics("instrument", txn, &key, soundIDs);
    
    try {
        for (i = 0; i < soundIDs.size(); i++)
            this->remove_i_sound(soundIDs[i], txn);
    } catch (DbException e) {
        throw e;
    }
}

void BDBConnector::remove_i_soundDirectory ( char* path, DbTxn* txn )
{
    vector<string> formats = getSupportedFormats();
    boost::filesystem::recursive_directory_iterator it;
    try
    {
        it = boost::filesystem::recursive_directory_iterator(boost::filesystem::path(path));
    }
    catch(boost::filesystem::filesystem_error& fex)
    {
        std::cout << fex.what() << std::endl;
        return;
    }
    boost::filesystem::recursive_directory_iterator end;
    
    while(it != end)
    {
        if(boost::filesystem::is_directory(*it) && boost::filesystem::is_symlink(*it))
            it.no_push();
        
        boost::filesystem::path sound = *it;
        if (!boost::filesystem::is_directory(sound))
        {
            if (std::find(formats.begin(), formats.end(), sound.extension().string()) != formats.end())
            {
                string filepath = sound.string();
                string filename = getFileName<string>(filepath);
                try {
                    int index = this->get_i_index(filename, txn);
                    this->remove_i_sound(index, txn);
                } catch (DbException e) {
                    throw e;
                }
            }
        }
        try
        {
            ++it;
        }
        catch(std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            it.no_push();
            try { ++it; } catch(...) { std::cout << "!!" << std::endl; return; }
        }
    }
}

bool BDBConnector::refresh_i_soundDirectory( string path, string source, vector<string> &filenames, DbTxn* txn )
{    
    vector<string> formats = getSupportedFormats();
    boost::filesystem::recursive_directory_iterator it;
    try
    {
        it = boost::filesystem::recursive_directory_iterator(boost::filesystem::path(path));
    }
    catch(boost::filesystem::filesystem_error& fex)
    {
        std::cout << fex.what() << std::endl;
        return false;
    }
    boost::filesystem::recursive_directory_iterator end;
    
    while(it != end)
    {
        if(boost::filesystem::is_directory(*it) && boost::filesystem::is_symlink(*it))
            it.no_push();
        
        boost::filesystem::path sound = *it;
        if (!boost::filesystem::is_directory(sound))
        {
            if (std::find(formats.begin(), formats.end(), sound.extension().string()) != formats.end())
            {
                string filepath = sound.string();
                string filename = getFileName<string>(filepath);
                filenames.push_back(filename);
                // Check if sound exists or not
                Db* db = getTable_i("name");
                Dbt key;
                init_dbt(&key, filename.c_str(), (int)strlen(filename.c_str()) + 1);
                try {
                    if ( db->exists(txn, &key, 0) == DB_NOTFOUND )
                    {
                        // Retrieve the instrument of this source
                        symbolics   val;
                        Dbt         source_key;
                        init_dbt(&source_key, source.c_str(), (int)strlen(source.c_str()) + 1);
                        get_i_symbolics("source", txn, &source_key, val);
                        // Add sound to database
                        if ( !this->add_i_sound(filepath, source, val.instrument, txn) ) return false;
                    }
                } catch (DbException e) {
                    cout << e.what() << endl;
                }
            }
        }
        try
        {
            ++it;
        }
        catch(std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
            it.no_push();
            try { ++it; } catch(...) { std::cout << "!!" << std::endl; return false; }
        }
    }
    
    return true;
}

int BDBConnector::getIndex( string filename )
{
    return get_i_index(filename, NULL);
}

bool BDBConnector::getSymbolics( string table, Dbt *key, symbolics& val )
{
    return get_i_symbolics(table, NULL, key, val);
}

bool BDBConnector::getSymbolics( string table, Dbt *key, vector<int>& IDs )
{
    return get_i_symbolics(table, NULL, key, IDs);
}

bool BDBConnector::getSymbolicsByRange( string table, float start, float end, vector<int>& soundIDs )
{
    return get_i_symbolicsByRange( table, NULL, start, end, soundIDs );
}

bool BDBConnector::getMelBands( string table, Dbt *key, melStruct& val )
{
    return get_i_melBands(table, NULL, key, val);
}

bool BDBConnector::getPartials( string table, Dbt *key, partials& val )
{
    return get_i_partials(table, NULL, key, val);
}

bool BDBConnector::getDescriptor( string table, Dbt *key, feature<float>& val )
{
    return get_i_descriptor(table, NULL, key, val);
}

bool BDBConnector::getDescriptorByMeanRange( string table, float start, float end, vector<int>& soundIDs )
{
    return get_i_descriptorByMeanRange( table, NULL, start, end, soundIDs);
}

bool BDBConnector::getDescriptorMinMax ( string descriptor, float& min, float& max )
{
    return get_i_descriptorMinMax(NULL, descriptor, min, max);
}

bool BDBConnector::getSoundsQuery ( vector<Query> queries, vector<int>& soundIDs )
{
    return get_i_soundsQuery(NULL, queries, soundIDs);
}

bool BDBConnector::getTemporalQueryDTW ( vector<float>& descValue, string descName, vector<int>& soundIDs )
{
    return get_i_temporalQueryDTW(NULL, descValue, descName, soundIDs);
}

bool BDBConnector::getSecondaryIndexList ( string table, vector<string>& list )
{
    return get_i_secondaryIndexList(NULL, table, list);
}

vector<string> BDBConnector::getNames ( vector<int>& soundIDs )
{
    return get_i_names(NULL, soundIDs);
}

vector<string> BDBConnector::getPaths ( vector<int>& soundIDs, vector<bool>& isSOL )
{
    return get_i_paths(NULL, soundIDs, isSOL);
}

int BDBConnector::get_i_index( string filename, DbTxn* txn )
{
    int index = 0;
    Dbt key, pkey, vald;
    init_dbt(&key, filename.c_str(), (int)strlen(filename.c_str()) + 1);
    memset(&pkey, 0, sizeof(Dbt));
    memset(&vald, 0, sizeof(Dbt));

    Dbc* dbc = getCursor("name", txn);
    if ( !(dbc->pget(&key, &pkey, &vald, DB_SET)) )
        index = *(int*)pkey.get_data();
    
    dbc->close();
    return index;
}

bool BDBConnector::get_i_symbolics( string table, DbTxn* txn, Dbt *key, symbolics& val )
{
    Dbt vald;
    Dbc* dbc = getCursor(table, txn);
    if (dbc == NULL)
        return false;
    
    int ret;
    if (( ret = dbc->get( key, &vald, DB_SET )))
    {
        if ( ret == DB_NOTFOUND )
        {
            dbc->close();
            return false;
        }
    } else
        val = *(symbolics*)vald.get_data();
    
    dbc->close();
    return true;
}

vector<bool> BDBConnector::get_i_symbolicsOptimized( string table, DbTxn* txn, vector<int>& IDs, vector<symbolics>& values )
{
    int             i;
    Dbc*            dbc = getCursor(table, txn);
    vector<bool>    found;
    for (i = 0; i < IDs.size(); i++)
    {
        Dbt         key;
        Dbt         vald;
        init_dbt(&key, &IDs[i], sizeof(int));
        memset(&vald, 0, sizeof(Dbt));
        int         ret;
        symbolics   val;
        if (( ret = dbc->get( &key, &vald, DB_SET)))
        {
            if ( ret == DB_NOTFOUND )
            {
                //printf("ID %i not found\n", IDs[i]);
                found.push_back(false);
            }
        }
        else
        {
            val = *(symbolics*)vald.get_data();
            found.push_back(true);
        }
        values.push_back(val);
    }
    
    dbc->close();
    return found;
}

bool BDBConnector::get_i_symbolics( string table, DbTxn* txn, Dbt *key, vector<int>& IDs )
{
    Dbt vald;
    Dbt pkey;
    memset(&pkey, 0, sizeof(Dbt));
    Dbc* dbc = getCursor(table, txn);
    if (dbc == NULL)
        return false;
    
    // get the first one
    if ( !dbc->pget( key, &pkey, &vald, DB_SET ))
    {
        IDs.push_back( *(int*)pkey.get_data() );
        
        // get any additional duplicates
        while( !dbc->pget( key, &pkey, &vald, DB_NEXT_DUP ))
            IDs.push_back( *(int*)pkey.get_data() );
    }
    
    dbc->close();
    return true;
}

bool BDBConnector::get_i_symbolicsByRange ( string table, DbTxn* txn, float start, float end, vector<int>& soundIDs )
{
    Dbt key_dbt, pkey, data_dbt;
    // Set the starting point.
    init_dbt(&key_dbt, &start, sizeof(float));
    memset(&data_dbt, 0, sizeof(Dbt));
    memset(&pkey, 0, sizeof(Dbt));
    
    // Now, create a cursor.
    Dbc* dbc = getCursor(table, txn);
    if (dbc == NULL)
        return false;
    
    int ret;
    for (ret = dbc->pget(&key_dbt, &pkey, &data_dbt, DB_SET_RANGE);
         ret == 0;
         ret = dbc->pget(&key_dbt, &pkey, &data_dbt, DB_NEXT))
    {
        // Check if we are still in the range.
        if (*(float*)key_dbt.get_data() > end)
            break;
        soundIDs.push_back(*(int*)pkey.get_data());
    }
    
    dbc->close();
    return true;
}

bool BDBConnector::get_i_melBands( string table, DbTxn* txn, Dbt *key, melStruct& val )
{
    Dbt vald;
    Dbc* dbc = getCursor(table, txn);
    if (dbc == NULL)
        return false;
    
    int ret;
    if (( ret = dbc->get( key, &vald, DB_SET )))
    {
        if ( ret == DB_NOTFOUND )
        {
            dbc->close();
            return false;
        }
    }
    else
        restoreMelBands(val, vald);
    
    dbc->close();
    return true;
}

vector<bool> BDBConnector::get_i_melBandsOptimized( string table, DbTxn* txn, vector<int>& IDs, vector<melStruct>& values )
{
    int             i;
    Dbc*            dbc = getCursor(table, txn);
    vector<bool>    found;
    for (i = 0; i < IDs.size(); i++)
    {
        Dbt         key;
        Dbt         vald;
        init_dbt(&key, &IDs[i], sizeof(int));
        memset(&vald, 0, sizeof(Dbt));
        int         ret;
        melStruct   val;
        if (( ret = dbc->get( &key, &vald, DB_SET)))
        {
            if ( ret == DB_NOTFOUND )
            {
                //printf("ID %i not found\n", IDs[i]);
                found.push_back(false);
            }
        }
        else
        {
            restoreMelBands(val, vald);
            found.push_back(true);
        }
        values.push_back(val);
    }
    dbc->close();
    return found;
}

bool BDBConnector::get_i_partials( string table, DbTxn* txn, Dbt *key, partials& val )
{
    Dbt vald;
    Dbc* dbc = getCursor(table, txn);
    if (dbc == NULL)
        return false;
    
    int ret;
    if (( ret = dbc->get( key, &vald, DB_SET )))
    {
        if ( ret == DB_NOTFOUND )
        {
            dbc->close();
            return false;
        }
    }
    else
        restorePartials(val, vald);
    
    dbc->close();
    return true;
}

vector<bool> BDBConnector::get_i_partialsOptimized( string table, DbTxn* txn, vector<int>& IDs, vector<partials>& values )
{
    int             i;
    Dbc*            dbc = getCursor(table, txn);
    vector<bool>    found;
    for (i = 0; i < IDs.size(); i++)
    {
        Dbt         key;
        Dbt         vald;
        init_dbt(&key, &IDs[i], sizeof(int));
        memset(&vald, 0, sizeof(Dbt));
        int         ret;
        partials    val;
        if (( ret = dbc->get( &key, &vald, DB_SET)))
        {
            if ( ret == DB_NOTFOUND )
            {
                //printf("ID %i not found\n", IDs[i]);
                found.push_back(false);
            }
        }
        else
        {
            restorePartials(val, vald);
            found.push_back(true);
        }
        values.push_back(val);
    }
    
    dbc->close();
    return found;
}

bool BDBConnector::get_i_descriptor( string table, DbTxn* txn, Dbt *key, feature<float>& val )
{
    Dbt vald;
    memset(&vald, 0, sizeof(Dbt));
    Dbc* dbc = getCursor(table, txn);
    if (dbc == NULL)
        return false;
    
    int ret;
    if (( ret = dbc->get( key, &vald, DB_SET)))
    {
        if ( ret == DB_NOTFOUND )
        {
            dbc->close();
            return false;
        }
    }
    else
        restoreDescriptor(val, vald);
    
    val.name = table;
    dbc->close();
    return true;
}

vector<bool> BDBConnector::get_i_descriptorOptimized( string table, DbTxn* txn, vector<int>& IDs, vector<feature<float> >& values )
{
    int             i;
    Dbc*            dbc = getCursor(table, txn);
    vector<bool>    found;
    for (i = 0; i < IDs.size(); i++)
    {
        Dbt key;
        Dbt vald;
        init_dbt(&key, &IDs[i], sizeof(int));
        memset(&vald, 0, sizeof(Dbt));
        int ret;
        feature<float> f;
        if (( ret = dbc->get( &key, &vald, DB_SET)))
        {
            if ( ret == DB_NOTFOUND )
            {
                //printf("ID %i not found\n", IDs[i]);
                found.push_back(false);
            }
        }
        else
        {
            restoreDescriptor(f, vald);
            found.push_back(true);
        }
        values.push_back(f);
    }

    dbc->close();
    return found;
}

bool BDBConnector::get_i_descriptorByMeanRange( string table, DbTxn* txn, float start, float end, vector<int>& soundIDs )
{
    Dbt key_dbt, pkey, data_dbt;
    // Set the starting point.
    init_dbt(&key_dbt, &start, sizeof(float));
    memset(&data_dbt, 0, sizeof(Dbt));
    memset(&pkey, 0, sizeof(Dbt));
    
    // Now, create a cursor.
    Dbc* dbc = getCursor(table, txn);
    if (dbc == NULL)
        return false;
    
    int ret;
    for (ret = dbc->pget(&key_dbt, &pkey, &data_dbt, DB_SET_RANGE);
         ret == 0;
         ret = dbc->pget(&key_dbt, &pkey, &data_dbt, DB_NEXT))
    {
        // Check if we are still in the range.
        if (*(float*)key_dbt.get_data() > end)
            break;
        
        soundIDs.push_back(*(int*)pkey.get_data());
    }
    
    dbc->close();
    return true;
}

bool BDBConnector::get_i_descriptorMinMax ( DbTxn* txn, string descriptor, float& min, float& max )
{
    Dbt key, vald;
    Dbc* dbc = getCursor(descriptor, txn);
    if (dbc == NULL)
        return false;
    
    // get the first one
    if ( !dbc->get( &key, &vald, DB_FIRST ))
        min = *(float*)key.get_data();
    
    //get the last one
    if ( !dbc->get( &key, &vald, DB_LAST ))
        max = *(float*)key.get_data();
    
    dbc->close();
    return true;
}

bool BDBConnector::get_i_multipleFinalOptimized ( DbTxn* txn, vector<int>& IDs, vector<string>& updateFeatures, vector<int>& typesIds, map<int, vector<matrixPtr> >& features, bool denormalized )
{
    int i, j;
    // Get all features types
    vector<string> types = getTypesList();
    for (i = 0; i < updateFeatures.size(); i++)
    {
        string          fName   = updateFeatures[i];
        string          type    = types[typesIds[i]];
        vector<bool>    found;
        // Get melbands
        if (fName == "LoudnessFactor")
        {
            vector<melStruct> values;
            found = get_i_melBandsOptimized("melbands", txn, IDs, values);
            for (j = 0; j < IDs.size(); j++)
            {
                matrixPtr t(new matrix(1, 1));
                if (found[j])
                    t->operator()(0, 0) = values[j].loudnessFactor;
                features[IDs[j]].push_back(t);
            }
        }
        else if (fName == "MelAmplitude")
        {
            vector<melStruct> values;
            found = get_i_melBandsOptimized("melbands", txn, IDs, values);
            for (j = 0; j < IDs.size(); j++)
                if (found[j])
                    features[IDs[j]].push_back(values[j].melAmplitude);
                else
                {
                    matrixPtr t(new matrix(70, 64));
                    features[IDs[j]].push_back(t);
                }
        }
        else if (fName == "MelMeanAmplitude")
        {
            vector<melStruct> values;
            found = get_i_melBandsOptimized("melbands", txn, IDs, values);
            for (j = 0; j < IDs.size(); j++)
                if (found[j])
                    features[IDs[j]].push_back(values[j].melMeanAmp);
                else
                {
                    matrixPtr t(new matrix(70, 1));
                    features[IDs[j]].push_back(t);
                }
        }
        else if (fName == "MelEnergy")
        {
            vector<melStruct> values;
            found = get_i_melBandsOptimized("melbands", txn, IDs, values);
            for (j = 0; j < IDs.size(); j++)
                if (found[j])
                    features[IDs[j]].push_back(values[j].melEnergy);
                else
                {
                    matrixPtr t(new matrix(1, 64));
                    features[IDs[j]].push_back(t);
                }
        }
        else if (fName == "MelMeanEnergy")
        {
            vector<melStruct> values;
            found = get_i_melBandsOptimized("melbands", txn, IDs, values);
            for (j = 0; j < IDs.size(); j++)
            {
                matrixPtr t(new matrix(1, 1));
                if (found[j])
                    t->operator()(0, 0) = values[j].melMeanEnergy;
                features[IDs[j]].push_back(t);
            }
        }
        // Get symbolics
        else if (fName == "duration" || fName == "MIDIcent")
        {
            vector<symbolics> values;
            found = get_i_symbolicsOptimized("symbolics", txn, IDs, values);
            for (j = 0; j < IDs.size(); j++)
            {
                matrixPtr t(new matrix(1, 1));
                if (found[j])
                    t->operator()(0, 0) = boost::any_cast<float>(values[j].getMember(fName));
                features[IDs[j]].push_back(t);
            }
        }
        // Get partials
        else if (type == "array" || type == "array2D" || fName == "PartialsMeanEnergy")
        {
            vector<partials> values;
            found = get_i_partialsOptimized("partials", txn, IDs, values);
            if (fName == "PartialsFrequency")
            {
                for (j = 0; j < IDs.size(); j++)
                {
                    if (found[j])
                    {
                        features[IDs[j]].push_back(values[j].partialsFrequency);
                        values[j].deallocate();
                    }
                    else
                    {
                        matrixPtr t(new matrix(25, 64));
                        features[IDs[j]].push_back(t);
                    }
                }
            }
            else if (fName == "PartialsAmplitude")
            {
                for (j = 0; j < IDs.size(); j++)
                    if (found[j])
                    {
                        features[IDs[j]].push_back(values[j].partialsAmplitude);
                        values[j].deallocate();
                    }
                    else
                    {
                        matrixPtr t(new matrix(25, 64));
                        features[IDs[j]].push_back(t);
                    }
            }
            else if (fName == "PartialsMeanFrequency")
            {
                for (j = 0; j < IDs.size(); j++)
                {
                    if (found[j])
                    {
                        matrixPtr partialsMeanFrequency(new matrix(values[j].partialsMeanFrequency, true, values[j].partialsMeanFrequencySize, 1));
                        features[IDs[j]].push_back(partialsMeanFrequency);
                        free(values[j].partialsMeanAmplitude);
                        free(values[j].partialsEnergy);
                    }
                    else
                    {
                        matrixPtr t(new matrix(25, 1));
                        features[IDs[j]].push_back(t);
                    }
                }
            }
            else if (fName == "PartialsMeanAmplitude")
            {
                for (j = 0; j < IDs.size(); j++)
                {
                    if (found[j])
                    {
                        matrixPtr partialsMeanAmplitude(new matrix(values[j].partialsMeanAmplitude, true, values[j].partialsMeanAmplitudeSize, 1));
                        features[IDs[j]].push_back(partialsMeanAmplitude);
                        free(values[j].partialsMeanFrequency);
                        free(values[j].partialsEnergy);
                    }
                    else
                    {
                        matrixPtr t(new matrix(25, 1));
                        features[IDs[j]].push_back(t);
                    }
                }
            }
            else if (fName == "PartialsEnergy")
            {
                for (j = 0; j < IDs.size(); j++)
                {
                    if (found[j])
                    {
                        matrixPtr partialsEnergy(new matrix(values[j].partialsEnergy, true, 1, values[j].partialsEnergySize));
                        features[IDs[j]].push_back(partialsEnergy);
                        free(values[j].partialsMeanAmplitude);
                        free(values[j].partialsMeanFrequency);
                    }
                    else
                    {
                        matrixPtr t(new matrix(1, 64));
                        features[IDs[j]].push_back(t);
                    }
                }
            }
            else if (fName == "PartialsMeanEnergy")
            {
                for (j = 0; j < IDs.size(); j++)
                {
                    matrixPtr t(new matrix(1, 1));
                    if (found[j])
                    {
                        t->operator()(0, 0) = values[j].partialsMeanEnergy;
                        values[j].deallocate();
                    }
                    features[IDs[j]].push_back(t);
                }
            }
        }
        // Get descriptor
        else
        {
            vector<feature<float> > values;
            string fNameCopy;
            if (fName.find("Mean") != string::npos)
                fNameCopy = fName.substr(0, fName.size() - 4);
            else if (fName.find("StdDev") != string::npos)
                fNameCopy = fName.substr(0, fName.size() - 6);
            else
                fNameCopy = fName;
            
            found = get_i_descriptorOptimized(fNameCopy, txn, IDs, values);
            if (fName.find("Mean") != string::npos)
            {
                for (j = 0; j < IDs.size(); j++)
                {
                    matrixPtr t(new matrix(1, 1));
                    if (found[j])
                        t->operator()(0, 0) = values[j].mean;
                    features[IDs[j]].push_back(t);
                }
            }
            else if (fName.find("StdDev") != string::npos)
            {
                for (j = 0; j < IDs.size(); j++)
                {
                    matrixPtr t(new matrix(1, 1));
                    if (found[j])
                        t->operator()(0, 0) = values[j].stdDev;
                    features[IDs[j]].push_back(t);
                }
            }
            else
            {
                for (j = 0; j < IDs.size(); j++)
                {
                    if (found[j])
                    {
                        values[j].resampled_val->transpose();
                        if (denormalized)
                        {
                            // Denormalisation
                            values[j].resampled_val->operator*(values[j].max - values[j].min);
                            values[j].resampled_val->operator+(values[j].min);
                        }
                        features[IDs[j]].push_back(values[j].resampled_val);
                    }
                    else
                    {
                        matrixPtr t(new matrix(1, 128));
                        features[IDs[j]].push_back(t);
                    }
                }
            }
        }
    }
    return true;
}

bool BDBConnector::get_i_soundsQuery ( DbTxn* txn, vector<Query> queries, vector<int>& soundIDs )
{
    int i, j;
    vector<int>::iterator it;
    for (i = 0; i < queries.size(); i++)
    {
        if (queries[i].getType() == "IS")
        {
            //Case: IS MUTED
            if (queries[i].getDescriptor() == "MUTED")
            {
                for (it = soundIDs.begin(); it != soundIDs.end();)
                {
                    Dbt key;
                    init_dbt(&key, &*it, sizeof(int));
                    symbolics s;
                    get_i_symbolics("symbolics", txn, &key, s);
                    //Delete not muted sounds
                    if ( (strcmp(s.brassMute, "NA") != 0 && (strcmp(s.brassMute, "N") != 0))
                        || (strcmp(s.stringMute, "NA") != 0 && strcmp(s.stringMute, "N") != 0) )
                        ++it;
                    else
                        it = soundIDs.erase(it);
                }
            }
            //Case: IS UNMUTED
            else if (queries[i].getDescriptor() == "UNMUTED")
            {
                for (it = soundIDs.begin(); it != soundIDs.end();)
                {
                    Dbt key;
                    init_dbt(&key, &*it, sizeof(int));
                    symbolics s;
                    get_i_symbolics("symbolics", txn, &key, s);
                    //Delete muted sounds
                    if ( (strcmp(s.brassMute, "NA") == 0 || (strcmp(s.brassMute, "N") == 0))
                        && (strcmp(s.stringMute, "NA") == 0 || strcmp(s.stringMute, "N") == 0) )
                        ++it;
                    else
                        it = soundIDs.erase(it);
                }
            }
            //Case: IS {string}
            else
            {
                //First query
                if (soundIDs.size() == 0)
                {
                    Dbt key;
                    char *val = (char*)boost::any_cast<string>(queries[i].getValue()).c_str();
                    init_dbt(&key, val, (int)(strlen(val) + 1));
                    get_i_symbolics(queries[i].getDescriptor(), txn, &key, soundIDs);
                }
                //Have already a list of soundIDs
                else
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        symbolics s;
                        get_i_symbolics("symbolics", txn, &key, s);
                        //Delete sounds that doesn't verify the condition
                        if ( boost::any_cast<string>(s.getMember(queries[i].getDescriptor()))
                            == boost::any_cast<string>(queries[i].getValue()))
                            ++it;
                        else
                            it = soundIDs.erase(it);
                    }
            }
        }
        //Case: REGEXP {string}
        else if (queries[i].getType() == "REGEXP")
        {
            boost::regex e("[|]|[ ]|[,]", boost::regex::normal | boost::regbase::icase);
            vector<string> l;
            string str = boost::any_cast<string>(queries[i].getValue());
            boost::regex_split(std::back_inserter(l), str, e);
            
            //Firt query
            if (soundIDs.size() == 0)
            {
                for (j = 0; j < l.size(); j++)
                {
                    Dbt key;
                    init_dbt(&key, l[j].c_str(), (int)(strlen(l[j].c_str()) + 1));
                    get_i_symbolics(queries[i].getDescriptor(), txn, &key, soundIDs);
                }
            }
            //Have already a list of soundIDs
            else
                for (it = soundIDs.begin(); it != soundIDs.end();)
                {
                    Dbt key;
                    init_dbt(&key, &*it, sizeof(int));
                    symbolics s;
                    get_i_symbolics("symbolics", txn, &key, s);
                    //Delete sounds that doesn't verify the condition
                    if ( std::find(l.begin(), l.end(), boost::any_cast<string>(s.getMember(queries[i].getDescriptor()))) != l.end() )
                        ++it;
                    else
                        it = soundIDs.erase(it);
                }
        }
        else if (queries[i].getType() == "UNDER")
        {
            float limit = boost::any_cast<pair<float, float> >(queries[i].getValue()).first;
            //Firt query
            if (soundIDs.size() == 0)
            {
                float min, max;
                get_i_descriptorMinMax(txn, queries[i].getDescriptor(), min, max);
                
                if (queries[i].getDescriptor() == "duration" || queries[i].getDescriptor() == "MIDIcent")
                    get_i_symbolicsByRange(queries[i].getDescriptor(), txn, min, limit, soundIDs);
                else
                    get_i_descriptorByMeanRange(queries[i].getDescriptor(), txn, min, limit, soundIDs);
            }
            //Have already a list of soundIDs
            else
            {
                if (queries[i].getDescriptor() == "MIDIcent" || queries[i].getDescriptor() == "duration")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        symbolics s;
                        get_i_symbolics("symbolics", txn, &key, s);
                        //Delete sounds that doesn't verify the condition
                        ( boost::any_cast<float>(s.getMember(queries[i].getDescriptor())) <= limit ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else if (queries[i].getDescriptor() == "LoudnessFactor")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        melStruct m;
                        get_i_melBands("melbands", txn, &key, m);
                        //Delete sounds that doesn't verify the condition
                        ( m.loudnessFactor <= limit ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else if (queries[i].getDescriptor() == "PartialsMeanEnergy")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        partials p;
                        get_i_partials("partials", txn, &key, p);
                        //Delete sounds that doesn't verify the condition
                        ( p.partialsMeanEnergy <= limit ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else
                {
                    //Delete "Mean" from descriptor name
                    string desc = queries[i].getDescriptor().substr(0, queries[i].getDescriptor().size() - 4);
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        feature<float> f;
                        get_i_descriptor(desc, txn, &key, f);
                        //Delete sounds that doesn't verify the condition
                        ( f.mean <= limit ) ? ++it : it = soundIDs.erase(it);
                    }
                }
            }
        }
        else if (queries[i].getType() == "OVER")
        {
            float limit = boost::any_cast<pair<float, float> >(queries[i].getValue()).first;
            //Firt query
            if (soundIDs.size() == 0)
            {
                float min, max;
                get_i_descriptorMinMax(txn, queries[i].getDescriptor(), min, max);
                
                if (queries[i].getDescriptor() == "duration" || queries[i].getDescriptor() == "MIDIcent")
                    get_i_symbolicsByRange(queries[i].getDescriptor(), txn, limit, max, soundIDs);
                else
                    get_i_descriptorByMeanRange(queries[i].getDescriptor(), txn, limit, max, soundIDs);
            }
            //Have already a list of soundIDs
            else
            {
                if (queries[i].getDescriptor() == "MIDIcent" || queries[i].getDescriptor() == "duration")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        symbolics s;
                        get_i_symbolics("symbolics", txn, &key, s);
                        //Delete sounds that doesn't verify the condition
                        ( boost::any_cast<float>(s.getMember(queries[i].getDescriptor())) >= limit ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else if (queries[i].getDescriptor() == "LoudnessFactor")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        melStruct m;
                        get_i_melBands("melbands", txn, &key, m);
                        //Delete sounds that doesn't verify the condition
                        ( m.loudnessFactor >= limit ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else if (queries[i].getDescriptor() == "PartialsMeanEnergy")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        partials p;
                        get_i_partials("partials", txn, &key, p);
                        //Delete sounds that doesn't verify the condition
                        ( p.partialsMeanEnergy >= limit ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else
                {
                    //Delete "Mean" from descriptor name
                    string desc = queries[i].getDescriptor().substr(0, queries[i].getDescriptor().size() - 4);
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        feature<float> f;
                        get_i_descriptor(desc, txn, &key, f);
                        //Delete sounds that doesn't verify the condition
                        ( f.mean >= limit ) ? ++it : it = soundIDs.erase(it);
                    }
                }
            }
        }
        else if (queries[i].getType() == "BETWEEN")
        {
            float min = boost::any_cast<pair<float, float> >(queries[i].getValue()).first;
            float max = boost::any_cast<pair<float, float> >(queries[i].getValue()).second;
            
            //First query
            if (soundIDs.size() == 0)
            {
                //Case: BETWEEN duration / BETWEEN MIDIcent
                if (queries[i].getDescriptor() == "duration" || queries[i].getDescriptor() == "MIDIcent")
                    get_i_symbolicsByRange(queries[i].getDescriptor(), txn, min, max, soundIDs);
                else
                    get_i_descriptorByMeanRange(queries[i].getDescriptor(), txn, min, max, soundIDs);
            }
            //Have already a list of soundIDs
            else
            {
                if (queries[i].getDescriptor() == "MIDIcent" || queries[i].getDescriptor() == "duration")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        symbolics s;
                        get_i_symbolics("symbolics", txn, &key, s);
                        //Delete sounds that doesn't verify the condition
                        ( boost::any_cast<float>(s.getMember(queries[i].getDescriptor())) >= min
                         && boost::any_cast<float>(s.getMember(queries[i].getDescriptor())) <= max ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else if (queries[i].getDescriptor() == "LoudnessFactor")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        melStruct m;
                        get_i_melBands("melbands", txn, &key, m);
                        //Delete sounds that doesn't verify the condition
                        ( m.loudnessFactor >= min && m.loudnessFactor <= max ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else if (queries[i].getDescriptor() == "PartialsMeanEnergy")
                {
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        partials p;
                        get_i_partials("melbands", txn, &key, p);
                        //Delete sounds that doesn't verify the condition
                        ( p.partialsMeanEnergy >= min && p.partialsMeanEnergy <= max ) ? ++it : it = soundIDs.erase(it);
                    }
                }
                else
                {
                    //Delete "Mean" from descriptor name
                    string desc = queries[i].getDescriptor().substr(0, queries[i].getDescriptor().size() - 4);
                    for (it = soundIDs.begin(); it != soundIDs.end();)
                    {
                        Dbt key;
                        init_dbt(&key, &*it, sizeof(int));
                        feature<float> f;
                        get_i_descriptor(desc, txn, &key, f);
                        //Delete sounds that doesn't verify the condition
                        ( f.mean >= min && f.mean <= max ) ? ++it : it = soundIDs.erase(it);
                    }
                }
            }
        }
        else if (queries[i].getType() == "FOLLOWS")
        {
            vector<float> vec = boost::any_cast<vector<float> >(queries[i].getValue());
            switch ((int)vec[0])
            {
                case 0:
                    // "Complete" case
                    vec.erase(vec.begin());
                    get_i_temporalQueryDTW(txn, vec, queries[i].getDescriptor(), soundIDs);
                    break;
                    
                case 1:
                    // "Fast" case
                    vec.erase(vec.begin());
                    indexTreePtr indexDesc = constructIndexesFromDB(sSession->getKnowledge(), queries[i].getDescriptor());
                    indexNodePtr resNode;
                    float duration = vec[0];
                    vec.erase(vec.begin());
                    vector<int> results;
                    indexDesc->approximateQueryConstrained(vec, soundIDs, 1, duration, results, resNode);
                    soundIDs = results;
                    break;
            }
        }
        else
        {
            printf("%s: Query type unknown...\n", queries[i].getType().c_str());
            return false;
        }
        if (soundIDs.size() == 0)
            return true;
    }
    
    return true;
}

bool BDBConnector::get_i_temporalQueryDTW ( DbTxn* txn, vector<float>& descValue, string descName, vector<int>& soundIDs )
{
    int i;
    vector<pair<int, float> > listID;
    Dbt vald;
    memset(&vald, 0, sizeof(Dbt));
    Dbc* dbc = getCursor(descName, txn);
    if (dbc == NULL)
        return false;
    
    //Retrieve first data = threshold
    int threshold = descValue[0];
    //and erase it
    descValue.erase(descValue.begin());
    
    //Normalization (not needed because we receive values between 0 and 1)
    /*
    float mean = getMean(&descValue[0], (int)descValue.size());
    vector<float> absDescValue(descValue.size());
    for (i = 0; i < descValue.size(); i++)
        absDescValue[i] = abs(descValue[i]);
    float max = getMax(&absDescValue[0], (int)absDescValue.size());
    for (i = 0; i < descValue.size(); i++)
    {
        descValue[i] -= mean;
        descValue[i] /= max;
    }
    */
    
    float reach = floor((0.0625 * 128) / 2);
    float *U    = (float*)malloc(descValue.size() * sizeof(float));
    float *L    = (float*)malloc(descValue.size() * sizeof(float));
    for (i = 0; i < descValue.size(); i++)
    {
        U[i] = _max(descValue[_max(i - reach, 1) - 1], descValue[_min(i + reach, descValue.size()) - 1]);
        L[i] = _min(descValue[_max(i - reach, 1) - 1], descValue[_min(i + reach, descValue.size()) - 1]);
    }
    
    // If IDs filter is empty then compute DTW on all IDs
    if (soundIDs.empty())
        soundIDs = linspace(1, get_current_index(txn), get_current_index(txn));
    
    for (i = 0; i < soundIDs.size(); i++)
    {
        Dbt key;
        init_dbt(&key, &soundIDs[i], sizeof(int));
        feature<float> val;
        int ret;
        if ( !(ret = dbc->get( &key, &vald, DB_SET )) )
        {
            restoreDescriptor(val, vald);
            if (val.mean != 0 && val.stdDev != 0)
            {
                float dMin = min_distDTWBSF(val.resampled_val->c_data(), val.resampled_val->size(), U, L);
                listID.push_back(pair<int, float>(soundIDs[i], dMin));
            }
        }
    }
    dbc->close();
    free(U);
    free(L);
        
    //Sort list by distances
    std::sort(listID.begin(), listID.end(), pairCompare<float>);
    //Store soundIDs
    soundIDs.clear();
    for (i = 0; i < threshold; i++)
        soundIDs.push_back(listID[i].first);    

    return true;
}

bool BDBConnector::get_i_secondaryIndexList ( DbTxn* txn, string table, vector<string>& list )
{
    Dbc *dbc = getCursor(table, txn);
    if (dbc == NULL)
        return false;

    Dbt key, data;

    dbc->get(&key, &data, DB_FIRST);
    do {
        list.push_back((char*)key.get_data());
        memset(&key, 0, sizeof(key));
        key.set_flags(DB_DBT_REALLOC);
    } while (!dbc->get(&key, &data, DB_NEXT_NODUP));
    
    dbc->close();
    return true;
}

bool BDBConnector::get_i_soundsDescriptorMean ( DbTxn* txn, string table, vector<int>& soundIDs, vector<float>& means )
{
    int i;
    Dbc *dbc = getCursor(table, txn);
    Dbt vald;
    for (i = 0; i < soundIDs.size(); i++)
    {
        Dbt key;
        init_dbt(&key, &soundIDs[i], sizeof(int));
        if (!dbc->get( &key, &vald, DB_SET ))
        {
            feature<float> f;
            restoreDescriptor(f, vald);
            means.push_back(f.mean);
        }
    }
    
    dbc->close();
    return true;
}

bool BDBConnector::get_i_allSoundsDescriptorMean ( DbTxn* txn, string table, vector<float>& means )
{
    Dbc *dbc = getCursor(table, txn);
    Dbt key, data;
    
    dbc->get(&key, &data, DB_FIRST);
    do {
        feature<float> f;
        restoreDescriptor(f, data);
        means.push_back(f.mean);
        memset(&key, 0, sizeof(key));
        key.set_flags(DB_DBT_REALLOC);
    } while (!dbc->get(&key, &data, DB_NEXT));
    
    dbc->close();
    return true;
}

vector<string> BDBConnector::get_i_names( DbTxn* txn, vector<int>& soundIDs )
{
    int i;
    vector<string> soundNames;
    Dbc *dbc = getCursor("symbolics", txn);
    Dbt vald;
    for (i = 0; i < soundIDs.size(); i++)
    {
        Dbt key;
        init_dbt(&key, &soundIDs[i], sizeof(int));
        if (!dbc->get( &key, &vald, DB_SET ))
            soundNames.push_back(((symbolics*)vald.get_data())->name);
    }
    
    dbc->close();
    return soundNames;
}

vector<string> BDBConnector::get_i_paths( DbTxn* txn, vector<int>& soundIDs, vector<bool>& isSOL )
{
    int i;
    vector<string> soundNames;
    Dbc *dbc = getCursor("symbolics", txn);
    Dbt vald;
    for (i = 0; i < soundIDs.size(); i++)
    {
        Dbt key;
        init_dbt(&key, &soundIDs[i], sizeof(int));
        if (!dbc->get( &key, &vald, DB_SET ))
        {
            string file = ((symbolics*)vald.get_data())->file;
            soundNames.push_back(file);
            string source = ((symbolics*)vald.get_data())->source;
            if (source == "SOL")
                isSOL.push_back(true);
            else
                isSOL.push_back(false);
        }
        else
        {
            soundNames.push_back("");
            isSOL.push_back(false);
        }
    }
    dbc->close();
    return soundNames;
}

void BDBConnector::createIndex( string index, string table, BDBExtractorFunc extractor, bool allowDuplicates, bool set_float_compare, bool set_int_compare )
{
    try {
        createIndex_i(index, table, NULL, extractor, allowDuplicates, set_float_compare, set_int_compare);
    } catch (DbException e) {
        throw e;
    }
}

void BDBConnector::createIndex_i( string index, string table,
                                 DbTxn* txn, BDBExtractorFunc extractor, bool allowDuplicates, bool set_float_compare, bool set_int_compare )
{
    try {
        Db* tdb = getTable_i( table );
        if ( !tdb )
        {
            printf("Table '%s' not found\n", table.c_str());
            return;
        }
        Db *idb = openTable_i( index, txn, allowDuplicates, set_float_compare, set_int_compare );
        mIndexes.insert( pair<string, Db*>(index, idb) );
        if ( tdb->associate( txn, idb, extractor, 0 )) closeTable_i( idb );
    } catch (DbException e) {
        throw e;
    }
}

DbTxn* BDBConnector::startTxn_i()
{
    DbTxn* txn;
    
    //Start transactions
    if ( mEnv.txn_begin( NULL, &txn, 0 ))
        return NULL;

    return txn;
}


//-------------------------------------------BDBTxn----------------------------------------------
BDBTxn::BDBTxn( BDBConnector& dbc ) :
mTxn( NULL ), mDbc( &dbc ), mDirty( false ) { }

BDBTxn::~BDBTxn()
{
    if ( mTxn )
        mTxn->abort();
}

void BDBTxn::initCoreDB()
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    try {
        mDbc->initCoreDB(mTxn);
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::close()
{    
    if ( mTxn )
        mTxn->abort();
    
    try {
        mDbc->close();
    } catch (DbException e) {
        throw e;
    }
}

bool BDBTxn::addSound( string filepath, string source, string instru )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    bool res = mDbc->add_i_sound(filepath, source, instru, mTxn);
    if (res) commit();
    return res;
}

bool BDBTxn::addSoundDirectory ( string path, string source, string instru )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    bool res =  mDbc->add_i_soundDirectory(path, source, instru, mTxn);
    if (res) commit();
    return res;
}

bool BDBTxn::addSoundDirectory_OSC_style( string path, string source, string instru, string instruShort, string style)
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    bool res =  mDbc->add_i_soundDirectory_OSC_style(path, source, instru, instruShort, style, mTxn);
    if (res) commit();
    return res;
}

bool BDBTxn::addSoundDirectory_OSC_recurse(string path, string source, string instru, string instruShort)
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    bool res =  mDbc->add_i_soundDirectory_OSC_recurse(path, source, instru, instruShort, mTxn);
    if (res) commit();
    return res;
}

int BDBTxn::get_current_index ( )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_current_index(mTxn);
}


int BDBTxn::key_count( string dbName )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    Db* db = mDbc->getTable_i(dbName);
    return bdb_key_count(db, mTxn);
}

void BDBTxn::putSymbolics( string table, Dbt *key, symbolics val, bool update )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    try {
        mDbc->put_i_symbolics( table, mTxn, key, val, update );
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::putMelBands( string table, Dbt *key, melStruct& val, bool update )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    try {
        mDbc->put_i_melBands( table, mTxn, key, val, update );
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::putPartials( string table, Dbt *key, partials& val, bool update )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    try {
        mDbc->put_i_partials( table, mTxn, key, val, update );
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::putDescriptor( string table, Dbt *key, feature<float>& val, bool update )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    try {
        mDbc->put_i_descriptor( table, mTxn, key, val, update );
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::putADSR( string table, Dbt *key, envelope val, bool update )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    try {
        mDbc->put_i_adsr( table, mTxn, key, val, update );
    } catch (DbException e) {
        throw e;
    }
}

int BDBTxn::getIndex ( string filename )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_index(filename, mTxn);
}

bool BDBTxn::getSymbolics( string table, Dbt *key, symbolics& val )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_symbolics( table, mTxn, key, val );
}

bool BDBTxn::getMelBands( string table, Dbt *key, melStruct& val )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_melBands( table, mTxn, key, val );
}

bool BDBTxn::getPartials( string table, Dbt *key, partials& val )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_partials( table, mTxn, key, val );
}

bool BDBTxn::getDescriptor( string table, Dbt *key, feature<float>& val )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_descriptor( table, mTxn, key, val );
}

bool BDBTxn::getDescriptorByMeanRange( string table, float start, float end, vector<int>& soundIDs)
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_descriptorByMeanRange(table, mTxn, start, end, soundIDs);
}

bool BDBTxn::getDescriptorMinMax ( string descriptor, float& min, float& max )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_descriptorMinMax(mTxn, descriptor, min, max);
}

bool BDBTxn::getMultipleFinalOptimized ( vector<int>& IDs, vector<string>& updateFeatures, vector<int>& typesIds, map<int, vector<matrixPtr> >& features, bool denormalized )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_multipleFinalOptimized(mTxn, IDs, updateFeatures, typesIds, features, denormalized);
}

bool BDBTxn::getSoundsQuery ( vector<Query> queries, vector<int>& soundIDs )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_soundsQuery(mTxn, queries, soundIDs);
}

bool BDBTxn::getTemporalQueryDTW ( vector<float>& descValue, string descName, vector<int>& soundIDs )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_temporalQueryDTW(mTxn, descValue, descName, soundIDs);
}

bool BDBTxn::getSecondaryIndexList ( string table, vector<string>& list )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_secondaryIndexList(mTxn, table, list);
}

bool BDBTxn::getSoundsDescriptorMean ( string table, vector<int> &soundIDs, vector<float> &means )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_soundsDescriptorMean(mTxn, table, soundIDs, means);
}

bool BDBTxn::getAllSoundsDescriptorMean ( string table, vector<float> &means )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_allSoundsDescriptorMean(mTxn, table, means);
}

vector<string> BDBTxn::getNames ( vector<int>& soundIDs )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_names(mTxn, soundIDs);
}

vector<string> BDBTxn::getPaths ( vector<int>& soundIDs, vector<bool>& isSOL )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    return mDbc->get_i_paths(mTxn, soundIDs, isSOL);
}

void BDBTxn::check_sounds_to_delete ( string source, vector<string> &filenames )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    try {
        mDbc->check_sounds_to_delete(mTxn, source, filenames);
        commit();
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::del( string table, Dbt *key )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    try {
        mDbc->del_i(table, mTxn, key);
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::removeSound ( int soundID )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    mDbc->remove_i_sound(soundID, mTxn);
    commit();
}

void BDBTxn::removeFamily ( char* family )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    mDbc->remove_i_family(family, mTxn);
    commit();
}

void BDBTxn::removeSoundDirectory ( char* path )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    try {
        mDbc->remove_i_soundDirectory(path, mTxn);
    } catch (DbException e) {
        throw e;
    }
    commit();
}

bool BDBTxn::refreshSoundDirectory ( string path, string source, vector<string> &filenames )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    mDirty = true;
    bool res = mDbc->refresh_i_soundDirectory(path, source, filenames, mTxn);
    if (res) commit();
    return res;
}

void BDBTxn::createTable( string table, bool allowDuplicates, bool set_float_compare, bool set_int_compare )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    try {
        mDbc->createTable_i(table, mTxn, allowDuplicates, set_float_compare, set_int_compare);
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::createIndex( string index, string table, BDBExtractorFunc extractor, bool allowDuplicates, bool set_float_compare, bool set_int_compare )
{
    if ( !mTxn ) mTxn = mDbc->startTxn_i();
    try {
        mDbc->createIndex_i(index, table, mTxn, extractor, allowDuplicates, set_float_compare, set_int_compare);
    } catch (DbException e) {
        throw e;
    }
}

void BDBTxn::commit()
{
    //assert( mTxn );
    if ( mTxn )
    {
        if ( mDirty )
        {
            cout << "Committing..." << endl;
            mTxn->commit(0);
        }
        else
            mTxn->abort();
        mTxn = NULL;
    }
}

void BDBTxn::abort()
{
    //assert( mTxn );
    if ( mTxn ) mTxn->abort();
    mTxn = NULL;
}