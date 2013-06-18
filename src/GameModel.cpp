/** @file
 * @brief Implementation of the game model.
 *
 * @author Jan Bobek
 */

#include "GameModel.h"

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
    safeFree( mMap );
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

GameEntity&
GameModel::at(
    const GameCoord& pos
    )
{
    return mMap[pos.row * mSize.col + pos.col];
}

/*************************************************************************/
/* GameLocalModel                                                        */
/*************************************************************************/
const GameInteraction
GameLocalModel::GAME_INTERACTIONS[GENT_COUNT][GENT_COUNT] =
{
    /* GENT_NONE */
    {
        GINT_STOP,      /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_STOP,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_OK,        /* GENT_BOMB */
        GINT_OK,        /* GENT_BONUS */
        GINT_OK,        /* GENT_FLAME */
        GINT_OK,        /* GENT_PLAYER */
        GINT_OK         /* GENT_MONSTER */
    },
    /* GENT_BARRIER */
    {
        GINT_STOP,      /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_STOP,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_STOP,      /* GENT_BOMB */
        GINT_STOP,      /* GENT_BONUS */
        GINT_STOP,      /* GENT_FLAME */
        GINT_STOP,      /* GENT_PLAYER */
        GINT_STOP       /* GENT_MONSTER */
    },
    /* GENT_SPAWN */
    {
        GINT_STOP,      /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_STOP,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_STOP,      /* GENT_BOMB */
        GINT_STOP,      /* GENT_BONUS */
        GINT_STOP,      /* GENT_FLAME */
        GINT_STOP,      /* GENT_PLAYER */
        GINT_STOP       /* GENT_MONSTER */
    },
    /* GENT_TRAP */
    {
        GINT_STOP,      /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_STOP,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_DIE,       /* GENT_BOMB */
        GINT_DIE,       /* GENT_BONUS */
        GINT_STOP,      /* GENT_FLAME */
        GINT_DIE,       /* GENT_PLAYER */
        GINT_DIE        /* GENT_MONSTER */
    },
    /* GENT_WALL */
    {
        GINT_STOP,      /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_STOP,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_STOP,      /* GENT_BOMB */
        GINT_STOP,      /* GENT_BONUS */
        GINT_KILLBONUS, /* GENT_FLAME */
        GINT_STOP,      /* GENT_PLAYER */
        GINT_STOP       /* GENT_MONSTER */
    },
    /* GENT_BOMB */
    {
        GINT_OK,        /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_KILL,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_STOP,      /* GENT_BOMB */
        GINT_STOP,      /* GENT_BONUS */
        GINT_KILL,      /* GENT_FLAME */
        GINT_STOP,      /* GENT_PLAYER */
        GINT_STOP       /* GENT_MONSTER */
    },
    /* GENT_BONUS */
    {
        GINT_OK,        /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_KILL,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_STOP,      /* GENT_BOMB */
        GINT_STOP,      /* GENT_BONUS */
        GINT_KILL,      /* GENT_FLAME */
        GINT_GIVEBONUS, /* GENT_PLAYER */
        GINT_KILL,      /* GENT_MONSTER */
    },
    /* GENT_FLAME */
    {
        GINT_OK,        /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_STOP,      /* GENT_TRAP */
        GINT_DIEBONUS,  /* GENT_WALL */
        GINT_DIE,       /* GENT_BOMB */
        GINT_DIE,       /* GENT_BONUS */
        GINT_OK,        /* GENT_FLAME */
        GINT_DIE,       /* GENT_PLAYER */
        GINT_DIE        /* GENT_MONSTER */
    },
    /* GENT_PLAYER */
    {
        GINT_OK,        /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_KILL,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_STOP,      /* GENT_BOMB */
        GINT_GETBONUS,  /* GENT_BONUS */
        GINT_KILL,      /* GENT_FLAME */
        GINT_STOP,      /* GENT_PLAYER */
        GINT_KILL       /* GENT_MONSTER */
    },
    /* GENT_MONSTER */
    {
        GINT_OK,        /* GENT_NONE */
        GINT_STOP,      /* GENT_BARRIER */
        GINT_STOP,      /* GENT_SPAWN */
        GINT_KILL,      /* GENT_TRAP */
        GINT_STOP,      /* GENT_WALL */
        GINT_STOP,      /* GENT_BOMB */
        GINT_DIE,       /* GENT_BONUS */
        GINT_KILL,      /* GENT_FLAME */
        GINT_DIE,       /* GENT_PLAYER */
        GINT_STOP       /* GENT_MONSTER */
    }
};

