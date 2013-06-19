/** @file
 * @brief A game model-related declarations.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_MODEL__H__INCL__
#define __GAME_MODEL__H__INCL__

#include "Game.h"

class GameCanvas;
class GameController;

/* This structure is going over the wire, so
   disable any performance aligning. */
#pragma pack( push, 1 )
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
#pragma pack( pop )

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

#endif /* !__GAME_MODEL__H__INCL__ */
