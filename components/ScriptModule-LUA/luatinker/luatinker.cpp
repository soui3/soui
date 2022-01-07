// luatinker.cpp
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright(c) 2005-2007 Kwon-il Lee(zupet@hitel.net)
//
// please check Licence.txt file for licence and legal issues.

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cassert>

#include "luatinker.h"

#if defined(_MSC_VER)
#define I64_FMT "I64"
#elif defined(__APPLE__)
#define I64_FMT "q"
#else
#define I64_FMT "ll"
#endif

/*---------------------------------------------------------------------------*/
/* excution                                                                  */
/*---------------------------------------------------------------------------*/
void luatinker::dofile( lua_State *L, const char *filename )
{
    lua_pushcclosure( L, on_error, 0 );
    int errfunc = lua_gettop( L );

    if( luaL_loadfile( L, filename ) == 0 )
    {
        lua_pcall( L, 0, 1, errfunc );
    }
    else
    {
        print_error( L, "%s", lua_tostring( L, -1 ) );
    }

    lua_remove( L, errfunc );
    lua_pop( L, 1 );
}

void luatinker::dostring( lua_State *L, const char* buff )
{
    luatinker::dobuffer( L, buff, std::strlen( buff ) );
}

void luatinker::dobuffer( lua_State *L, const char* buff, size_t len )
{
    lua_pushcclosure( L, on_error, 0 );
    int errfunc = lua_gettop( L );

    if( luaL_loadbuffer( L, buff, len, "luatinker::dobuffer()" ) == 0 )
    {
        lua_pcall( L, 0, 1, errfunc );
    }
    else
    {
        print_error( L, "%s", lua_tostring( L, -1 ) );
    }

    lua_remove( L, errfunc );
    lua_pop( L, 1 );
}

/*---------------------------------------------------------------------------*/
/* debug helpers                                                             */
/*---------------------------------------------------------------------------*/
static void call_stack( lua_State* L, int n )
{
    lua_Debug ar;
    if( lua_getstack( L, n, &ar ) == 1 )
    {
        lua_getinfo( L, "nSlu", &ar );

        const char* indent;
        if( n == 0 )
        {
            indent = "->\t";
            luatinker::print_error( L, "\t<call stack>" );
        }
        else
        {
            indent = "\t";
        }

        if( ar.name )
        {
            luatinker::print_error( L,
                    "%s%s() : line %d [%s : line %d]",
                    indent, ar.name, ar.currentline, ar.source, ar.linedefined );
        }
        else
        {
            luatinker::print_error( L,
                    "%sunknown : line %d [%s : line %d]",
                    indent, ar.currentline, ar.source, ar.linedefined );
        }

        call_stack( L, n + 1 );
    }
}

int luatinker::on_error( lua_State *L )
{
    print_error( L, "%s", lua_tostring( L, -1 ) );
    call_stack( L, 0 );

    return 0;
}

void luatinker::print_error( lua_State *L, const char* fmt, ... )
{
    char text[4096];

    va_list args;
    va_start( args, fmt );
    vsnprintf( text, sizeof( text ), fmt, args );
    va_end( args );

    lua_getglobal( L, "_ALERT" );

    if( lua_isfunction( L, -1 ) )
    {
        lua_pushstring( L, text );
        lua_call( L, 1, 0 );
    }
    else
    {
        std::printf( "%s\n", text );
        lua_pop( L, 1 );
    }
}

