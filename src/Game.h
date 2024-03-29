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
#include <csignal>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <limits>
#include <list>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

#include <curses.h>
#include <menu.h>

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

/// A color pair of red foreground.
#define COLOR_PAIR_RED     1
/// A color pair of green foreground.
#define COLOR_PAIR_GREEN   2
/// A color pair of yellow foreground.
#define COLOR_PAIR_YELLOW  3
/// A color pair of blue foreground.
#define COLOR_PAIR_BLUE    4
/// A color pair of magenta foreground.
#define COLOR_PAIR_MAGENTA 5
/// A color pair of cyan foreground.
#define COLOR_PAIR_CYAN    6

/// How many ticks per second (game speed)?
#define GAME_TICKS_PER_SEC  15
/// How many ticks before a bomb explodes?
#define GAME_BOMB_TICKS     3 * GAME_TICKS_PER_SEC
/// How many bombs by default?
#define GAME_BOMBS_DEFAULT  1
/// How long flames by default?
#define GAME_FLAMES_DEFAULT 1
/// How many ticks per move by default?
#define GAME_SPEED_DEFAULT  5

/// How many % that a bonus is dropped?
#define GAME_BONUS_PERCENT        10
/// How many % that a bonus is a bomb?
#define GAME_BONUS_BOMB_PERCENT   30
/// How many % that a bonus is flames?
#define GAME_BONUS_FLAMES_PERCENT 30
/// How many % that a bonus is speed?
#define GAME_BONUS_SPEED_PERCENT  30

#endif /* !__GAME_H__INCL__ */
