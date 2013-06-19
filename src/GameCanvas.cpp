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
    /* GENT_NONE */    ' ',
    /* GENT_BARRIER */ '#' | A_BOLD,
    /* GENT_SPAWN */    0,
    /* GENT_TRAP */    'O' | A_BOLD | COLOR_PAIR( COLOR_PAIR_MAGENTA ),
    /* GENT_WALL */    'X',
    /* GENT_BOMB */    '.' | A_BOLD,
    /* GENT_BONUS */   '*' | A_BOLD | COLOR_PAIR( COLOR_PAIR_GREEN ),
    /* GENT_FLAME */   'x' | A_BOLD | COLOR_PAIR( COLOR_PAIR_YELLOW ),
    /* GENT_PLAYER */  '@' | A_BOLD | COLOR_PAIR( COLOR_PAIR_CYAN ),
    /* GENT_MONSTER */ '%' | A_BOLD | COLOR_PAIR( COLOR_PAIR_RED )
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
