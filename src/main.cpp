/** @file
 * @brief The program entry point.
 *
 * @author Jan Bobek
 */

#include "GameCanvas.h"
#include "GameController.h"
#include "GameModel.h"
#include "GameModelLoader.h"

void main_menu();
void start_singleplayer();
void start_multiplayer();
void connect_multiplayer();

static volatile bool g_run;

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
    /* Install signal handler. */
    signal( SIGINT, sig_recv );

    /* Init curses. */
    initscr();
    cbreak();
    noecho();
    curs_set( 0 );

    /* Show main menu. */
    main_menu();

    /* End curses, return. */
    endwin();
    return 0;
}

void
main_menu()
{
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
            case 0: start_singleplayer(); break;
            case 1: start_multiplayer(); break;
            case 2: connect_multiplayer(); break;
            case 3: return;
        }
    }
}

void
start_singleplayer()
{
    std::string map;
    choose_file( map, "Vyberte mapu:" );

    GameModel* gm = GameModelLoader<GameLocalModel>::load( map );
    if( !gm )
        msgbox( "Chyba", "Nepodarilo se nacist zvolenou mapu, zvolte prosim jinou." );
    else
        play_game( *gm );

    safeDelete( gm );
}

void
start_multiplayer()
{
    std::string map;
    choose_file( map, "Vyberte mapu:" );
}

void
connect_multiplayer()
{
    // printw( "You selected connect" );
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
    event.coords = GameCoordRect();
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
