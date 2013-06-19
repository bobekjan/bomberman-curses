/** @file
 * @brief Game model loading-related stuff.
 *
 * @author Jan Bobek
 */

#ifndef __GAME_MODEL_LOADER_H__INCL__
#define __GAME_MODEL_LOADER_H__INCL__

#include "Game.h"

class GameLocalModel;
class GameServerModel;
class GameRemoteModel;

/**
 * @brief Loads a game model.
 */
class GameModelLoader
{
public:
    /**
     * @brief Loads a game model.
     *
     * @return The loaded game model.
     */
    template<typename T>
    static T* load();

protected:
    /**
     * @brief Loads a map into a game model.
     *
     * @param[in] name Name of the map.
     *
     * @return The loaded game model.
     */
    template<typename T>
    static T* loadMap( const std::string& name );

    /**
     * @brief Creates a menu for file choosing.
     *
     * @param[out] name Where the name of the chosen file goes.
     * @param[in]  tit  Title of the menu, optional.
     * @param[in]  dir  Directory with files, optional.
     */
    static void chooseFile( std::string& name, const char* tit,
                            const char* dir = "examples" );
    /**
     * @brief Creates a box for typing an address.
     *
     * @param[out] addr Where to store the address as IP-NUL-port-NUL.
     * @param[in]  msg  A descriptive message.
     *
     * @retval true  Address loaded successfully.
     * @retval false Failed to load an address.
     */
    static bool chooseAddress( std::string& addr, const char* msg );
    /**
     * @brief Translates a character into a GameEntity.
     *
     * @param[in] c The character.
     *
     * @return The translated game entity.
     */
    static GameEntity translate( char c );
};

/* Specializations of the load method. */
template<>
GameLocalModel*  GameModelLoader::load<GameLocalModel>();
template<>
GameServerModel* GameModelLoader::load<GameServerModel>();
template<>
GameRemoteModel* GameModelLoader::load<GameRemoteModel>();

#endif /* !__GAME_MODEL_LOADER_H__INCL__ */
