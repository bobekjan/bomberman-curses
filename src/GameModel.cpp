/** @file
 * @brief Implementation of the game model.
 *
 * @author Jan Bobek
 */

#include "GameModel.h"

/*************************************************************************/
/* GameModel                                                             */
/*************************************************************************/
GameModel::GameModel( const GameCoord& size )
: mSize( size ),
  mMap( safeAllocArray<GameEntity>( size.row * size.col ) )
{
}

GameModel::~GameModel()
{
    safeFree( mMap );
}

void GameModel::dispatch( const GameModelEvent& event )
{
    /* Process the event. */
    GAME_COORD_RECT_ITERATE( cur, event.coords )
        mMap[cur.row * mSize.col + cur.col] = event.entity;

    /* Mark the region dirty. */
    mDirty.push( event.coords );
}

void GameModel::draw( GameCanvas& canvas )
{
    while( !mDirty.empty() )
    {
        /* Pop the dirty region. */
        GameCoordRect region = mDirty.front();
        mDirty.pop();

        /* Iterate over the region. */
        GAME_COORD_RECT_ITERATE( cur, region )
            /* Draw the entity at cur. */
            canvas.draw( mMap[cur.row * mSize.col + cur.col], cur );
    }
}

void GameModel::redraw( GameCanvas& canvas )
{
    /* Clear the dirty regions. */
    while( !mDirty.empty() )
        mDirty.pop();

    /* Mark the entire map dirty. */
    mDirty.push(
        GameCoordRect(
            GameCoord( 0, 0 ),
            GameCoord( mSize ) ) );

    /* Call regular draw. */
    draw( canvas );
}

/*************************************************************************/
/* GameLocalModel                                                        */
/*************************************************************************/
GameLocalModel::GameLocalModel( const GameCoordRect& size )
: GameModel( size )
{
}
