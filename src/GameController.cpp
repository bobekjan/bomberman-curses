/** @file
 * @brief Implementation of game controllers.
 *
 * @author Jan Bobek
 */

#include "GameController.h"

/*************************************************************************/
/* NcursesController                                                     */
/*************************************************************************/
NcursesController::NcursesController()
{
    /* Enable keypad and nodelay. */
    keypad( stdscr, TRUE );
    nodelay( stdscr, TRUE );
}

void
NcursesController::tick(
    GameCtlEvent& event
    )
{
    switch( wgetch( stdscr ) )
    {
        case KEY_UP:    event = GCE_MOVEUP; break;
        case KEY_DOWN:  event = GCE_MOVEDOWN; break;
        case KEY_LEFT:  event = GCE_MOVELEFT; break;
        case KEY_RIGHT: event = GCE_MOVERIGHT; break;
        case ' ':       event = GCE_PUTBOMB; break;
        case '\n':      event = GCE_RCEXPLODE; break;
        default:        event = GCE_NOOP; break;
    }
}
