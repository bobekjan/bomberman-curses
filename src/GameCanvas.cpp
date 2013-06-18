/** @file
 * @brief Implementation of GameCanvas-related classes.
 *
 * @author Jan Bobek
 */

#include "GameCanvas.h"

/*************************************************************************/
/* NcursesCanvas                                                         */
/*************************************************************************/
const chtype NcursesCanvas::CANVAS_ENTITIES[GENT_COUNT] =
{
    ' ', /* GENT_NONE */
    '#', /* GENT_BARRIER */
    0,   /* GENT_SPAWN */
    'O', /* GENT_TRAP */
    'X', /* GENT_WALL */
    'o', /* GENT_BOMB */
    'b', /* GENT_BONUS */
    'x', /* GENT_FLAME */
    '@', /* GENT_PLAYER */
    'M'  /* GENT_MONSTER */
};

void
NcursesCanvas::draw(
    GameEntity entity,
    const GameCoord& coord
    )
{
    /* Simple enough :) */
    mvaddch( coord.row, coord.col,
             CANVAS_ENTITIES[entity] );
}
