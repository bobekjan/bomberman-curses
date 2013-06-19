/** @file
 * @brief Implementation of a server game model.
 *
 * @author Jan Bobek
 */

#include "GameServerModel.h"
#include "util.h"

/*************************************************************************/
/* GameServerModel                                                       */
/*************************************************************************/
GameServerModel::GameServerModel(
    const GameCoord& size
    )
: GameLocalModel( size ),
  mClientSocket( NULL )
{
}

GameServerModel::~GameServerModel()
{
    /* Close everything. */
    close();
}

bool
GameServerModel::open(
    const char* addr
    )
{
    /* Some variables. */
    addrinfo* ai = NULL, hints;
    unsigned int reuse_addr = 1;

    /* Extract the parts. */
    const char* name = addr;
    const char* serv = addr + strlen( addr ) + 1;

    /* Setup hints. */
    memset( &hints, 0, sizeof( hints ) );
    /* Setup some flags. */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST
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
        /* Bind to the address. */
        mSocket.bind( ai->ai_addr, ai->ai_addrlen ) ||
        /* Setup non-block. */
        mSocket.fcntl( F_SETFL, O_NONBLOCK ) ||
        /* Start listening. */
        mSocket.listen()
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

void
GameServerModel::close()
{
    /* Kill all (humans) clients. */
    std::list<GameClient*>::iterator cur, end;
    cur = mClients.begin();
    end = mClients.end();
    for(; cur != end; ++cur )
        safeDelete( *cur );

    /* Wipe the list. */
    mClients.clear();
    /* Close the socket. */
    mSocket.close();
    /* Delete the client socket. */
    safeDelete( mClientSocket );
}

void
GameServerModel::dispatch(
    const GameModelEvent& event
    )
{
    /* Parents first. */
    GameLocalModel::dispatch( event );
    /* If it has invalid position, not for us. */
    if( event.coords.second == mSize ) return;

    /* Broadcast to all clients. */
    std::list<GameClient*>::iterator cur, end;
    cur = mClients.begin();
    end = mClients.end();
    while( cur != end )
        if( (*cur)->push( event ) )
            ++cur;
        else
        {
            /* So long, dont come back. */
            safeDelete( *cur );
            cur = mClients.erase( cur );
        }
}

bool
GameServerModel::tick()
{
    while( true )
    {
        /* Create a new socket if we do not have one. */
        if( !mClientSocket )
            mClientSocket = new Socket;

        /* Try to accept a connection. */
        if( mSocket.accept( *mClientSocket, NULL, NULL ) )
            /* No new connections ... */
            break;

        /* Handle it. */
        tickClientConnected( mClientSocket );
    }

    return GameLocalModel::tick();
}

bool
GameServerModel::checkEndCond()
{
    std::list<GameCtlEntity>::iterator cur, end;
    cur = mCtlEntities.begin();
    end = mCtlEntities.end();
    for(; cur != end; ++cur )
        if( GENT_PLAYER == cur->ent )
            return true;

    return false;
}

void
GameServerModel::tickClientConnected(
    Socket*& sock
    )
{
    unsigned int bufsize = 64 * 1024;
    if(
        /* Increase recv buffer size. */
        sock->setopt( SOL_SOCKET, SO_RCVBUF,
                               &bufsize, sizeof( bufsize ) ) ||
        /* Enable non-block */
        sock->fcntl( F_SETFL, O_NONBLOCK ) )
    {
        /* Strange, should not happen. */
        sock->close();
        return;
    }

    /* Create a GameClient. */
    GameClient* client = new GameClient( sock );

    /* Add him to the game. */
    GameModelEvent event;
    event.entity = GENT_PLAYER;
    event.coords = GameCoordRect( mSize, mSize );
    event.ctl = client->ctl();
    dispatch( event );

    /* First send him dimensions of the game map. */
    GameCoordRect rect(
        GameCoord( 0, 0 ),
        GameCoord( mSize ) );

    event.entity = GENT_NONE;
    event.coords = rect;
    event.ctl = NULL;

    /* Push it to him. */
    if( !client->push( event ) )
    {
        /* So soon and you're going? */
        safeDelete( client );
        return;
    }

    /* Now push the entire game map to him. */
    --rect.second.row;
    --rect.second.col;

    GAME_COORD_RECT_ITERATE( cur, rect )
    {
        /* Fill the event. */
        event.entity = at( cur );
        event.coords = GameCoordRect( cur, cur );
        event.ctl = NULL;

        /* Push it to him. */
        if( !client->push( event ) )
        {
            /* So soon and you're going? */
            safeDelete( client );
            return;
        }
    }

    /* Welcome to the family. */
    mClients.push_back( client );
}

/*************************************************************************/
/* GameServerModel::GameClient                                           */
/*************************************************************************/
GameServerModel::GameClient::GameClient(
    Socket*& sock
    )
: mSocket( sock ),
  mCtl( NULL )
{
    /* Consume the socket. */
    sock = NULL;
}

GameServerModel::GameClient::~GameClient()
{
    /* If we have a controller, decouple. */
    if( mCtl )
        mCtl->setClient( NULL );

    /* Delete the socket. */
    safeDelete( mSocket );
}

GameController*
GameServerModel::GameClient::ctl()
{
    if( !mCtl )
        mCtl = new Controller( this );
    return mCtl;
}

bool
GameServerModel::GameClient::push(
    const GameModelEvent& event
    )
{
    /* Stick it to the buffer. */
    mBuffer.insert(
        mBuffer.end(),
        (const unsigned char*)&event,
        (const unsigned char*)&event + sizeof( event ) );

    /* Push the buffer out. */
    return push();
}

void
GameServerModel::GameClient::die()
{
    /* The controller died. */
    mCtl = NULL;
    /* No more receives. */
    mSocket->shutdown( SHUT_RD );
}

bool
GameServerModel::GameClient::push()
{
    int code = mSocket->send(
        &mBuffer[0], mBuffer.size(), MSG_NOSIGNAL );

    if( 0 <= code )
    {
        /* Sent "code" bytes. */
        mBuffer.erase(
            mBuffer.begin(),
            mBuffer.begin() + code );

        return true;
    }
    else if( EAGAIN == errno || EWOULDBLOCK == errno )
        return true;
    else
        /* Something's fucked up. */
        return false;
}

/*************************************************************************/
/* GameServerModel::GameClient::Controller                               */
/*************************************************************************/
GameServerModel::GameClient::Controller::Controller(
    GameServerModel::GameClient* client
    )
: mClient( client )
{
}

GameServerModel::GameClient::Controller::~Controller()
{
    /* Tell him he's dead. */
    if( mClient )
        mClient->die();
}

void
GameServerModel::GameClient::Controller::tick(
    GameCtlEvent& event
    )
{
    int code = 0;
    if( mClient )
        code = mClient->mSocket->recv( &event, sizeof( event ), 0 );

    if( code <= 0 )
        /* Nothing to do. */
        event = GCE_NOOP;
    else if( sizeof( event ) != code )
        /* Received incomplete event ... */
        abort();
}

void
GameServerModel::GameClient::Controller::setClient(
    GameServerModel::GameClient* client
    )
{
    mClient = client;
}
