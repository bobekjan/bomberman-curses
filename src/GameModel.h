/** @file
 * @brief A game model-related declarations.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_MODEL__H__INCL__
#define __GAME_MODEL__H__INCL__

#include "Game.h"
#include "GameCanvas.h"
#include "GameController.h"

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
     * @brief Obtain size of the map.
     *
     * @return Size of the map.
     */
    const GameCoord& size() const { return mSize; }
    /**
     * @brief Obtain number of spawns.
     *
     * @return Number of spawns.
     */
    unsigned int spawnCount() const { return mSpawns.size(); }

    /**
     * @brief Dispatches a game model event.
     *
     * @param[in] event The event to dispatch.
     */
    virtual void dispatch(
        const GameModelEvent& event );

    /**
     * @brief Performs a single tick.
     *
     * @retval true  The game continues.
     * @retval false The game has ended.
     */
    virtual bool tick() = 0;

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
     * @brief Easier access to an entity.
     *
     * @param[in] pos Coords of the entity.
     *
     * @return The entity.
     */
    GameEntity& at( const GameCoord& pos );

    /// Size of the game map.
    GameCoord mSize;
    /// The game map.
    GameEntity* mMap;

    /// A vector of spawn points.
    std::vector<GameCoord> mSpawns;
    /// A queue of dirty map regions to draw.
    std::queue<GameCoordRect> mDirty;
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
     *
     * @retval true  The game continues.
     * @retval false The game has ended.
     */
    bool tick();

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
        /// Previous position of the entity.
        GameCoord prevpos;
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
        /// Is the entity being processed?
        bool active;
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
        GameBombEntity( const GameCoord& pos_, GameCtlEntity* ctl_ );

        /// Position of the bomb.
        GameCoord pos;
        /// The associated controlled entity.
        GameCtlEntity* ctl;

        /// Number of ticks until explosion.
        unsigned char timer;
        /// Length of the flames.
        unsigned char flames;
    };

    /**
     * @brief Handles spawn of an entity.
     *
     * @param[in]  entity The entity to be spawned.
     * @param[in]  ctl    Controller of the entity.
     */
    void dispatchSpawnEntity( GameEntity entity, GameController* ctl );

    /**
     * @brief Checks if the end conditions have been met.
     *
     * @retval true  The game continues.
     * @retval false The game has ended.
     */
    bool tickEndCond();

    /**
     * @brief Ticks the bombs.
     *
     * @retval true  The active entity has died.
     * @retval false An inactive entity has died.
     */
    bool tickBombs();
    /**
     * @brief Processes a bomb explosion.
     *
     * @param[in] bomb The bomb which exploded.
     *
     * @retval true  The active entity has died.
     * @retval false An inactive entity has died.
     */
    bool tickBombExploded( GameBombEntity& bomb );
    /**
     * @brief Processes spreading of flames.
     *
     * @param[in,out] pos     Initial position of the flame.
     * @param[in]     rowstep How the flame spreads among rows.
     * @param[in]     colstep How the flame spreads among columns.
     * @param[in]     flames  Length of the flame.
     *
     * @retval true  The active entity has died.
     * @retval false An inactive entity has died.
     */
    bool tickBombSpreadFlame( GameCoord& pos, char rowstep, char colstep,
                              unsigned char flames );

    /**
     * @brief Ticks the controlled entities.
     */
    void tickEntities();
    /**
     * @brief Ticks a single entity.
     *
     * @param[in] entity The entity to tick.
     *
     * @retval true  The entity has died.
     * @retval false The entity has not died yet.
     */
    bool tickEntity( GameCtlEntity& entity );

    /**
     * @brief Processes entity move.
     *
     * @param[in] entity  The entity which moved.
     * @param[in] rowstep How many rows it stepped.
     * @param[in] colstep How many cols it stepped.
     *
     * @retval true  The active entity has died.
     * @retval false An inactive entity has died.
     */
    bool tickEntityMoved( GameCtlEntity& entity, char rowstep, char colstep );
    /**
     * @brief Processes putting of a bomb.
     *
     * @param[in] entity The entity which put a bomb.
     */
    void tickEntityPutBomb( GameCtlEntity& entity );
    /**
     * @brief Processes an RC trigger.
     *
     * @param[in] entity The entity which pulled the trigger.
     * @param[in] force  Ignore the rc flag in GameCtlEntity.
     *
     * @retval true  The active entity has died.
     * @retval false An inactive entity has died.
     */
    bool tickEntityRcTrigger( GameCtlEntity& entity, bool force );

    /**
     * @brief Processes death of an entity.
     *
     * @param[in] pos   Position of the entity which died.
     * @param[in] bonus Drop bonus?
     *
     * @retval true  The active entity has died.
     * @retval false An inactive entity has died.
     */
    bool tickEntityDied( const GameCoord& pos, bool bonus );
    /**
     * @brief Processes death of an entity.
     *
     * @param[in] entity The entity which died.
     * @param[in] bonus  Drop bonus?
     *
     * @retval true  The active entity has died.
     * @retval false An inactive entity has died.
     */
    bool tickEntityDied( GameCtlEntity& entity, bool bonus );

    /**
     * @brief Gives a bonus to an entity.
     *
     * @param[in] pos Position of the entity which shall receive a bonus.
     */
    void tickEntityGiveBonus( const GameCoord& pos );
    /**
     * @brief Gives a bonus to an entity.
     *
     * @param[in] entity The entity which shall receive a bonus.
     */
    void tickEntityGiveBonus( GameCtlEntity& entity );

    /// A list of controlled entities.
    std::list<GameCtlEntity> mCtlEntities;
    /// A list of bombs.
    std::list<GameBombEntity> mBombs;

    /// A queue of events to dispatch at next tick.
    std::queue<GameModelEvent> mEventPipe;

    /// A table of all possible in-game interactions.
    static const GameInteraction GAME_INTERACTIONS[GENT_COUNT][GENT_COUNT];
};

#endif /* !__GAME_MODEL__H__INCL__ */
