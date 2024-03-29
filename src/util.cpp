/** @file
 * @brief Implementation of various utilities.
 *
 * @author Jan Bobek
 */

#include "Game.h"
#include "util.h"

void
mvwaddstr_center(
    WINDOW* w,
    int y,
    int x,
    const char* t
    )
{
    size_t len = strlen( t );
    mvwaddstr( w, y, x - len / 2, t );
}

void
msgbox(
    const char* tit,
    const char* msg
    )
{
    /* Get size of the screen. */
    int sr, sc;
    getmaxyx( stdscr, sr, sc );

    /* Create a new window. */
    int wr = 7;
    int wc = strlen( msg ) + 4;

    WINDOW* win = newwin(
        wr, wc, (sr - wr) / 2, (sc - wc) / 2 );
    nodelay( win, FALSE );
    box( win, 0, 0 );

    /* Print the content. */
    mvwaddstr( win, 0, 2, tit );
    mvwaddstr( win, 2, 2, msg );
    mvwaddstr( win, 4, 2, "Pokracujte stiskem libovolne klavesy." );

    wrefresh( stdscr );
    wrefresh( win );
    wgetch( win );

    /* Release the window. */
    delwin( win );
    /* Invalidate the screen. */
    touchwin( stdscr );
}

void
askbox(
    const char* tit,
    const char* msg,
    const char* chrs,
    std::string& into
    )
{
    /* Get size of the screen. */
    int sr, sc;
    getmaxyx( stdscr, sr, sc );

    /* Create a new window. */
    int wr = 7;
    int wc = strlen( msg ) + 4;

    WINDOW* win = newwin(
        wr, wc, (sr - wr) / 2, (sc - wc) / 2 );
    nodelay( win, FALSE );
    keypad( win, TRUE );
    box( win, 0, 0 );

    /* Print the content. */
    mvwaddstr( win, 0, 2, tit );
    mvwaddstr( win, 2, 2, msg );
    mvwaddstr( win, 4, 2, into.c_str() );

    /* Show the cursor. */
    int oldcurs = curs_set( 1 );
    /* Show the window. */
    wrefresh( stdscr );

    int c = 0;
    unsigned int idx = into.size();

    while( wrefresh( win ), ERR != c )
    {
        /* Get a character. */
        c = wgetch( win );

        if( strchr( chrs, c ) )
        {
            /* Valid character, add it. */
            if( idx < into.size() )
                into[idx] = c;
            else
                into.push_back( c );

            waddch( win, c ); ++idx;
            continue;
        }

        switch( c )
        {
            case KEY_BACKSPACE:
                /* Delete last character. */
                if( !(idx < into.size()) )
                {
                    into.erase( --into.end() );
                    mvwaddch( win, 4, idx + 1, ' ' );
                }
                /* Fallthrough */

            case KEY_LEFT:
                /* Move cursor to the left. */
                if( 0 < idx )
                    wmove( win, 4, --idx + 2 );
                break;

            case KEY_RIGHT:
                /* Move cursor to the right. */
                if( idx < into.size() )
                    wmove( win, 4, ++idx + 2 );
                break;

            default:
                /* Break out of the loop. */
                c = ERR;
                break;
        }
    }

    /* Put the cursor back as it was. */
    curs_set( oldcurs );

    /* Release the window. */
    delwin( win );
    /* Invalidate the screen. */
    touchwin( stdscr );
}

unsigned int
menu_select(
    const char* tit,
    unsigned int cnt,
    ...
    )
{
    /* Start the va list. */
    va_list ap;
    va_start( ap, cnt );

    /* Allocate memory for menu items. */
    ITEM** items = safeAllocArray<ITEM*>( cnt + 1 );
    /* Fill the menu item array. */
    for( unsigned int i = 0; i < cnt; ++i )
    {
        /* Obtain the name. */
        const char* name = va_arg(ap, const char*);
        /* Obtain the description. */
        const char* desc = va_arg(ap, const char*);
        /* Create the menu item. */
        items[i] = new_item( name, desc );
    }
    /* NULL-terminate! */
    items[cnt] = NULL;

    /* Call our sibling. */
    unsigned int idx = menu_select( tit, items );

    /* Free the menu items. */
    for( unsigned int i = 0; i < cnt; ++i )
        free_item( items[i] );
    /* Delete the menu item array. */
    safeDeleteArray( items );

    /* End the va list. */
    va_end( ap );

    return idx;
}

unsigned int
menu_select(
    const char* tit,
    ITEM** its
    )
{
    /* Get size of the screen. */
    int sr, sc;
    getmaxyx( stdscr, sr, sc );

    /* Create the menu. */
    MENU* menu = new_menu( its );
    /* Get size of the menu. */
    int mr, mc;
    scale_menu( menu, &mr, &mc );

    /* Create a window for the menu. */
    WINDOW* win,* sub;
    /* Do we have a title? */
    if( tit && *tit )
    {
        /* Get dimensions of the window. */
        int wr = mr + 2;
        int wc = std::max<int>( mc, strlen( tit ) );

        win = newwin(
            wr, wc, (sr - wr) / 2, (sc - wc) / 2 );
        sub = derwin( win, mr, mc, wr - mr, (wc - mc) / 2 );

        /* Print the centered title. */
        wattron( win, A_BOLD | A_UNDERLINE );
        mvwaddstr_center( win, 0, wc / 2, tit );
        wattroff( win, A_BOLD | A_UNDERLINE );
    }
    else
    {
        /* Menu size = window size. */
        win = newwin(
            mr, mc, (sr - mr) / 2, (sc - mc) / 2 );
        sub = derwin( win, mr, mc, 0, 0 );
    }

    /* Display the window and menu. */
    nodelay( win, FALSE );
    keypad( win, TRUE );
    set_menu_win( menu, win );
    set_menu_sub( menu, sub );
    set_menu_mark( menu, "" );
    post_menu( menu );

    /* Redraw the entire screen. */
    wrefresh( stdscr );

    /* Let the user select a choice. */
    int c;
    while( wrefresh( win ), '\n' != (c = wgetch( win )) )
        switch( c )
        {
            case KEY_DOWN:
                menu_driver( menu, REQ_DOWN_ITEM );
                break;
            case KEY_UP:
                menu_driver( menu, REQ_UP_ITEM );
                break;
        }

    /* Obtain the selected index. */
    unsigned int idx =
        item_index( current_item( menu ) );

    /* Unpost and free the window and menu. */
    unpost_menu( menu );
    delwin( win );
    free_menu( menu );

    /* Invalidate the entire screen. */
    touchwin( stdscr );

    return idx;
}
