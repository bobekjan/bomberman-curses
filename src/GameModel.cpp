/** @file
 * @brief Implementation of the game model.
 *
 * @author Jan Bobek
 */

#include "GameModel.h"
#include "GameCanvas.h"
#include "util.h"

/*************************************************************************/
/* GameModel                                                             */
/*************************************************************************/
GameModel::GameModel(
    const GameCoord& size
    )
: mSize( size ),
  mMap( safeAllocArray<GameEntity>( size.row * size.col ) )
{
}

GameModel::~GameModel()
{
    safeDeleteArray( mMap );
}

void
GameModel::dispatch(
    const GameModelEvent& event
    )
{
    if( GENT_SPAWN == event.entity )
    {
        /* Add spawn point(s) */
        GAME_COORD_RECT_ITERATE( cur, event.coords )
            mSpawns.push_back( cur );
    }
    else
    {
        /* Process the event. */
        GAME_COORD_RECT_ITERATE( cur, event.coords )
            at( cur ) = event.entity;

        /* Mark the region dirty. */
        mDirty.push( event.coords );
    }
}

void
GameModel::draw(
    GameCanvas& canvas
    )
{
    while( !mDirty.empty() )
    {
        /* Pop the dirty region. */
        GameCoordRect region = mDirty.front();
        mDirty.pop();

        /* Iterate over the region. */
        GAME_COORD_RECT_ITERATE( cur, region )
            /* Draw the entity at cur. */
            canvas.draw( at( cur ), cur );
    }
}

void
GameModel::redraw(
    GameCanvas& canvas
    )
{
    if( !mSize.row || !mSize.col )
        /* Empty map ... */
        return;

    /* Clear the dirty regions. */
    while( !mDirty.empty() )
        mDirty.pop();

    /* Mark the entire map dirty. */
    mDirty.push(
        GameCoordRect(
            GameCoord( 0, 0 ),
            GameCoord( mSize.row - 1,
                       mSize.col - 1 ) ) );

    /* Call regular draw. */
    draw( canvas );
}

GameEntity&
GameModel::at(
    const GameCoord& pos
    )
{
    return mMap[pos.row * mSize.col + pos.col];
}