GameLocalModel::GameLocalModel(
    const GameCoord& size
    )
: GameModel( size )
{
}

void
GameLocalModel::dispatch(
    const GameModelEvent& event
    )
{
    if( !event.ctl )
        GameModel::dispatch( event );
    else
        /* Choose a spawn. */
        dispatchSpawnEntity( event.entity, event.ctl );
}

void
GameLocalModel::queue(
    const GameModelEvent& event
    )
{
    mEventPipe.push( event );
}

void
GameLocalModel::tick()
{
    /* Dispatch events in the queue. */
    for(; !mEventPipe.empty(); mEventPipe.pop() )
        dispatch( mEventPipe.front() );

    /* Explode bombs. */
    bool active = tickBombs();
    assert( !active );

    /* Visit controlled entities. */
    tickEntities();
}

void
GameLocalModel::dispatchSpawnEntity(
    GameEntity entity,
    GameController* ctl
    )
{
    GameCoord spawn;
    std::vector<GameCoord> spawns = mSpawns;

    while( true )
    {
        /* Randomly pick a spawn. */
        const size_t spawnIdx = rand() % mSpawns.size();
        spawn = spawns[spawnIdx];

        /* Is it OK? */
        if( GINT_OK == GAME_INTERACTIONS[entity][at( spawn )] )
            break;

        /* It is not, remove it. */
        spawns.erase( spawns.begin() + spawnIdx );
        if( spawns.empty() )
            throw std::runtime_error( "No suitable spawn found." );
    }

    /* Create a controlled entity. */
    mCtlEntities.push_back(
        GameCtlEntity( entity, spawn, NULL ) );

    /* Assign the controller. */
    mCtlEntities.back().ctl = ctl;

    /* Make an event again. */
    GameModelEvent event;
    event.entity = entity;
    event.coords = GameCoordRect( spawn, spawn );
    event.ctl = NULL;
    dispatch( event );
}

bool
GameLocalModel::tickBombs()
{
    bool active = false;

    std::list<GameBombEntity>::iterator cur, end;
    cur = mBombs.begin();
    end = mBombs.end();
    while( cur != end )
        if(
            /* Bomb exploding, continue */
            !cur->timer ||
            /* Timer ticking, continue. */
            --cur->timer )
            ++cur;
        else
        {
            /* The bomb has exploded. */
            active |= tickBombExploded( *cur );
            /* Remove the bomb. */
            cur = mBombs.erase( cur );
        }

    return active;
}

bool
GameLocalModel::tickBombExploded(
    GameBombEntity& bomb
    )
{
    GameCoordRect
        horiz( bomb.pos, bomb.pos ),
        vert( bomb.pos, bomb.pos );
    bool active = false;

    /* Mark as exploding. */
    bomb.timer = 0;
    /* Refund the bomb to the owner. */
    if( bomb.ctl )
        ++bomb.ctl->bombs;

    /* Horizontal left. */
    active |= tickBombSpreadFlame( horiz.first, 0, -1, bomb.flames );
    /* Horizontal right. */
    active |= tickBombSpreadFlame( horiz.second, 0, 1, bomb.flames );
    /* Vertical up. */
    active |= tickBombSpreadFlame( vert.first, -1, 0, bomb.flames );
    /* Vertical down. */
    active |= tickBombSpreadFlame( vert.second, 1, 0, bomb.flames );

    /* Create the event. */
    GameModelEvent event;
    event.entity = GENT_FLAME;
    event.ctl = NULL;
    /* Add the horizontal flames. */
    event.coords = horiz;
    dispatch( event );
    /* Add the vertical flames. */
    event.coords = vert;
    dispatch( event );

    /* Queue the clean event. */
    event.entity = GENT_NONE;
    event.ctl = NULL;
    /* Clean the horizontal flames. */
    event.coords = horiz;
    queue( event );
    /* Clean the vertical flames. */
    event.coords = vert;
    queue( event );

    return active;
}

