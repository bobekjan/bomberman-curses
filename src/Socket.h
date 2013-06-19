/** @file
 * @brief A simple socket wrapper.
 *
 * @author Jan Bobek
 */

#ifndef __SOCKET_H__INCL__
#define __SOCKET_H__INCL__

/**
 * @brief Simple wrapper for sockets.
 *
 * @author Jan Bobek
 */
class Socket
{
public:
    /**
     * @brief Creates an invalid socket.
     */
    Socket();
    /**
     * @brief Creates a new socket.
     *
     * @param[in] af   An address family.
     * @param[in] type Type of socket to create.
     * @param[in] prot A protocol to use.
     */
    Socket( int af, int type, int prot );
    /**
     * @brief Properly releases the socket.
     */
    ~Socket();

    /**
     * @brief Creates a new socket.
     *
     * @param[in] af   An address family.
     * @param[in] type Type of socket to create.
     * @param[in] prot A protocol to use.
     *
     * @retval  0  Socket created successfully.
     * @retval !=0 Failed to create a socket.
     */
    int create( int af, int type, int prot );
    /**
     * @brief Shut down send/receive on the socket.
     *
     * @param[in] how Send/receive/send and receive?
     *
     * @return A value returned by <code>shutdown</code>.
     */
    int shutdown( int how );
    /**
     * @brief Closes the socket.
     *
     * @return A value returned by <code>close</code>.
     */
    int close();

    /**
     * @brief Connects to a given address.
     *
     * @param[in] name The address where to connect.
     * @param[in] len  Length of the name.
     *
     * @return A value returned by <code>connect</code>.
     */
    int connect( const sockaddr* name, unsigned int len );
    /**
     * @brief Binds a socket to a local address.
     *
     * @param[in] name The address to bind to.
     * @param[in] len  Length of the address to bind to.
     *
     * @return A value returned by <code>bind</code>.
     */
    int bind( const sockaddr* name, unsigned int len );

    /**
     * @brief Listen for incoming connections.
     *
     * @param[in] backlog Optional; how many connections to enqueue.
     *
     * @return A value returned by <code>listen</code>.
     */
    int listen( int backlog = SOMAXCONN );
    /**
     * @brief Accepts an incoming connection.
     *
     * @param[out] sock Where to store the accepted socket.
     * @param[out] addr Address of the connectee.
     * @param[out] len  Length of the address of the connectee.
     *
     * @retval  0  Connection accepted successfully.
     * @retval !=0 Failed to accept the connection.
     */
    int accept( Socket& sock, sockaddr* addr, unsigned int* len );

    /**
     * @brief Retrieves data from the socket.
     *
     * @param[out] buf   Where to store the data.
     * @param[in]  len   Length of the buffer.
     * @param[in]  flags Optional flags.
     *
     * @return A value returned by <code>recv</code>.
     */
    int recv( void* buf, unsigned int len, int flags );
    /**
     * @brief Sends data to the socket.
     *
     * @param[in] buf   Buffer with the data to send.
     * @param[in] len   Length of the data.
     * @param[in] flags Optional flags.
     *
     * @return A value returned by <code>send</code>.
     */
    int send( const void* buf, unsigned int len, int flags );

    /**
     * @brief Receives data from a peer.
     *
     * @param[out] buf   Where to store the data.
     * @param[in]  len   Length of the buffer.
     * @param[in]  flags Optional flags.
     * @param[out] from  Where to store address of the peer.
     * @param[out] flen  Length of the address of the sending peer.
     *
     * @return A value returned by <code>recvfrom</code>.
     */
    int recvfrom( void* buf, unsigned int len, int flags,
                  sockaddr* from, unsigned int* flen );
    /**
     * @brief Sends data to a peer.
     *
     * @param[in] buf   Buffer with the data to send.
     * @param[in] len   Length of the data.
     * @param[in] flags Optional flags.
     * @param[in] to    Address of the receiver.
     * @param[in] tolen Length of the address of the receiver.
     *
     * @return A value returned by <code>sendto</code>.
     */
    int sendto( const void* buf, unsigned int len, int flags,
                const sockaddr* to, unsigned int tolen );

    /**
     * @brief Sets an option of the socket.
     *
     * @param[in] level   Level of the option.
     * @param[in] optname Name of the option.
     * @param[in] optval  Address of the value of the option.
     * @param[in] optlen  Length of the value.
     *
     * @return A value returned by <code>setsockopt</code>.
     */
    int setopt( int level, int optname, const void* optval,
                unsigned int optlen );
    /**
     * @brief Sets an advanced option of the socket.
     *
     * @param[in] cmd A command to perform.
     * @param[in] arg An argument to the command.
     *
     * @return A value returned by <code>fcntl</code>.
     */
    int fcntl( int cmd, long arg );

protected:
    /// The socket
    int mSock;
};

#endif /* !__SOCKET_H__INCL__ */
