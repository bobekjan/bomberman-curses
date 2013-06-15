/** @file
 * @brief A game model-related declarations.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_MODEL__H__INCL__
#define __GAME_MODEL__H__INCL__

#include "Game.h"

/**
 * @brief An abstract entity controller.
 *
 * At each tick the controller says what the associated
 * entity intends to do.
 *
 * @author Jan Bobek
 */
class GameController
{
public:
    /**
     * @brief Obtain an action for the current tick.
     *
     * @param[out] event Where to store the action.
     */
    virtual void tick( GameCtlEvent& event ) = 0;
};

/**
 * @brief An abstact game canvas.
 *
 * The canvas is used to draw the game map.
 *
 * @author Jan Bobek
 */
class GameCanvas
{
public:
    /**
     * @brief Draws an entity at given coords.
     *
     * @param[in] entity The entity to draw.
     * @param[in] coord  The coords at which to draw.
     */
    virtual void draw( GameEntity entity, const GameCoord& coord ) = 0;
};

/**
 * @brief An event related to the game model.
 *
 * These events are sent over the wire to keep
 * the remote game models synchronized.
 *
 * @author Jan Bobek
 */
struct GameModelEvent
{
    enum
    {
        GME_ADDENT, ///< Entities added
        GME_REMENT  ///< Entities removed
    } event;        ///< The event itself.

    /// A related game entity.
    GameEntity entity;
    /// A related area.
    GameCoordRect coords;

    /// A related entity controller; may be NULL.
    GameController* ctl;
};

/**
 * @brief A basic game model.
 *
 * @author Jan Bobek
 */
class GameModel
{
public:
    /**
     * @brief Initializes empty game map.
     *
     * @param[in] size Size of the map.
     */
    GameModel( const GameCoord& size );
    /**
     * @brief Releases the game map and other resources.
     */
    virtual ~GameModel();

    /**
     * @brief Dispatches a game model event.
     *
     * @param[in] event The event to dispatch.
     */
    virtual void dispatch(
        const GameModelEvent& event );

    /**
     * @brief Performs a single tick.
     */
    virtual void tick() = 0;

    /**
     * @brief Draws changes of the last tick.
     *
     * @param[in] canvas The canvas to use.
     */
    virtual void draw( GameCanvas& canvas );
    /**
     * @brief Redraws the entire map.
     *
     * @param[in] canvas The canvas to use.
     */
    virtual void redraw( GameCanvas& canvas );

protected:
    /// Size of the game map.
    GameCoord mSize;
    /// The game map.
    GameEntity* mMap;

    /// A vector of spawn points.
    std::vector<GameCoord> mSpawns;
    /// A vector of dirty map regions to draw.
    std::vector<GameCoordRect> mDirty;
};

/**
 * @brief A local (as opposed to remote) game model.
 *
 * @author Jan Bobek
 */
class GameLocalModel
: public GameModel
{
public:
    /**
     * @brief Initializes empty game map.
     *
     * @param[in] size Size of the map.
     */
    GameLocalModel( const GameCoord& size );
    /**
     * @brief Releases the controlled entities.
     */
    ~GameLocalModel();

    /**
     * @brief Dispatches a game model event.
     *
     * @param[in] event The event to dispatch.
     */
    void dispatch(
        const GameModelEvent& event );
    /**
     * @brief Enqueues an event to be dispatched on next tick.
     *
     * @param[in] event The event.
     */
    void queue(
        const GameModelEvent& event );

    /**
     * @brief Performs a single tick.
     */
    void tick();

    /**
     * @brief Draws changes of the last tick.
     *
     * @param[in] canvas The canvas to use.
     */
    void draw( GameCanvas& canvas );
    /**
     * @brief Redraws the entire map.
     *
     * @param[in] canvas The canvas to use.
     */
    void redraw( GameCanvas& canvas );

protected:
    /**
     * @brief A controlled game entity.
     *
     * @author Jan Bobek
     */
    struct GameCtlEntity
    {
        /**
         * @brief Initializes a new entity.
         *
         * @param[in] ent_ The entity.
         * @param[in] pos_ Position of the entity.
         * @param[in] ctl_ The associated controller.
         */
        GameCtlEntity( GameEntity ent_, const GameCoord& pos_,
                       GameController* ctl_ );
        /**
         * @brief Deletes the controller.
         */
        ~GameCtlEntity();

        /// The entity.
        GameEntity ent;
        /// Position of the entity.
        GameCoord pos;
        /// The associated controller.
        GameController* ctl;

        /// Number of available bombs.
        unsigned char bombs;
        /// Length of the flames.
        unsigned char flames;
        /// Number of ticks between moves.
        unsigned char speed;
        /// Remote bomb control enabled?
        bool rc;

        /// Number of ticks until the next move.
        unsigned char nextmove;
    };
    /**
     * @brief A bomb game entity.
     *
     * @author Jan Bobek
     */
    struct GameBombEntity
    {
        /**
         * @brief Initializes the bomb entity.
         *
         * @param[in] pos_ Position of the bomb.
         * @param[in] ctl_ The associated controlled entity.
         */
        GameBombEntity( const GameCoord& pos_, GameCtlEntity& ctl_ );

        /// Position of the bomb.
        GameCoord pos;
        /// The associated controlled entity.
        GameCtlEntity& ctl;

        /// Number of ticks until explosion.
        unsigned char timer;
    };

    /// A list of controlled entities.
    std::list<GameCtlEntity> mCtlEntities;
    /// A list of bombs.
    std::list<GameBombEntity> mBombs;

    /// A queue of events to dispatch at next tick.
    std::queue<GameModelEvent> mEventPipe;
};

#endif /* !__GAME_MODEL__H__INCL__ */