bool
GameLocalModel::tickBombSpreadFlame(
    GameCoord& pos,
    char rowstep,
    char colstep,
    unsigned char flames
    )
{
    while(
        /* Limit by length of flames. */
        0 < flames-- &&
        /* Limit by height of the game map. */
        pos.row + rowstep < mSize.row &&
        /* Limit by width of the game map. */
        pos.col + colstep < mSize.col )
    {
        /* Get the point where the flame should spread. */
        const GameEntity target = at(
            GameCoord( pos.row + rowstep, pos.col + colstep ) );
        /* Get the interaction. */
        const GameInteraction gint = GAME_INTERACTIONS[GENT_FLAME][target];

        switch( gint )
        {
            case GINT_OK:
                break;

            case GINT_DIE:
            case GINT_DIEBONUS:
                /* Kill the entity, stop the flame. */
                return tickEntityDied( pos, GINT_DIEBONUS == gint );

            case GINT_STOP:
            case GINT_KILL:
            case GINT_KILLBONUS:
            case GINT_GETBONUS:
            case GINT_GIVEBONUS:
                /* Just stop the flame. */
                return false;
        }

        /* The flame spreads. */
        pos.row += rowstep;
        pos.col += colstep;
    }

    return false;
}

void
GameLocalModel::tickEntities()
{
    bool died;

    std::list<GameCtlEntity>::iterator cur, end;
    cur = mCtlEntities.begin();
    end = mCtlEntities.end();
    while( cur != end )
    {
        cur->active = true;
        died = tickEntity( *cur );
        cur->active = false;

        if( died )
            /* Erase it */
            cur = mCtlEntities.erase( cur );
        else
            ++cur;
    }
}

bool
GameLocalModel::tickEntity(
    GameCtlEntity& entity
    )
{
    bool active = false;

    /* Get control event of the entity. */
    GameCtlEvent event;
    entity.ctl->tick( event );

    /* Handle the event. */
    switch( event )
    {
        case GCE_NOOP:      /* Fair enough :) */ break;
        case GCE_MOVEUP:    active = tickEntityMoved( entity, -1, 0 ); break;
        case GCE_MOVEDOWN:  active = tickEntityMoved( entity, 1, 0 ); break;
        case GCE_MOVELEFT:  active = tickEntityMoved( entity, 0, -1 ); break;
        case GCE_MOVERIGHT: active = tickEntityMoved( entity, 0, 1 ); break;
        case GCE_PUTBOMB:   tickEntityPutBomb( entity ); break;
        case GCE_RCEXPLODE: active = tickEntityRcTrigger( entity, false ); break;
    }

    /* Tick the move timer. */
    if( 0 < entity.nextmove )
        --entity.nextmove;

    return active;
}

