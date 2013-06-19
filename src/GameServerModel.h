/** @file
 * @brief Server-side of the game model.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_SERVER_MODEL_H__INCL__
#define __GAME_SERVER_MODEL_H__INCL__

#include "GameController.h"
#include "GameLocalModel.h"
#include "Socket.h"

/**
 * @brief Server game model.
 *
 * @author Jan Bobek
 */
class GameServerModel
: public GameLocalModel
{
public:
    /**
     * @brief Initialize the server.
     *
     * @param[in] size Size of the map.
     */
    GameServerModel( const GameCoord& size );
    /**
     * @brief Closes sockets and other resources.
     */
    ~GameServerModel();

    /**
     * @brief Open the model for remote players.
     *
     * @param[in] addr Address in the form of IP-NUL-port-NUL.
     *
     * @retval true  Listening started.
     * @retval false Failed to start listening.
     */
    bool open( const char* addr );
    /**
     * @brief Close all remote connections.
     */
    void close();

    /**
     * @brief Broadcasts the updates to all connected clients.
     *
     * @param[in] event The event to broadcast.
     */
    void dispatch( const GameModelEvent& event );

    /**
     * @brief Handles the sockets.
     *
     * @retval true  The game continues.
     * @retval false The game has ended.
     */
    bool tick();

protected:
    /**
     * @brief A connected game client.
     *
     * @author Jan Bobek
     */
    class GameClient
    {
    public:
        /**
         * @brief Initializes the client.
         *
         * @param[in] sock Socket of the client.
         */
        GameClient( Socket*& sock );
        /**
         * @brief Releases the socket and other resources.
         */
        ~GameClient();

        /**
         * @brief Obtain an associated controller.
         *
         * @return The associated controller.
         */
        GameController* ctl();
        /**
         * @brief Pushes a game model evente to the client.
         *
         * @param[in] event The event to send.
         *
         * @retval true  Push ok.
         * @retval false Push failed.
         */
        bool push( const GameModelEvent& event );

    protected:
        class Controller;

        /**
         * @brief Notify the client about death.
         */
        void die();
        /**
         * @brief Pushes the buffer to the client.
         *
         * @retval true  Push ok.
         * @retval false Push failed.
         */
        bool push();

        /// The send buffer.
        std::vector<unsigned char> mBuffer;
        /// Socket of the client.
        Socket* mSocket;
        /// Our associated controller.
        Controller* mCtl;
    };
    /**
     * @brief A controller associated with a client.
     *
     * @author Jan Bobek
     */
    class GameClient::Controller
    : public GameController
    {
    public:
        /**
         * @brief Initializes the controller.
         *
         * @param[in] client The associated game client.
         */
        Controller( GameClient* client );
        /**
         * @brief Notifies the client about its death.
         */
        ~Controller();

        /**
         * @brief Receives a command from the client.
         *
         * @param[out] event The control event.
         */
        void tick( GameCtlEvent& event );
        /**
         * @brief Couples with a client.
         *
         * @param[in] client The new client.
         */
        void setClient( GameClient* client );

    protected:
        /// Our associated game client.
        GameClient* mClient;
    };

    /**
     * @brief Checks if the end conditions have been met.
     *
     * @retval true  The game continues.
     * @retval false The game has ended.
     */
    bool checkEndCond();

    /**
     * @brief Handles a new client.
     *
     * @param[in] sock The socket of the client.
     */
    void tickClientConnected( Socket*& sock );

    /// Our listen socket.
    Socket mSocket;
    /// Candidate client socket.
    Socket* mClientSocket;
    /// Our connected clients.
    std::list<GameClient*> mClients;
};

#endif /* !__GAME_SERVER_MODEL_H__INCL__ */
