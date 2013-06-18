/** @file
 * @brief Game-drawing code.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_CANVAS_H__INCL__
#define __GAME_CANVAS_H__INCL__

#include "Game.h"

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
 * @brief An ncurses game canvas.
 *
 * This canvas uses ncurses as a backend.
 *
 * @author Jan Bobek
 */
class NcursesCanvas
: public GameCanvas
{
public:
    /**
     * @brief Draws an entity at given coords.
     *
     * @param[in] entity The entity to draw.
     * @param[in] coord  The coords at which to draw.
     */
    void draw( GameEntity entity, const GameCoord& coord );

protected:
    /// The GameEntity to char translation table.
    static const chtype CANVAS_ENTITIES[GENT_COUNT];
};

#endif /* !__GAME_CANVAS_H__INCL__ */