bool
GameLocalModel::tickEntityMoved(
    GameCtlEntity& entity,
    char rowstep,
    char colstep
    )
{
    const GameCoord newpos(
        entity.pos.row + rowstep,
        entity.pos.col + colstep );

    if(
        /* Limit by move timer. */
        0 == entity.nextmove &&
        /* Limit by height of the game map. */
        newpos.row < mSize.row &&
        /* Limit by width of the game map. */
        newpos.col < mSize.col )
    {
        /* Look at the entity where we are going. */
        const GameEntity ent = at( newpos );
        const GameInteraction gint = GAME_INTERACTIONS[entity.ent][ent];
        switch( gint )
        {
            case GINT_OK:        /* Okay. */ break;
            case GINT_STOP:      /* No can do. */ return false;
            case GINT_DIE:
            case GINT_DIEBONUS:  return tickEntityDied( newpos, GINT_DIEBONUS == gint );
            case GINT_KILL:
            case GINT_KILLBONUS: return tickEntityDied( entity, GINT_KILLBONUS == gint );
            case GINT_GIVEBONUS: tickEntityGiveBonus( newpos ); return false;
            case GINT_GETBONUS:  tickEntityGiveBonus( entity ); return false;
        }

        /* I guess we're good to go. */
        entity.prevpos  = entity.pos;
        entity.pos      = newpos;
        entity.nextmove = entity.speed;

        /* Create the event. */
        GameModelEvent event;

        event.entity = entity.ent;
        event.coords = GameCoordRect(
            newpos, newpos );
        event.ctl = NULL;
        dispatch( event );

        event.entity = GENT_NONE;
        event.coords = GameCoordRect(
            entity.prevpos, entity.prevpos );
        event.ctl = NULL;
        dispatch( event );
    }

    return false;
}

void
GameLocalModel::tickEntityPutBomb(
    GameCtlEntity& entity
    )
{
    if(
        /* Limit by number of bombs. */
        0 < entity.bombs &&
        /* Limit by movement. */
        entity.pos != entity.prevpos )
    {
        /* Look where we go after that. */
        const GameEntity ent = at( entity.prevpos );
        switch( GAME_INTERACTIONS[entity.ent][ent] )
        {
            case GINT_OK:        /* Okay. */ break;
            case GINT_STOP:
            case GINT_DIE:
            case GINT_DIEBONUS:
            case GINT_KILL:
            case GINT_KILLBONUS:
            case GINT_GIVEBONUS:
            case GINT_GETBONUS:  /* No can do. */ return;
        }

        /* Construct the bomb. */
        mBombs.push_back(
            GameBombEntity(
                entity.pos, &entity ) );
        /* Take the bomb from the owner. */
        --entity.bombs;

        /* Move player to previous position. */
        GameModelEvent event;
        event.entity = GENT_PLAYER;
        event.coords = GameCoordRect(
            entity.prevpos, entity.prevpos );
        event.ctl = NULL;
        dispatch( event );

        /* Put bomb to player position. */
        event.entity = GENT_BOMB;
        event.coords = GameCoordRect(
            entity.pos, entity.pos );
        event.ctl = NULL;
        dispatch( event );

        /* Update the player position. */
        entity.pos = entity.prevpos;
    }
}

bool
GameLocalModel::tickEntityRcTrigger(
    GameCtlEntity& entity,
    bool force
    )
{
    bool active = false;

    if( entity.rc || force )
    {
        /* Let us blow stuff up. */
        std::list<GameBombEntity>::iterator cur, end;
        cur = mBombs.begin();
        end = mBombs.end();
        while( cur != end )
            if(
                /* Not us ... */
                cur->ctl != &entity ||
                /* Already exploding */
                !cur->timer )
                ++cur;
            else
            {
                /* That's our bomb, blow it. */
                active |= tickBombExploded( *cur );
                /* And remove it. */
                cur = mBombs.erase( cur );
            }
    }

    return active;
}

bool
GameLocalModel::tickEntityDied(
    const GameCoord& pos,
    bool bonus
    )
{
    bool active = false;

    const GameEntity ent = at( pos );
    switch( ent )
    {
        case GENT_BOMB:
        {
            /* We need to find the GameBombEntity ... */
            std::list<GameBombEntity>::iterator cur, end;
            cur = mBombs.begin();
            end = mBombs.end();
            for(; cur != end; ++cur )
                if( cur->pos == pos &&
                    /* It must have nonzero timer. */
                    cur->timer )
                {
                    /* Chain explosion, ka-boom */
                    active |= tickBombExploded( *cur );
                    /* Remove the bomb. */
                    mBombs.erase( cur );
                    break;
                }
        } break;

        case GENT_PLAYER:
        case GENT_MONSTER:
        {
            /* We need to find the GameCtlEntity ... */
            std::list<GameCtlEntity>::iterator cur, end;
            cur = mCtlEntities.begin();
            end = mCtlEntities.end();
            for(; cur != end; ++cur )
                if( cur->pos == pos )
                {
                    active = tickEntityDied( *cur, bonus );
                    if( !active )
                        /* Not active, safely remove. */
                        mCtlEntities.erase( cur );

                    return active;
                }
        } break;

        default:
        case GENT_NONE:
        case GENT_BARRIER:
        case GENT_SPAWN:
        case GENT_TRAP:
        case GENT_WALL:
        case GENT_BONUS:
        case GENT_FLAME:
            break;
    }

    /* Place something else instead. */
    GameModelEvent event;
    event.entity = (bonus && !(rand() % 10) ? GENT_BONUS : GENT_NONE);
    event.coords = GameCoordRect( pos, pos );
    event.ctl = NULL;
    dispatch( event );

    return active;
}