void luatinker::enum_stack( lua_State *L )
{
    int top = lua_gettop( L );

    print_error( L, "%s", "----------stack----------" );
    print_error( L, "Type:%d", top );

    for( int i = 1; i <= lua_gettop( L ); ++i )
    {
        switch( lua_type( L, i ) )
        {
            case LUA_TNIL:
                print_error( L, "\t%s", lua_typename( L, lua_type( L, i ) ) );
                break;
            case LUA_TBOOLEAN:
                print_error( L, "\t%s    %s", lua_typename( L, lua_type( L, i ) ), lua_toboolean( L, i ) ? "true" : "false" );
                break;
            case LUA_TLIGHTUSERDATA:
                print_error( L, "\t%s    0x%08p", lua_typename( L, lua_type( L, i ) ), lua_topointer( L, i ) );
                break;
            case LUA_TNUMBER:
                print_error( L, "\t%s    %f", lua_typename( L, lua_type( L, i ) ), lua_tonumber( L, i ) );
                break;
            case LUA_TSTRING:
                print_error( L, "\t%s    %s", lua_typename( L, lua_type( L, i ) ), lua_tostring( L, i ) );
                break;
            case LUA_TTABLE:
                print_error( L, "\t%s    0x%08p", lua_typename( L, lua_type( L, i ) ), lua_topointer( L, i ) );
                break;
            case LUA_TFUNCTION:
                print_error( L, "\t%s()  0x%08p", lua_typename( L, lua_type( L, i ) ), lua_topointer( L, i ) );
                break;
            case LUA_TUSERDATA:
                print_error( L, "\t%s    0x%08p", lua_typename( L, lua_type( L, i ) ), lua_topointer( L, i ) );
                break;
            case LUA_TTHREAD:
                print_error( L, "\t%s", lua_typename( L, lua_type( L, i ) ) );
                break;
        }
    }

    print_error( L, "%s", "-------------------------" );
}

/*---------------------------------------------------------------------------*/
/* read                                                                      */
/*---------------------------------------------------------------------------*/

template<> char* luatinker::read( lua_State *L, int index )
{
    return ( char* )lua_tostring( L, index );
}

template<> const char* luatinker::read( lua_State *L, int index )
{
    return ( const char* )lua_tostring( L, index );
}

template<> char luatinker::read( lua_State *L, int index )
{
    return ( char )lua_tointeger( L, index );
}

template<> unsigned char luatinker::read( lua_State *L, int index )
{
    return ( unsigned char )lua_isinteger( L, index );
}

template<> short luatinker::read( lua_State *L, int index )
{
    return ( short )lua_tointeger( L, index );
}

template<> unsigned short luatinker::read( lua_State *L, int index )
{
    return ( unsigned short )lua_tointeger( L, index );
}

// long is different between i386 and X86_64 architecture
template<> long luatinker::read(lua_State *L, int index)
{
    if ( lua_isinteger(L,index) )
    {
        return (long)lua_tointeger(L, index);
    }
    else
    {
        return *(long*)lua_touserdata(L, index);
    }
}

template<> unsigned long luatinker::read(lua_State *L, int index)
{
    if ( lua_isinteger(L,index) )
    {
        return (unsigned long)lua_tointeger(L, index);
    }
    else
    {
        return *( unsigned long* )lua_touserdata( L, index );
    }
}

template<> int luatinker::read( lua_State *L, int index )
{
    return ( int )lua_tointeger( L, index );
}

template<> unsigned int luatinker::read( lua_State *L, int index )
{
    return ( unsigned int )lua_tointeger( L, index );
}

template<> float luatinker::read( lua_State *L, int index )
{
    return ( float )lua_tonumber( L, index );
}

template<> double luatinker::read( lua_State *L, int index )
{
    return ( double )lua_tonumber( L, index );
}

template<> bool luatinker::read( lua_State *L, int index )
{
    if ( lua_isboolean( L, index ) )
    {
        return lua_toboolean( L, index ) != 0;
    }
    else
    {
        return lua_tointeger( L, index ) != 0;
    }
}

template<> void luatinker::read( lua_State *L, int index )
{
    ( void ) L;
    ( void ) index;
    return;
}

template<> long long luatinker::read( lua_State *L, int index )
{
    if ( lua_isinteger( L, index ) )
    {
        return ( long long )lua_tointeger( L, index );
    }

    if ( lua_isnumber( L, index ) )
    {
        return ( long long )lua_tonumber( L, index );
    }
    else
    {
        return *( long long* )lua_touserdata( L, index );
    }
}

template<> unsigned long long luatinker::read( lua_State *L, int index )
{
    if ( lua_isinteger( L, index ) )
    {
        return ( unsigned long long )lua_tointeger( L, index );
    }

    if ( lua_isnumber( L, index ) )
    {
        return ( unsigned long long )lua_tonumber( L, index );
    }
    else
    {
        return *( unsigned long long* )lua_touserdata( L, index );
    }
}

template<> luatinker::table luatinker::read( lua_State *L, int index )
{
    return table( L, index );
}

