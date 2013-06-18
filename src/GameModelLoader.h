/** @file
 * @brief Game model loading-related stuff.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_MODEL_LOADER_H__INCL__
#define __GAME_MODEL_LOADER_H__INCL__

#include "Game.h"

/**
 * @brief Loads a game model.
 */
template<typename T>
class GameModelLoader
{
public:
    /**
     * @brief Loads a game model.
     *
     * @param[in] name Name of the map.
     *
     * @return The loaded game model.
     */
    static T* load( const std::string& name );

protected:
    /**
     * @brief Translates a character into a GameEntity.
     *
     * @param[in] c The character.
     *
     * @return The translated game entity.
     */
    static GameEntity translate( char c );
};

// Include template code
#include "GameModelLoader.ipp"

#endif /* !__GAME_MODEL_LOADER_H__INCL__ */
