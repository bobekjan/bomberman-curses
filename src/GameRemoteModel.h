/** @file
 * @brief Declarations of a remote game model.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_REMOTE_MODEL_H__INCL__
#define __GAME_REMOTE_MODEL_H__INCL__

#include "GameModel.h"
#include "Socket.h"

/**
 * @brief A remote game model.
 *
 * @author Jan Bobek
 */
class GameRemoteModel
: public GameModel
{
public:
    /**
     * @brief Initializes the remote model.
     *
     * @param[in] addr Address in the form of IP-NUL-port-NUL.
     */
    GameRemoteModel( const char* addr );
    /**
     * @brief Releases all remote entities.
     */
    ~GameRemoteModel();

    /**
     * @brief Adds a new client if necessary.
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
     * @brief A remote controlled entity.
     *
     * @author Jan Bobek
     */
    class GameRemoteCtlEntity
    {
    public:
        /**
         * @brief Initializes the remote entity.
         *
         * @param[in] ctl The associated controller.
         */
        GameRemoteCtlEntity( GameController* ctl );
        /**
         * @brief Releases resources.
         */
        ~GameRemoteCtlEntity();

        /**
         * @brief Obtains the endgame flag.
         *
         * @retval true  The game has ended.
         * @retval false The game has not ended yet.
         */
        bool endgame() const { return mEndgame; }
        /**
         * @brief Opens a connection to a server.
         *
         * @param[in] addr Address in the form of IP-NUL-port-NUL.
         *
         * @retval true  Connection established.
         * @retval false Connection failed.
         */
        bool open( const char* addr );
        /**
         * @brief Sets the socket non-blocking.
         *
         * @retval true  The socket is now non-blocking.
         * @retval false Failed to switch to non-blocking mode.
         */
        bool setNonblock();

        /**
         * @brief Pop a game model event from the socket.
         *
         * @param[out] event The popped event.
         *
         * @retval true  An event was popped.
         * @retval false No event available.
         */
        bool pop( GameModelEvent& event );
        /**
         * @brief Ticks this entity.
         *
         * Gets a control event from the controller and
         * sticks it to the socket.
         */
        void tick();

    protected:
        /// Associated controller.
        GameController* mCtl;
        /// Our socket.
        Socket mSocket;
        /// An endgame flag.
        bool mEndgame;
    };

    /**
     * @brief Handles initalization of a new entity.
     *
     * @param[in] ctl Controller of the entity.
     */
    void dispatchEntityAdded( GameController* ctl );

    /// Our entities.
    std::list<GameRemoteCtlEntity*> mEntities;
    /// Address of the server.
    std::string mAddr;
};

#endif /* !__GAME_REMOTE_MODEL_H__INCL__ */
