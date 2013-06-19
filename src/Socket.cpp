/** @file
 * @brief Implementation of a socket wrapper.
 *
 * @author Jan Bobek
 */

#include "Game.h"
#include "Socket.h"

/*************************************************************************/
/* Socket                                                                */
/*************************************************************************/
Socket::Socket()
: mSock( -1 )
{
}

Socket::Socket(
    int af,
    int type,
    int prot
    )
: mSock( -1 )
{
    int code = create( af, type, prot );
    assert( 0 == code );
}

Socket::~Socket()
{
    int code = close();
    assert( 0 == code );
}

int
Socket::create(
    int af,
    int type,
    int prot
    )
{
    /* Close any previous socket. */
    if( close() )
        return 1;

    mSock = ::socket( af, type, prot );
    return !(0 < mSock);
}

int
Socket::shutdown(
    int how
    )
{
    return ::shutdown( mSock, how );
}

int
Socket::close()
{
    if( !(0 < mSock) )
        /* Invalid socket. */
        return 0;

    /* Disable writes. */
    shutdown( SHUT_WR );
    /* Disable reads. */
    shutdown( SHUT_RD );
    /* Close the socket. */
    if( ::close( mSock ) )
        return 1;

    /* Invalidate the socket. */
    mSock = -1;
    return 0;
}

int
Socket::connect(
    const sockaddr* name,
    unsigned int len
    )
{
    return ::connect( mSock, name, len );
}

int
Socket::bind(
    const sockaddr* name,
    unsigned int len
    )
{
    return ::bind( mSock, name, len );
}

int
Socket::listen(
    int backlog
    )
{
    return ::listen( mSock, backlog );
}

int
Socket::accept(
    Socket& sock,
    sockaddr* addr,
    unsigned int* len
    )
{
    /* Close the other socket first. */
    int code = sock.close();
    if( code )
        return code;

    /* Accept the connection. */
    sock.mSock = ::accept( mSock, addr, len );
    return !(0 < sock.mSock);
}

int
Socket::recv(
    void* buf,
    unsigned int len,
    int flags
    )
{
    return ::recv( mSock, buf, len, flags );
}

int
Socket::send(
    const void* buf,
    unsigned int len,
    int flags
    )
{
    return ::send( mSock, buf, len, flags );
}

int
Socket::recvfrom(
    void* buf,
    unsigned int len,
    int flags,
    sockaddr* from,
    unsigned int* flen
    )
{
    return ::recvfrom(
        mSock, buf, len,
        flags, from, flen );
}

int
Socket::sendto(
    const void* buf,
    unsigned int len,
    int flags,
    const sockaddr* to,
    unsigned int tolen
    )
{
    return ::sendto(
        mSock, buf, len,
        flags, to, tolen );
}

int
Socket::setopt(
    int level,
    int optname,
    const void* optval,
    unsigned int optlen
    )
{
    return ::setsockopt(
        mSock, level, optname,
        optval, optlen );
}

int
Socket::fcntl(
    int cmd,
    long arg
    )
{
    return ::fcntl( mSock, cmd, arg );
}
