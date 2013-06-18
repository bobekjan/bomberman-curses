/** @file
 * @brief Concept to control game entities.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_CONTROLLER_H__INCL__
#define __GAME_CONTROLLER_H__INCL__

#include "Game.h"

/**
 * @brief An abstract entity controller.
 *
 * At each tick the controller says what the associated
 * entity intends to do.
 *
 * @author Jan Bobek
 */
class GameController
{
public:
    /**
     * @brief Properly delete the controller.
     */
    virtual ~GameController() {}

    /**
     * @brief Obtain an action for the current tick.
     *
     * @param[out] event Where to store the action.
     */
    virtual void tick( GameCtlEvent& event ) = 0;
};

/**
 * @brief An ncurses-based controller.
 *
 * It pulls its events from stdscr keyboard.
 *
 * @author Jan Bobek
 */
class NcursesController
: public GameController
{
public:
    /**
     * @brief Initializes stdscr as needed.
     */
    NcursesController();

    /**
     * @brief Obtain an action for the current tick.
     *
     * @param[out] event Where to store the action.
     */
    void tick( GameCtlEvent& event );
};

#endif /* !__GAME_CONTROLLER_H__INCL__ */
