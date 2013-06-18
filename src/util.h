/** @file
 * @brief Various utilities.
 *
 * @author Jan Bobek
 */

#ifndef __UTIL_H__INCL__
#define __UTIL_H__INCL__

/**
 * @brief Allocates and zero-initializes memory.
 *
 * Useful for allocating constructor-less C structs.
 *
 * @return A pointer to the allocated memory.
 */
template< typename T >
inline T* safeAlloc()
{
    T* p = new T;
    ::memset( p, 0, sizeof( T ) );

    return p;
}

/**
 * @brief Allocates and zero-initializes an array.
 *
 * Useful for allocating constructor-less C struct arrays.
 *
 * @param[in] size Desired size of the array.
 *
 * @return A pointer to the array.
 */
template< typename T >
inline T* safeAllocArray( size_t size )
{
    T* p = new T[ size ];
    ::memset( p, 0, size * sizeof( T ) );

    return p;
}

/**
 * @brief Deletes and nullifies a pointer.
 *
 * Basic programming tips
 * URL: http://nedprod.com/programs/index.html
 * Note: always nullify pointers after deletion, why? because its safer on a MT application
 *
 * @param[in] p The pointer.
 */
template< typename T >
inline void safeDelete( T*& p )
{
    delete p;
    p = NULL;
}

/**
 * @brief Deletes and nullifies an array pointer.
 *
 * @param[in] p The array pointer.
 *
 * @see safeDelete< T >( T*& p )
 */
template< typename T >
inline void safeDeleteArray( T*& p )
{
    delete[] p;
    p = NULL;
}

/**
 * @brief Frees and nullifies a pointer.
 *
 * @param[in] p The pointer.
 *
 * @see safeDelete< T >( T*& p )
 */
template< typename T >
inline void safeFree( T*& p )
{
    ::free( p );
    p = NULL;
}

/**
 * @brief Releases pointer and nullifies it.
 *
 * @param[in] p The pointer.
 * @param[in] f The release function.
 */
template< typename T, typename F >
inline void safeRelease( T*& p, F f )
{
    /* We don't know if the function accepts
       NULL pointers, like delete or free. */
    if( NULL != p )
        f( p );

    p = NULL;
}

/**
 * @brief Prints a centered text.
 *
 * @param[in] w The windown into which to print.
 * @param[in] y The row at which to print.
 * @param[in] x The column at which to center.
 * @param[in] t The text to print.
 */
void mvwaddstr_center( WINDOW* w, int y, int x, const char* t );
/**
 * @brief Pops a message box.
 *
 * @param[in] tit Title of the box.
 * @param[in] msg The message.
 */
void msgbox( const char* tit, const char* msg );
/**
 * @brief Lets a user select an option from a menu.
 *
 * @param[in] tit Title of the menu, optional.
 * @param[in] cnt Number of items in the menu.
 * @param[in] ... The items; pairs of name, description.
 *
 * @return A value of 0..cnt-1, according to what was selected.
 */
unsigned int menu_select( const char* tit, unsigned int cnt, ... );
/**
 * @brief Lets a user select an option from a menu.
 *
 * @param[in] tit Title of the menu, optional.
 * @param[in] its An array of items to choose from.
 *
 * @return A value of 0..cnt-1, according to what was selected.
 */
unsigned int menu_select( const char* tit, ITEM** its );
/**
 * @brief Creates a menu for file choosing.
 *
 * @param[out] name Where the name of the chosen file goes.
 * @param[in]  tit  Title of the menu, optional.
 * @param[in]  dir  Directory with files, optional.
 */
void choose_file( std::string& name, const char* tit,
                  const char* dir = "examples" );

#endif /* __UTIL_H__INCL__ */
