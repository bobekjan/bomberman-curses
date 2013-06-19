/** @file
 * @brief Implementation of a remote game model.
 *
 * @author Jan Bobek
 */

#include "GameController.h"
#include "GameRemoteModel.h"
#include "util.h"

/*************************************************************************/
/* GameRemoteModel                                                       */
/*************************************************************************/
GameRemoteModel::GameRemoteModel(
    const char* addr
    )
: GameModel( GameCoord( 0, 0 ) )
{
    unsigned int len = strlen( addr ) + 1;
    len += strlen( addr + len );

    mAddr.assign( addr, addr + len );
}

GameRemoteModel::~GameRemoteModel()
{
    std::list<GameRemoteCtlEntity*>::iterator cur, end;
    cur = mEntities.begin();
    end = mEntities.end();
    for(; cur != end; ++cur )
        safeDelete( *cur );
}

void
GameRemoteModel::dispatch(
    const GameModelEvent& event
    )
{
    if( event.ctl )
        dispatchEntityAdded( event.ctl );
    else
        GameModel::dispatch( event );
}

bool
GameRemoteModel::tick()
{
    std::list<GameRemoteCtlEntity*>::iterator cur, end;
    cur = mEntities.begin();
    end = mEntities.end();
    while( cur != end )
    {
        /* Tick the entity. */
        (*cur)->tick();

        /* Pull all events. */
        GameModelEvent event;
        while( (*cur)->pop( event ) )
            dispatch( event );

        /* Endgame? */
        if( !(*cur)->endgame() )
            ++cur;
        else
        {
            safeDelete( *cur );
            cur = mEntities.erase( cur );
        }
    }

    return !mEntities.empty();
}

void
GameRemoteModel::dispatchEntityAdded(
    GameController* ctl
    )
{
    /* Create a new entity. */
    GameRemoteCtlEntity* ent = new GameRemoteCtlEntity( ctl );
    /* Try to connect to the server. */
    if( !ent->open( mAddr.c_str() ) )
    {
        /* Failed ... */
        safeDelete( ent );
        return;
    }

    /* It is now still in blocking mode. Pop the first update. */
    GameModelEvent event;
    if( !ent->pop( event ) )
    {
        /* Failed ... */
        safeDelete( ent );
        return;
    }

    assert( GENT_NONE == event.entity );
    if( mSize.row || mSize.col )
        assert( mSize == event.coords.second );
    else
    {
        /* We need to create a new map. */
        safeDeleteArray( mMap );

        mSize = event.coords.second;
        mMap = safeAllocArray<GameEntity>( mSize.row * mSize.col );
    }

    /* Now set nonblock. */
    if( !ent->setNonblock() )
    {
        /* Failed ... */
        safeDelete( ent );
        return;
    }

    /* Welcome home. */
    mEntities.push_back( ent );
}

/*************************************************************************/
/* GameRemoteModel::GameRemoteCtlEntity                                  */
/*************************************************************************/
GameRemoteModel::GameRemoteCtlEntity::GameRemoteCtlEntity(
    GameController* ctl
    )
: mCtl( ctl ),
  mEndgame( false )
{
}

GameRemoteModel::GameRemoteCtlEntity::~GameRemoteCtlEntity()
{
    safeDelete( mCtl );
}

bool
GameRemoteModel::GameRemoteCtlEntity::open(
    const char* addr
    )
{
    /* Some variables. */
    addrinfo* ai = NULL, hints;
    unsigned int reuse_addr = 1;
    unsigned int bufsize = 64 * 1024;

    /* Extract the parts. */
    const char* name = addr;
    const char* serv = addr + strlen( addr ) + 1;

    /* Setup hints. */
    memset( &hints, 0, sizeof( hints ) );
    /* Setup some flags. */
    hints.ai_flags = AI_NUMERICHOST
        | AI_NUMERICSERV | AI_ADDRCONFIG;
    /* We want stream socket. */
    hints.ai_socktype = SOCK_STREAM;

    if(
        /* Translate the name. */
        getaddrinfo( name, serv, &hints, &ai ) || !ai ||
        /* Use the first address. */
        mSocket.create(
            ai->ai_family, ai->ai_socktype, ai->ai_protocol ) ||
        /* Allow reuse addr. */
        mSocket.setopt( SOL_SOCKET, SO_REUSEADDR,
                        &reuse_addr, sizeof( reuse_addr ) ) ||
        /* Connect to the address. */
        mSocket.connect( ai->ai_addr, ai->ai_addrlen ) ||
        /* Setup recv bufsize. */
        mSocket.setopt( SOL_SOCKET, SO_RCVBUF,
                        &bufsize, sizeof( bufsize ) )
        )
    {
        /* Do not forget to release the addrinfo. */
        safeRelease( ai, freeaddrinfo );
        return false;
    }

    /* Do not forget to release the addrinfo. */
    safeRelease( ai, freeaddrinfo );
    return true;
}

bool
GameRemoteModel::GameRemoteCtlEntity::setNonblock()
{
    return !mSocket.fcntl( F_SETFL, O_NONBLOCK );
}

bool
GameRemoteModel::GameRemoteCtlEntity::pop(
    GameModelEvent& event
    )
{
    int code = mSocket.recv( &event, sizeof( event ), 0 );
    if( code <= 0 )
    {
        mEndgame = !code;
        return false;
    }
    else if( sizeof( event ) != code )
        /* Incomplete event received */
        abort();
    else
        return true;
}

void
GameRemoteModel::GameRemoteCtlEntity::tick()
{
    GameCtlEvent event;
    mCtl->tick( event );

    if( GCE_NOOP != event )
        mSocket.send( &event, sizeof( event ),
                      MSG_NOSIGNAL );
}
