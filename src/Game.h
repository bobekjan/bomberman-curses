/** @file
 * @brief General game-related declarations.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_H__INCL__
#define __GAME_H__INCL__

/*************************************************************************/
/* Project-wide includes                                                 */
/*************************************************************************/
#include <cassert>
#include <cstdlib>
#include <cstring>

#include <list>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "util.h"

/*************************************************************************/
/* Game-related declarations                                             */
/*************************************************************************/
/**
 * @brief Holds coordinates of a single point.
 *
 * @author Jan Bobek
 */
struct GameCoord
{
    /// Type of the coordinate value.
    typedef unsigned short coord_t;

    /**
     * @brief Initializes the members.
     *
     * @param[in] r The row number.
     * @param[in] c The column number.
     */
    GameCoord( coord_t r = 0, coord_t c = 0 ) : row( r ), col( c ) {}

    /**
     * @brief Compare coordinates.
     *
     * @param[in] oth The other set of coords.
     *
     * @retval true  The coordinates are equal.
     * @retval false The coordinates are different.
     */
    bool operator==( const GameCoord& oth ) const
    {
        return row == oth.row && col == oth.col;
    }
    /**
     * @brief Compare coordinates.
     *
     * @param[in] oth The other set of coords.
     *
     * @retval true  The coordinates are different.
     * @retval false The coordinates are equal.
     */
    bool operator!=( const GameCoord& oth ) const
    {
        return !( *this == oth );
    }

    /// The row number.
    coord_t row;
    /// The column number.
    coord_t col;
};

/// A pair of GameCoord structs which specify a rectangle.
typedef std::pair<GameCoord, GameCoord> GameCoordRect;

/**
 * @brief Helper macro to iterate over a GameCoordRect.
 *
 * @param[in] cur  Name of the current-point variable.
 * @param[in] rect Name of the GameCoordRect.
 */
#define GAME_COORD_RECT_ITERATE( cur, rect ) \
    for( GameCoord cur( rect.first ); cur.row <= rect.second.row; ++cur.row ) \
        for( cur.col = rect.first.col; cur.col <= rect.second.col; ++cur.col )

/**
 * @brief Describes possible actions at each tick.
 *
 * @author Jan Bobek
 */
enum GameCtlEvent
{
    GCE_NOOP,      ///< Do nothing.
    GCE_MOVEUP,    ///< Move up.
    GCE_MOVEDOWN,  ///< Move down.
    GCE_MOVELEFT,  ///< Move left.
    GCE_MOVERIGHT, ///< Move right.
    GCE_PUTBOMB,   ///< Put a bomb.
    GCE_RCEXPLODE  ///< Trigger an RC explosion.
};

/**
 * @brief Describes possible outcomes of an interaction.
 *
 * @author Jan Bobek
 */
enum GameInteraction
{
    GINT_OK,        ///< Perform expected action (eg. walk in corridor).
    GINT_STOP,      ///< Stop the initiator (eg. wall)

    GINT_DIE,       ///< The target dies
    GINT_DIEBONUS,  ///< The target dies and possibly leaves a bonus behind.

    GINT_KILL,      ///< The initiator dies
    GINT_KILLBONUS, ///< The initiator dies and possibly leaves a bonus behind.

    GINT_GIVEBONUS, ///< The target receives a bonus
    GINT_GETBONUS   ///< The initiator receives a bonus
};

/**
 * @brief Describes each entity present in the game.
 *
 * @author Jan Bobek
 */
enum GameEntity
{
    GENT_NONE,    ///< No entity (corridor)

    GENT_BARRIER, ///< Barrier (unbreakable wall)
    GENT_SPAWN,   ///< Spawn point
    GENT_TRAP,    ///< Deadly trap
    GENT_WALL,    ///< Breakable wall

    GENT_BOMB,    ///< Player bomb
    GENT_BONUS,   ///< Bonus for players
    GENT_FLAME,   ///< Flame of a bomb

    GENT_PLAYER,  ///< Player
    GENT_MONSTER, ///< Player-seeking monster

    GENT_COUNT    ///< Number of entities in the game
};

#endif /* !__GAME_H__INCL__ */