/*---------------------------------------------------------------------------*/
/* push                                                                      */
/*---------------------------------------------------------------------------*/
template<> void luatinker::push( lua_State *L, char ret )
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push( lua_State *L, unsigned char ret )
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push( lua_State *L, short ret )
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push( lua_State *L, unsigned short ret )
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push(lua_State *L, long ret)
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push(lua_State *L, unsigned long ret)
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push( lua_State *L, int ret )
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push( lua_State *L, unsigned int ret )
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push( lua_State *L, float ret )
{
    lua_pushnumber( L, ret );
}

template<> void luatinker::push( lua_State *L, double ret )
{
    lua_pushnumber( L, ret );
}

template<> void luatinker::push( lua_State *L, char* ret )
{
    lua_pushstring( L, ret );
}

template<> void luatinker::push( lua_State *L, const char* ret )
{
    lua_pushstring( L, ret );
}

template<> void luatinker::push( lua_State *L, bool ret )
{
    lua_pushboolean( L, ret );
}

template<> void luatinker::push( lua_State *L, lua_value* ret )
{
    if( ret )
    {
        ret->to_lua( L );
    }
    else
    {
        lua_pushnil( L );
    }
}

template<> void luatinker::push( lua_State *L, long long ret )
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push( lua_State *L, unsigned long long ret )
{
    lua_pushinteger( L, ret );
}

template<> void luatinker::push( lua_State *L, luatinker::table ret )
{
    lua_pushvalue( L, ret.m_obj->m_index );
}

/*---------------------------------------------------------------------------*/
/* pop                                                                       */
/*---------------------------------------------------------------------------*/
template<> void luatinker::pop( lua_State *L )
{
    lua_pop( L, 1 );
}

template<> luatinker::table luatinker::pop( lua_State *L )
{
    return table( L, lua_gettop( L ) );
}

/*---------------------------------------------------------------------------*/
/* Tinker Class Helper                                                       */
/*---------------------------------------------------------------------------*/
static void invoke_parent( lua_State *L )
{
    lua_pushstring( L, "__parent" );
    lua_rawget( L, -2 );

    if( lua_istable( L, -1 ) )
    {
        lua_pushvalue( L, 2 );
        lua_rawget( L, -2 );
        if( !lua_isnil( L, -1 ) )
        {
            lua_remove( L, -2 );
        }
        else
        {
            lua_remove( L, -1 );
            invoke_parent( L );
            lua_remove( L, -2 );
        }
    }
}

int luatinker::meta_get( lua_State *L )
{
    // 传入表 和 索引参数
    // stack: 1.类(userdata) 2.变量(string)
    lua_getmetatable( L, 1 );
    // stack: 1.类(userdata) 2.变量(string) 3.meta(table)
    lua_pushvalue( L, 2 );
    // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.变量(string)
    lua_rawget( L, -2 );
    // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.meta[变量]value值(userdata)

    // 如果存在userdata 存在该变量
    if( lua_isuserdata( L, -1 ) )
    {
        user2type<var_base*>::invoke( L, -1 )->get( L );
        // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.meta[变量]value值(userdata) 5.实际值
        lua_remove( L, -2 );
        // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.实际值
    }
    else if( lua_isnil( L, -1 ) )
    {
        // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.nil
        lua_remove( L, -1 );
        // stack: 1.类(userdata) 2.变量(string) 3.meta(table)
        invoke_parent( L );
        // fix bug by fergus
        // 调用父类也需调用get
        if( lua_isuserdata( L, -1 ) )
        {
            // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.父类中的变量(userdata)
            user2type<var_base*>::invoke( L, -1 )->get( L );
            // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.父类中的变量(userdata) 5.实际值
            lua_remove( L, -2 );
            // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.实际值
        }
        else if( lua_isnil( L, -1 ) )
        {
            // stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.nil
            lua_pushfstring( L, "can't find '%s' class variable.(forgot registering class variable ?)", lua_tostring( L, 2 ) );
            lua_error( L );
        }
    }

    lua_remove( L, -2 );
    // stack: 1.类(userdata) 2.变量(string) 3.实际值

    return 1;
}

