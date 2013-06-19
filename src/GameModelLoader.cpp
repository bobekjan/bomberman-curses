/** @file
 * @brief Implementation of the GameModeLoader template.
 *
 * @author Jan Bobek
 */

#include "GameModelLoader.h"
#include "GameLocalModel.h"
#include "GameServerModel.h"
#include "GameRemoteModel.h"
#include "util.h"

/*************************************************************************/
/* GameModelLoader                                                       */
/*************************************************************************/
template<>
GameLocalModel*
GameModelLoader::load<GameLocalModel>()
{
    /* Choose a map. */
    std::string map;
    chooseFile( map, "Vyberte mapu:" );

    /* Load the map. */
    GameLocalModel* gm = loadMap<GameLocalModel>( map );
    if( !gm )
        msgbox( "Chyba", "Nepodarilo se nacist zvolenou mapu, zvolte prosim jinou." );

    GameModelEvent event;
    event.entity = GENT_MONSTER;
    event.coords = GameCoordRect( gm->size(), gm->size() );
    event.ctl = NULL;
    gm->dispatch( event );

    return gm;
}

template<>
GameServerModel*
GameModelLoader::load<GameServerModel>()
{
    /* Choose a map. */
    std::string map;
    chooseFile( map, "Vyberte mapu:" );

    /* Load the map. */
    GameServerModel* gm = loadMap<GameServerModel>( map );
    if( !gm )
    {
        msgbox( "Chyba", "Nepodarilo se nacist zvolenou mapu, zvolte prosim jinou." );
        return NULL;
    }

    /* Choose an address. */
    std::string addr = "0.0.0.0:34567";
    if( !chooseAddress( addr, "Prosim zadejte adresu pro "
                        "naslouchani ve formatu adresa:port." ) )
    {
        safeDelete( gm );
        return NULL;
    }

    /* Open the address. */
    if( !gm->open( addr.c_str () ) )
    {
        msgbox( "Chyba", "Nepodarilo se otevrit hru pro ostatni hrace. Zkuste to prosim znovu." );
        safeDelete( gm );
        return NULL;
    }

    GameModelEvent event;
    event.entity = GENT_MONSTER;
    event.coords = GameCoordRect( gm->size(), gm->size() );
    event.ctl = NULL;
    gm->dispatch( event );

    /* Success. */
    return gm;
}

template<>
GameRemoteModel*
GameModelLoader::load<GameRemoteModel>()
{
    /* Choose an address. */
    std::string addr = "127.0.0.1:34567";
    if( !chooseAddress( addr, "Prosim zadejte adresu pro "
                        "pripojeni ve formatu adresa:port." ) )
        return NULL;

    return new GameRemoteModel( addr.c_str() );
}

template<typename T>
T*
GameModelLoader::loadMap(
    const std::string& name
    )
{
    /* Open the file. */
    std::ifstream file( name.c_str(), std::ios_base::in );
    if( !file )
        return NULL;

    /* Load size of the map. */
    GameCoord size;
    if( !(file >> size.row >> size.col) )
        return NULL;

    /* Skip the rest of the line. */
    if( !file.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n' ))
        return NULL;

    /* Instantiate the game model. */
    T* gm = new T( size );
    GameModelEvent event;
    event.ctl = NULL;

    for( GameCoord cur; cur.row < size.row; ++cur.row )
    {
        for( cur.col = 0; cur.col < size.col; ++cur.col )
        {
            /* Fill the event. */
            event.entity = translate( file.get() );
            event.coords = GameCoordRect( cur, cur );

            if( GENT_COUNT == event.entity )
            {
                /* Invalid format. */
                safeDelete( gm );
                return NULL;
            }

            /* Dispatch the event. */
            gm->dispatch( event );
        }

        if( '\n' != file.get() )
        {
            /* Invalid format. */
            safeDelete( gm );
            return NULL;
        }
    }

    return gm;
}

void
GameModelLoader::chooseFile(
    std::string& name,
    const char* tit,
    const char* dir
    )
{
    name.clear();

    /* Open the directory. */
    DIR* d = opendir( dir );
    if( !d ) return;

    /* Iterate over the entries. */
    std::vector<ITEM*> items;
    dirent* ent = NULL;
    while( (ent = readdir( d )) )
    {
        if( !strcmp( ent->d_name, "." ) ||
            !strcmp( ent->d_name, ".." ) )
            /* Not interested. */
            continue;

        /* Duplicate the name. */
        char* name = strdup( ent->d_name );
        /* Create a menu item. */
        ITEM* item = new_item( name, "" );
        /* Store the filename to it. */
        set_item_userptr( item, name );

        items.push_back( item );
    }
    /* NULL-teminate! */
    items.push_back( NULL );

    /* Close the directory. */
    closedir( d );
    /* Extract the name. */
    unsigned int idx = menu_select(
        tit, &items[0] );
    name += dir;
    name += '/';
    name += (char*)item_userptr( items[idx] );

    /* Release the items. */
    std::vector<ITEM*>::iterator cur, end;
    cur = items.begin();
    end = items.end();
    for(; cur != end; ++cur )
    {
        /* Free the memory from strdup. */
        free( item_userptr( *cur ) );
        /* Free the item. */
        free_item( *cur );
    }
}

bool
GameModelLoader::chooseAddress(
    std::string& addr,
    const char* msg
    )
{
    /* Get the address. */
    askbox( "Zadost", msg, "0123456789.:", addr );

    /* Find a colon. */
    const size_t pos = addr.find( ':' );
    if( std::string::npos == pos )
    {
        msgbox( "Chyba", "Neplatne zadani, chybi dvojtecka. Zkuste to prosim znovu." );
        return false;
    }

    /* Replace by '\0' */
    addr[pos] = '\0';
    return true;
}

GameEntity
GameModelLoader::translate(
    char c
    )
{
    if( ' ' == c )
        return GENT_NONE;
    else if( '#' == c )
        return GENT_BARRIER;
    else if( '@' == c )
        return GENT_SPAWN;
    else if( 'O' == c )
        return GENT_TRAP;
    else if( 'X' == c )
        return GENT_WALL;
    else
        /* Invalid value. */
        return GENT_COUNT;
}
