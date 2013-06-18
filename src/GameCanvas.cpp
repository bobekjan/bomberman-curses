/** @file
 * @brief Implementation of GameCanvas-related classes.
 *
 * @author Jan Bobek
 */

#include "GameCanvas.h"
#include "GameModel.h"

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

NcursesCanvas::NcursesCanvas(
    GameModel& model
    )
: mWin( NULL )
{
    /* Get size of the screen. */
    int sr, sc;
    getmaxyx( stdscr, sr, sc );

    /* Get size of the game map. */
    GameCoord size = model.size();
    size.row += 2;
    size.col += 2;

    /* Create our window. */
    mWin = newwin(
        size.row, size.col,
        (sr - size.row) / 2,
        (sc - size.col) / 2 );

    /* Box the window. */
    box( mWin, 0, 0 );
}

NcursesCanvas::~NcursesCanvas()
{
    /* Release the window. */
    delwin( mWin );
}

void
NcursesCanvas::draw(
    GameEntity entity,
    const GameCoord& coord
    )
{
    /* Do not forget the border. */
    mvwaddch( mWin, coord.row + 1, coord.col + 1,
              CANVAS_ENTITIES[entity] );
}

void
NcursesCanvas::flush()
{
    wrefresh( mWin );
}