bool
GameLocalModel::tickEntityDied(
    GameCtlEntity& entity,
    bool bonus
    )
{
    /* Decouple the bombs. */
    std::list<GameBombEntity>::iterator cur, end;
    cur = mBombs.begin();
    end = mBombs.end();
    for(; cur != end; ++cur )
        if( &entity == cur->ctl )
            /* Decouple */
            cur->ctl = NULL;

    /* Place something else instead. */
    GameModelEvent event;
    event.entity = (bonus && !(rand() % 10) ? GENT_BONUS : GENT_NONE);
    event.coords = GameCoordRect( entity.pos, entity.pos );
    event.ctl = NULL;
    dispatch( event );

    /* Is it active entity? */
    return entity.active;
}

void
GameLocalModel::tickEntityGiveBonus(
    const GameCoord& pos
    )
{
    std::list<GameCtlEntity>::iterator cur, end;
    cur = mCtlEntities.begin();
    end = mCtlEntities.end();
    for(; cur != end; ++cur )
        if( pos == cur->pos )
        {
            tickEntityGiveBonus( *cur );
            break;
        }
}

void
GameLocalModel::tickEntityGiveBonus(
    GameCtlEntity& entity
    )
{
    switch( entity.ent )
    {
        case GENT_PLAYER:
        {
            unsigned int n = rand() % 100;

            if( n < 30 )
                /* More bombs. */
                ++entity.bombs;
            else if( n < 60 )
                /* More flames. */
                ++entity.flames;
            else if( n < 90 )
            {
                /* Increase speed. */
                if( 1 < entity.speed )
                    --entity.speed;
            }
            else
                /* Remote control! */
                entity.rc = true;
        } break;

        default:
        case GENT_NONE:
        case GENT_BARRIER:
        case GENT_SPAWN:
        case GENT_TRAP:
        case GENT_WALL:
        case GENT_BOMB:
        case GENT_BONUS:
        case GENT_FLAME:
        case GENT_MONSTER:
            break;
    }
}

/*************************************************************************/
/* GameLocalModel::GameCtlEntity                                         */
/*************************************************************************/
GameLocalModel::GameCtlEntity::GameCtlEntity(
    GameEntity ent_,
    const GameCoord& pos_,
    GameController* ctl_
    )
: ent( ent_ ),
  pos( pos_ ),
  prevpos( pos_ ),
  ctl( ctl_ ),
  /* Initialize some sane defaults. */
  bombs( ent_ == GENT_PLAYER ? 1 : 0 ),
  flames( 1 ),
  speed( 5 ),
  rc( false ),
  nextmove( 0 ),
  active( false )
{
}

GameLocalModel::GameCtlEntity::~GameCtlEntity()
{
    /* Delete the controller. */
    safeDelete( ctl );
}

/*************************************************************************/
/* GameLocalModel::GameBombEntity                                        */
/*************************************************************************/
GameLocalModel::GameBombEntity::GameBombEntity(
    const GameCoord& pos_,
    GameCtlEntity* ctl_
    )
: pos( pos_ ),
  ctl( ctl_ ),
  timer( 50 ),
  flames( ctl_->flames )
{
}