int luatinker::meta_set( lua_State *L )
{
    // stack: 1.类(userdata) 2.变量(string) 3.要赋的值
    lua_getmetatable( L, 1 );
    // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table)
    lua_pushvalue( L, 2 );
    // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string)
    lua_rawget( L, -2 );
    // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.meta[变量](userdata mem_var指针)

    if( lua_isuserdata( L, -1 ) )
    {
        user2type<var_base*>::invoke( L, -1 )->set( L );
        // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.meta[变量](userdata mem_var指针)
    }
    else if( lua_isnil( L, -1 ) )
    {
        // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.nil
        lua_remove( L, -1 );
        // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table)
        lua_pushvalue( L, 2 );
        // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string)
        lua_pushvalue( L, 4 );
        // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string) 6.类meta(table)
        invoke_parent( L );
        // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string) 6.类meta(table) 7.获取到父类的变量(userdata mem_var指针)
        if( lua_isuserdata( L, -1 ) )
        {
            user2type<var_base*>::invoke( L, -1 )->set( L );
        }
        else if( lua_isnil( L, -1 ) )
        {
            // stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string) 6.类meta(table) 7.nil
            lua_pushfstring( L, "can't find '%s' class variable.(forgot registering class variable ?)", lua_tostring( L, 2 ) );
            lua_error( L );
        }
    }

    lua_settop( L, 3 );
    // stack: 1.类(userdata) 2.变量(string) 3.要赋的值
    return 0;
}

void luatinker::push_meta( lua_State *L, const char* name )
{
    lua_getglobal( L, name );
}

/*---------------------------------------------------------------------------*/
/* table object on stack                                                     */
/*---------------------------------------------------------------------------*/
luatinker::table_obj::table_obj( lua_State* L, int index )
    : m_L( L ),
      m_index( index ),
      m_ref( 0 )
{
    if( lua_isnil( m_L, m_index ) )
    {
        m_pointer = NULL;
        lua_remove( m_L, m_index );
    }
    else
    {
        m_pointer = lua_topointer( m_L, m_index );
    }
}

luatinker::table_obj::~table_obj( )
{
    if( validate( ) )
    {
        lua_remove( m_L, m_index );
    }
}

void luatinker::table_obj::inc_ref( )
{
    ++m_ref;
}

void luatinker::table_obj::dec_ref( )
{
    if( --m_ref == 0 )
    {
        delete this;
    }
}

bool luatinker::table_obj::validate( )
{
    if ( m_pointer == NULL )
    {
        return false;
    }

    if ( m_pointer == lua_topointer( m_L, m_index ) )
    {
        return true;
    }

    int top = lua_gettop( m_L );

    for ( int i = 1; i <= top; ++i )
    {
        if ( m_pointer == lua_topointer( m_L, i ) )
        {
            m_index = i;
            return true;
        }
    }

    m_pointer = NULL;
    return false;
}

/*---------------------------------------------------------------------------*/
/* Table Object Holder                                                       */
/*---------------------------------------------------------------------------*/
luatinker::table::table( lua_State* L )
{
    lua_newtable( L );

    m_obj = new table_obj( L, lua_gettop( L ) );
    assert( m_obj != NULL && "new table_obj failed" );

    m_obj->inc_ref();
}

luatinker::table::table( lua_State* L, const char* name )
{
    lua_getglobal( L, name );

    if ( lua_istable( L, -1 ) == 0 )
    {
        lua_pop( L, 1 );

        lua_newtable( L );
        lua_setglobal( L, name );
        lua_getglobal( L, name );
    }

    m_obj = new table_obj( L, lua_gettop( L ) );
    assert( m_obj != NULL && "new table_obj failed" );

    m_obj->inc_ref();
}

luatinker::table::table( lua_State* L, int index )
{
    if ( index < 0 )
    {
        index = lua_gettop( L ) + index + 1;
    }

    m_obj = new table_obj( L, index );
    assert( m_obj != NULL && "new table_obj failed" );

    m_obj->inc_ref();
}

luatinker::table::table( const table& input )
{
    m_obj = input.m_obj;
    m_obj->inc_ref();
}

luatinker::table::~table( )
{
    m_obj->dec_ref( );
}

/*---------------------------------------------------------------------------*/
