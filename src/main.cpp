/** @file
 * @brief The program entry point.
 *
 * @author Jan Bobek
 */

#include "GameCanvas.h"
#include "GameController.h"
#include "GameLocalModel.h"
#include "GameServerModel.h"
#include "GameRemoteModel.h"
#include "GameModelLoader.h"
#include "util.h"

static volatile bool g_run;

void main_menu();
void play_game( GameModel& model );
void sig_recv( int );

int
main(
    int,
    char*[]
    )
{
    /* Seed random numbers. */
    srand( time( NULL ) );
    /* Install the signal handler. */
    signal( SIGINT, sig_recv );

    /* Init curses. */
    initscr();
    cbreak();
    noecho();
    start_color();
    use_default_colors();
    curs_set( 0 );

    /* Initialize colors. */
    init_pair( COLOR_PAIR_RED,     COLOR_RED,     -1 );
    init_pair( COLOR_PAIR_GREEN,   COLOR_GREEN,   -1 );
    init_pair( COLOR_PAIR_YELLOW,  COLOR_YELLOW,  -1 );
    init_pair( COLOR_PAIR_BLUE,    COLOR_BLUE,    -1 );
    init_pair( COLOR_PAIR_MAGENTA, COLOR_MAGENTA, -1 );
    init_pair( COLOR_PAIR_CYAN,    COLOR_CYAN,    -1 );

    /* Show main menu. */
    main_menu();

    /* End curses, return. */
    endwin();
    return 0;
}

void
main_menu()
{
    GameModel* gm;

    while( true )
    {
        /* Erase and box the screen completely. */
        werase( stdscr );
        box( stdscr, 0, 0 );

        /* Redraw the basic text. */
        mvwaddstr_center(
            stdscr, 2, COLS / 2,
            "Bomberman: Semestralni prace" );
        mvwaddstr_center(
            stdscr, 3, COLS / 2,
            "Copyright (c) 2013 Jan Bobek" );

        /* The menu itself. */
        switch(
            menu_select(
                "HLAVNI MENU", 4,
                "Zacit hru jednoho hrace", "",
                "Zacit hru vice hracu", "",
                "Pripojit se ke hre vice hracu", "",
                "Ukoncit hru", "" ) )
        {
            case 0: gm = GameModelLoader::load<GameLocalModel>(); break;
            case 1: gm = GameModelLoader::load<GameServerModel>(); break;
            case 2: gm = GameModelLoader::load<GameRemoteModel>(); break;
            case 3: return;
        }

        if( gm )
            play_game( *gm );

        safeDelete( gm );
    }
}

void
play_game(
    GameModel& model
    )
{
    clock_t timer;
    int skew_us;

    /* Add local player. */
    GameModelEvent event;
    event.entity = GENT_PLAYER;
    event.coords = GameCoordRect( model.size(), model.size() );
    event.ctl = new NcursesController;
    model.dispatch( event );

    /* Initial draw. */
    wrefresh( stdscr );
    GameCanvas* gc = new NcursesCanvas( model );
    model.redraw( *gc );
    gc->flush();

    /* Throw up the run flag. */
    g_run = true;

    /* Main loop. */
    while( g_run )
    {
        /* Start timing. */
        timer = clock();

        /* Do a tick. */
        if( !model.tick() )
            break;
        /* Draw changes. */
        model.draw( *gc );
        /* Flush the changes. */
        gc->flush();

        /* Stop timing. */
        timer = clock() - timer;

        /* Compute skew in us. */
        skew_us = 1000000 / GAME_TICKS_PER_SEC - 1000000 * timer / CLOCKS_PER_SEC;
        if( 0 < skew_us )
            /* Sleep to make just enough game ticks. */
            usleep( skew_us );
    }

    safeDelete( gc );

    /* Print an endgame message. */
    msgbox( "Informace", "Konec hry.                            " );
}

void
sig_recv(
    int
    )
{
    /* Kill the run flag. */
    g_run = false;
}
