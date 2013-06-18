/** @file
 * @brief The program entry point.
 *
 * @author Jan Bobek
 */

#include "GameCanvas.h"
#include "GameModel.h"

int
main(
    int,
    char*[]
    )
{
    GameCanvas* gc = new NcursesCanvas;
    GameModel* gm = new GameLocalModel( GameCoord( 7, 13 ) );

    safeDelete( gm );
    safeDelete( gc );
    return 0;
}
