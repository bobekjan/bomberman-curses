/** @file
 * @brief Implementation of the GameModeLoader template.
 *
 * @author Jan Bobek
 */

/*************************************************************************/
/* GameModelLoader                                                       */
/*************************************************************************/
template<typename T>
T* GameModelLoader<T>::load( const std::string& name )
{
    /* Open the file. */
    std::ifstream file( name.c_str(), std::ios_base::in );
    if( !file )
        return NULL;

    /* Load size of the map. */
    GameCoord size;
    if( !(file >> size.row >> size.col) )
        return NULL;

    /* Skip the rest of the line. */
    if( !file.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n' ))
        return NULL;

    /* Instantiate the game model. */
    T* gm = new T( size );
    GameModelEvent event;
    event.ctl = NULL;

    for( GameCoord cur; cur.row < size.row; ++cur.row )
    {
        for( cur.col = 0; cur.col < size.col; ++cur.col )
        {
            /* Fill the event. */
            event.entity = translate( file.get() );
            event.coords = GameCoordRect( cur, cur );

            if( GENT_COUNT == event.entity )
            {
                /* Invalid format. */
                safeDelete( gm );
                return NULL;
            }

            /* Dispatch the event. */
            gm->dispatch( event );
        }

        if( '\n' != file.get() )
        {
            /* Invalid format. */
            safeDelete( gm );
            return NULL;
        }
    }

    return gm;
}

template<typename T>
GameEntity GameModelLoader<T>::translate( char c )
{
    if( ' ' == c )
        return GENT_NONE;
    else if( '#' == c )
        return GENT_BARRIER;
    else if( '@' == c )
        return GENT_SPAWN;
    else if( 'O' == c )
        return GENT_TRAP;
    else if( 'X' == c )
        return GENT_WALL;
    else
        /* Invalid value. */
        return GENT_COUNT;
}
