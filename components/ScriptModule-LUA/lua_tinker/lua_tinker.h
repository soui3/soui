// lua_tinker.h
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
// 
// please check Licence.txt file for licence and legal issues. 

#if !defined(_LUA_TINKER_H_)
#define _LUA_TINKER_H_

#include <new>
#include <stdint.h>
#include <string.h>
#include <lua.hpp>
#include <stdio.h>
#include <typeinfo>

#ifndef UAPI
#define UAPI __stdcall
#endif

namespace lua_tinker
{
   // init LuaTinker


   // string-buffer excution
   void    dofile ( lua_State *L, const char *filename );
   void    dostring ( lua_State *L, const char* buff );
   void    dobuffer ( lua_State *L, const char* buff, size_t sz );

   // debug helpers
   void    enum_stack ( lua_State *L );
   int     on_error ( lua_State *L );
   void    print_error ( lua_State *L, const char* fmt, ... );

   // dynamic type extention
   struct lua_value
   {
      virtual void to_lua ( lua_State *L ) = 0;
   };

   // type trait
   template<typename T> struct class_name;
   struct table;

   // 编译期间的if，如果C是true就是A类型，false就是B类型
   template<bool C, typename A, typename B> struct if_ {};
   template<typename A, typename B>        struct if_ < true, A, B > { typedef A type; };
   template<typename A, typename B>        struct if_ < false, A, B > { typedef B type; };

   // 判断是否是指针
   template<typename A>
   struct is_ptr { static const bool value = false; };
   template<typename A>
   struct is_ptr < A* > { static const bool value = true; };

   // 判断是否是引用
   template<typename A>
   struct is_ref { static const bool value = false; };
   template<typename A>
   struct is_ref < A& > { static const bool value = true; };

   // 移除const
   template<typename A>
   struct remove_const { typedef A type; };
   template<typename A>
   struct remove_const < const A > { typedef A type; };

   // 获取基本类型 指针以及引用
   template<typename A>
   struct base_type { typedef A type; };
   template<typename A>
   struct base_type < A* > { typedef A type; };
   template<typename A>
   struct base_type < A& > { typedef A type; };

   // 获取类的类型 class A;
   // class_type<A*>::type a_inst;
   // class_type<A&>::type a_inst;
   // class_type<const A*>::type a_inst;
   // class_type<const A&>::type a_inst;
   // 都可以用来声明
   template<typename A>
   struct class_type { typedef typename remove_const<typename base_type<A>::type>::type type; };

   // 判断是否是对象
   template<typename A>
   struct is_obj { static const bool value = true; };
   template<> struct is_obj < char > { static const bool value = false; };
   template<> struct is_obj < unsigned char > { static const bool value = false; };
   template<> struct is_obj < short > { static const bool value = false; };
   template<> struct is_obj < unsigned short > { static const bool value = false; };
   template<> struct is_obj < long > { static const bool value = false; };
   template<> struct is_obj < unsigned long > { static const bool value = false; };
   template<> struct is_obj < int > { static const bool value = false; };
   template<> struct is_obj < unsigned int > { static const bool value = false; };
   template<> struct is_obj < float > { static const bool value = false; };
   template<> struct is_obj < double > { static const bool value = false; };
   template<> struct is_obj < char* > { static const bool value = false; };
   template<> struct is_obj < const char* > { static const bool value = false; };
   template<> struct is_obj < bool > { static const bool value = false; };
   template<> struct is_obj < lua_value* > { static const bool value = false; };
   template<> struct is_obj < long long > { static const bool value = false; };
   template<> struct is_obj < unsigned long long > { static const bool value = false; };
   template<> struct is_obj < table > { static const bool value = false; };

   /////////////////////////////////
   // 数组引用 sizeof(no_type) == 1  sizeof(yes_type) == 2
   enum { no = 1, yes = 2 };
   typedef char ( &no_type )[no];
   typedef char ( &yes_type )[yes];

   // int_conv_type 结构体，里面是它的构造函数，需要传入一个int类型
   struct int_conv_type { int_conv_type ( int ); };

   // 枚举可以隐式转换为int
   // 如果是int类型就会隐式转化为int_conv_type，返回yes_type；否则是no_type
   no_type int_conv_tester ( ... );
   yes_type int_conv_tester ( int_conv_type );

   // 传入枚举类型返回yes_type
   // enum XXX xxx;
   // sizeof(vfnd_ptr_tester(add_ptr(xxx))) == sizeof(yes_type)
   no_type vfnd_ptr_tester ( const volatile char * );
   no_type vfnd_ptr_tester ( const volatile short * );
   no_type vfnd_ptr_tester ( const volatile int * );
   no_type vfnd_ptr_tester ( const volatile long * );
   no_type vfnd_ptr_tester ( const volatile double * );
   no_type vfnd_ptr_tester ( const volatile float * );
   no_type vfnd_ptr_tester ( const volatile bool * );
   yes_type vfnd_ptr_tester ( const volatile void * );

   // 获取指针类型
   template <typename T> T* add_ptr ( T& );

   // bool类型转化为yes_type 或者 no_type
   template <bool C> struct bool_to_yesno { typedef no_type type; };
   template <> struct bool_to_yesno < true > { typedef yes_type type; };

   // 判断是否是枚举变量
   template <typename T>
   struct is_enum
   {
      static T arg;
      static const bool value = ( ( sizeof ( int_conv_tester ( arg ) ) == sizeof ( yes_type ) ) && ( sizeof ( vfnd_ptr_tester ( add_ptr ( arg ) ) ) == sizeof ( yes_type ) ) );
   };
   /////////////////////////////////

   // from lua
   // 输入参数转成T类型
   template<typename T>
   struct void2val { static T invoke ( void* input ){ return *( T* ) input; } };
   // 输入参数转成T类型指针
   template<typename T>
   struct void2ptr { static T* invoke ( void* input ){ return ( T* ) input; } };
   // 输入参数转成T类型引用
   template<typename T>
   struct void2ref { static T& invoke ( void* input ){ return *( T* ) input; } };

   // 将输入参数ptr转换成T T* 或者T&
   template<typename T>
   struct void2type
   {
      static T invoke ( void* ptr )
      {
         return  if_<is_ptr<T>::value
            , void2ptr<typename base_type<T>::type>
            , typename if_<is_ref<T>::value
            , void2ref<typename base_type<T>::type>
            , void2val<typename base_type<T>::type>
            >::type
         >::type::invoke ( ptr );
      }
   };

   // 存储指针的类
   struct user
   {
      user ( void* p ) : m_p ( p ) {}
      virtual ~user ( ) {}
      void* m_p;
   };

   // 将lua栈上索引的userdata转换为T T* T&
   template<typename T>
   struct user2type { static T invoke ( lua_State *L, int index ) { return void2type<T>::invoke ( lua_touserdata ( L, index ) ); } };

   // 将lua栈上索引的number转换为T T* T&
   // T为索引类型
   template<typename T>
   struct lua2enum { static T invoke ( lua_State *L, int index ) { return ( T ) ( int ) lua_tonumber ( L, index ); } };

   // 将lua栈上索引的userdata转换为T T* T&
   // 非userdata报错
   template<typename T>
   struct lua2object
   {
      static T invoke ( lua_State *L, int index )
      {
         if ( !lua_isuserdata ( L, index ) )
         {
            lua_pushstring ( L, "no class at first argument. (forgot ':' expression ?)" );
            lua_error ( L );
         }
         return void2type<T>::invoke ( user2type<user*>::invoke ( L, index )->m_p );
      }
   };

   // 将lua栈上索引的枚举值或者userdata转换为相对应的类型
   template<typename T>
   T lua2type ( lua_State *L, int index )
   {
      return  if_<is_enum<T>::value
         , lua2enum<T>
         , lua2object<T>
      >::type::invoke ( L, index );
   }

   // val转换到user
   template<typename T>
   struct val2user : user
   {
      val2user ( ) : user ( new T ) {}

      template<typename T1>
      val2user ( T1 t1 ) : user ( new T ( t1 ) ) {}

      template<typename T1, typename T2>
      val2user ( T1 t1, T2 t2 ) : user ( new T ( t1, t2 ) ) {}

      template<typename T1, typename T2, typename T3>
      val2user ( T1 t1, T2 t2, T3 t3 ) : user ( new T ( t1, t2, t3 ) ) {}

      template<typename T1, typename T2, typename T3, typename T4>
      val2user ( T1 t1, T2 t2, T3 t3, T4 t4 ) : user ( new T ( t1, t2, t3, t4 ) ) {}

      template<typename T1, typename T2, typename T3, typename T4, typename T5>
      val2user ( T1 t1, T2 t2, T3 t3, T4 t4, T5 t5 ) : user ( new T ( t1, t2, t3, t4, t5 ) ) {}

      // 只有lua调用new在C++堆上分配内存才会被__gc
      ~val2user ( ) { delete ( ( T* ) m_p ); }
   };

   // ptr转化到user
   template<typename T>
   struct ptr2user : user
   {
      // 指针传入不会被__gc
      ptr2user ( T* t ) : user ( ( void* ) t ) {}
   };

   // ref转化到user
   template<typename T>
   struct ref2user : user
   {
      // 引用传入不会被__gc
      ref2user ( T& t ) : user ( &t ) {}
   };

   // to lua
   // 值传入lua 
   // 方法：val2user<T> 分配在lua上，而T类型input分配在C++堆上，通过val2user<T>中的指针指向
   template<typename T>
   struct val2lua { static void invoke ( lua_State *L, T& input ){ new( lua_newuserdata ( L, sizeof ( val2user<T> ) ) ) val2user<T> ( input ); } };
   // 指针传入lua 
   // 方法：ptr2user<T> 分配在lua上，而T指针input存在C++中，通过ptr2user<T>中的指针指向
   template<typename T>
   struct ptr2lua { static void invoke ( lua_State *L, T* input ){ if ( input ) new( lua_newuserdata ( L, sizeof ( ptr2user<T> ) ) ) ptr2user<T> ( input ); else lua_pushnil ( L ); } };
   template<typename T>
   // 指针传入lua 
   // 方法：ref2user<T> 分配在lua上，而T引用input存在C++中，通过ref2user<T>中的指针指向
   struct ref2lua { static void invoke ( lua_State *L, T& input ){ new( lua_newuserdata ( L, sizeof ( ref2user<T> ) ) ) ref2user<T> ( input ); } };

   // 枚举传入lua
   template<typename T>
   struct enum2lua { static void invoke ( lua_State *L, T val ) { lua_pushnumber ( L, ( int ) val ); } };

   // 对象传入lua
   template<typename T>
   struct object2lua
   {
      static void invoke ( lua_State *L, T val )
      {
         if_<is_ptr<T>::value
            , ptr2lua<typename base_type<T>::type>
            , typename if_<is_ref<T>::value
            , ref2lua<typename base_type<T>::type>
            , val2lua<typename base_type<T>::type>
            >::type
         >::type::invoke ( L, val );

         // set C++对象传入lua 设置metatable
         push_meta ( L, class_name<typename class_type<T>::type>::name ( ) );
         lua_setmetatable ( L, -2 );
      }
   };

   // 类型传入lua
   template<typename T>
   void type2lua ( lua_State *L, T val )
   {
      if_<is_enum<T>::value
         , enum2lua<T>
         , object2lua<T>
      >::type::invoke ( L, val );
   }

   // get value from cclosure
   template<typename T>
   T upvalue_ ( lua_State *L )
   {
      // 获取函数指针
      return user2type<T>::invoke ( L, lua_upvalueindex ( 1 ) );
   }

   // read a value from lua stack 
   template<typename T>
   T read ( lua_State *L, int index )             { return lua2type<T> ( L, index ); }

   template<>  char*               read ( lua_State *L, int index );
   template<>  const char*         read ( lua_State *L, int index );
   template<>  char                read ( lua_State *L, int index );
   template<>  unsigned char       read ( lua_State *L, int index );
   template<>  short               read ( lua_State *L, int index );
   template<>  unsigned short      read ( lua_State *L, int index );
   template<>  long                read ( lua_State *L, int index );
   template<>  unsigned long       read ( lua_State *L, int index );
   template<>  int                 read ( lua_State *L, int index );
   template<>  unsigned int        read ( lua_State *L, int index );
   template<>  float               read ( lua_State *L, int index );
   template<>  double              read ( lua_State *L, int index );
   template<>  bool                read ( lua_State *L, int index );
   template<>  void                read ( lua_State *L, int index );
   template<>  long long           read ( lua_State *L, int index );
   template<>  unsigned long long  read ( lua_State *L, int index );
   template<>  table               read ( lua_State *L, int index );

   // push a value to lua stack 
   template<typename T>
   void push ( lua_State *L, T ret )                  { type2lua<T> ( L, ret ); }

   template<>  void push ( lua_State *L, char ret );
   template<>  void push ( lua_State *L, unsigned char ret );
   template<>  void push ( lua_State *L, short ret );
   template<>  void push ( lua_State *L, unsigned short ret );
   template<>  void push ( lua_State *L, long ret );
   template<>  void push ( lua_State *L, unsigned long ret );
   template<>  void push ( lua_State *L, int ret );
   template<>  void push ( lua_State *L, unsigned int ret );
   template<>  void push ( lua_State *L, float ret );
   template<>  void push ( lua_State *L, double ret );
   template<>  void push ( lua_State *L, char* ret );
   template<>  void push ( lua_State *L, const char* ret );
   template<>  void push ( lua_State *L, bool ret );
   template<>  void push ( lua_State *L, lua_value* ret );
   template<>  void push ( lua_State *L, long long ret );
   template<>  void push ( lua_State *L, unsigned long long ret );
   template<>  void push ( lua_State *L, table ret );

   // pop a value from lua stack
   template<typename T>
   T pop ( lua_State *L ) { T t = read<T> ( L, -1 ); lua_pop ( L, 1 ); return t; }

   template<>  void    pop ( lua_State *L );
   template<>  table   pop ( lua_State *L );

   // functor (with return value)
   // C函数
   // upvalue_<>(L)获取函数指针
   // 执行该函数，并压入栈
   template<typename RVal, typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void, typename T5 = void>
   struct functor
   {
      static int invoke ( lua_State *L ) { push ( L, upvalue_<RVal ( *)( T1, T2, T3, T4, T5 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ), read<T4> ( L, 4 ), read<T5> ( L, 5 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, upvalue_<RVal (UAPI *)( T1, T2, T3, T4, T5 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ), read<T4> ( L, 4 ), read<T5> ( L, 5 ) ) ); return 1; }
   };

   template<typename RVal, typename T1, typename T2, typename T3, typename T4>
   struct functor < RVal, T1, T2, T3, T4 >
   {
      static int invoke ( lua_State *L ) { push ( L, upvalue_<RVal ( *)( T1, T2, T3, T4 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ), read<T4> ( L, 4 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, upvalue_<RVal (UAPI *)( T1, T2, T3, T4 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ), read<T4> ( L, 4 ) ) ); return 1; }
   };

   template<typename RVal, typename T1, typename T2, typename T3>
   struct functor < RVal, T1, T2, T3 >
   {
      static int invoke ( lua_State *L ) { push ( L, upvalue_<RVal ( *)( T1, T2, T3 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, upvalue_<RVal (UAPI *)( T1, T2, T3 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ) ) ); return 1; }
   };

   template<typename RVal, typename T1, typename T2>
   struct functor < RVal, T1, T2 >
   {
      static int invoke ( lua_State *L ) { push ( L, upvalue_<RVal ( *)( T1, T2 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, upvalue_<RVal (UAPI *)( T1, T2 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ) ) ); return 1; }
   };

   template<typename RVal, typename T1>
   struct functor < RVal, T1 >
   {
      static int invoke ( lua_State *L ) { push ( L, upvalue_<RVal ( *)( T1 )> ( L )( read<T1> ( L, 1 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, upvalue_<RVal (UAPI *)( T1 )> ( L )( read<T1> ( L, 1 ) ) ); return 1; }
   };

   template<typename RVal>
   struct functor < RVal >
   {
      static int invoke ( lua_State *L ) { push ( L, upvalue_<RVal ( *)( )> ( L )( ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, upvalue_<RVal (UAPI *)( )> ( L )( ) ); return 1; }
   };

   // functor (without return value)
   // C函数
   // upvalue_<>(L)获取函数指针
   // 执行该函数，不用压入栈
   template<typename T1, typename T2, typename T3, typename T4, typename T5>
   struct functor < void, T1, T2, T3, T4, T5 >
   {
      static int invoke ( lua_State *L ) { upvalue_<void ( *)( T1, T2, T3, T4, T5 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ), read<T4> ( L, 4 ), read<T5> ( L, 5 ) ); return 0; }
	  static int invokeU ( lua_State *L ) { upvalue_<void (UAPI *)( T1, T2, T3, T4, T5 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ), read<T4> ( L, 4 ), read<T5> ( L, 5 ) ); return 0; }
   };

   template<typename T1, typename T2, typename T3, typename T4>
   struct functor < void, T1, T2, T3, T4 >
   {
      static int invoke ( lua_State *L ) { upvalue_<void ( *)( T1, T2, T3, T4 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ), read<T4> ( L, 4 ) ); return 0; }
	  static int invokeU ( lua_State *L ) { upvalue_<void (UAPI *)( T1, T2, T3, T4 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ), read<T4> ( L, 4 ) ); return 0; }
   };

   template<typename T1, typename T2, typename T3>
   struct functor < void, T1, T2, T3 >
   {
      static int invoke ( lua_State *L ) { upvalue_<void ( *)( T1, T2, T3 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ) ); return 0; }
	  static int invokeU ( lua_State *L ) { upvalue_<void (UAPI *)( T1, T2, T3 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ), read<T3> ( L, 3 ) ); return 0; }
   };

   template<typename T1, typename T2>
   struct functor < void, T1, T2 >
   {
      static int invoke ( lua_State *L ) { upvalue_<void ( *)( T1, T2 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ) ); return 0; }
	  static int invokeU ( lua_State *L ) { upvalue_<void (UAPI *)( T1, T2 )> ( L )( read<T1> ( L, 1 ), read<T2> ( L, 2 ) ); return 0; }
   };

   template<typename T1>
   struct functor < void, T1 >
   {
      static int invoke ( lua_State *L ) { upvalue_<void ( *)( T1 )> ( L )( read<T1> ( L, 1 ) ); return 0; }
	  static int invokeU ( lua_State *L ) { upvalue_<void (UAPI *)( T1 )> ( L )( read<T1> ( L, 1 ) ); return 0; }
   };

   template<>
   struct functor < void >
   {
      static int invoke ( lua_State *L ) { upvalue_<void ( *)( )> ( L )( ); return 0; }
	  static int invokeU ( lua_State *L ) { upvalue_<void (UAPI *)( )> ( L )( ); return 0; }
   };

   // functor (non-managed)
   template<typename T1>
   struct functor < int, lua_State*, T1 >
   {
      static int invoke ( lua_State *L ) { return upvalue_<int ( *)( lua_State*, T1 )> ( L )( L, read<T1> ( L, 1 ) ); }
	  static int invokeU ( lua_State *L ) { return upvalue_<int (UAPI *)( lua_State*, T1 )> ( L )( L, read<T1> ( L, 1 ) ); }
   };

   template<>
   struct functor < int, lua_State* >
   {
      static int invoke ( lua_State *L ) { return upvalue_<int ( *)( lua_State* )> ( L )( L ); }
	  static int invokeU ( lua_State *L ) { return upvalue_<int (UAPI *)( lua_State* )> ( L )( L ); }
   };

   // push_functor
   // 将栈上数据（函数指针）压入functor<>::invoke闭包
   // 并压入该函数
   template<typename RVal>
   void push_functor ( lua_State *L, RVal ( *func )( ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, functor<RVal>::invoke, 1 );
   }

   template<typename RVal, typename T1>
   void push_functor ( lua_State *L, RVal ( *func )( T1 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, functor<RVal, T1>::invoke, 1 );
   }

   template<typename RVal, typename T1, typename T2>
   void push_functor ( lua_State *L, RVal ( *func )( T1, T2 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, functor<RVal, T1, T2>::invoke, 1 );
   }

   template<typename RVal, typename T1, typename T2, typename T3>
   void push_functor ( lua_State *L, RVal ( *func )( T1, T2, T3 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, functor<RVal, T1, T2, T3>::invoke, 1 );
   }

   template<typename RVal, typename T1, typename T2, typename T3, typename T4>
   void push_functor ( lua_State *L, RVal ( *func )( T1, T2, T3, T4 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, functor<RVal, T1, T2, T3, T4>::invoke, 1 );
   }

   template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
   void push_functor ( lua_State *L, RVal ( *func )( T1, T2, T3, T4, T5 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, functor<RVal, T1, T2, T3, T4, T5>::invoke, 1 );
   }

#ifndef _WIN64
   template<typename RVal>
   void push_functor ( lua_State *L, RVal (UAPI *func )( ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, functor<RVal>::invokeU, 1 );
   }

   template<typename RVal, typename T1>
   void push_functor ( lua_State *L, RVal (UAPI *func )( T1 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, functor<RVal, T1>::invokeU, 1 );
   }

   template<typename RVal, typename T1, typename T2>
   void push_functor ( lua_State *L, RVal (UAPI  *func )( T1, T2 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, functor<RVal, T1, T2>::invokeU, 1 );
   }

   template<typename RVal, typename T1, typename T2, typename T3>
   void push_functor ( lua_State *L, RVal (UAPI  *func )( T1, T2, T3 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, functor<RVal, T1, T2, T3>::invokeU, 1 );
   }

   template<typename RVal, typename T1, typename T2, typename T3, typename T4>
   void push_functor ( lua_State *L, RVal (UAPI  *func )( T1, T2, T3, T4 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, functor<RVal, T1, T2, T3, T4>::invokeU, 1 );
   }

   template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
   void push_functor ( lua_State *L, RVal (UAPI  *func )( T1, T2, T3, T4, T5 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, functor<RVal, T1, T2, T3, T4, T5>::invokeU, 1 );
   }
#endif//_WIN64
   // member variable
   struct var_base
   {
      virtual ~var_base ( ) {};
      virtual void get ( lua_State *L ) = 0;
      virtual void set ( lua_State *L ) = 0;
   };

   template<typename T, typename V>
   struct mem_var : var_base
   {
      V T::*_var;
      mem_var ( V T::*val ) : _var ( val ) {}
      void get ( lua_State *L )  { push<typename if_<is_obj<V>::value, V&, V>::type> ( L, read<T*> ( L, 1 )->*( _var ) ); }
      void set ( lua_State *L )  { read<T*> ( L, 1 )->*( _var ) = read<V> ( L, 3 ); }
   };

   // class member functor (with return value)
   template<typename RVal, typename T, typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void, typename T5 = void>
   struct mem_functor
   {
      static int invoke ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal ( T::* )( T1, T2, T3, T4, T5 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ), read<T5> ( L, 6 ) ) );; return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal (UAPI T::* )( T1, T2, T3, T4, T5 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ), read<T5> ( L, 6 ) ) );; return 1; }
   };

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
   struct mem_functor < RVal, T, T1, T2, T3, T4 >
   {
      static int invoke ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal ( T::* )( T1, T2, T3, T4 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal (UAPI T::* )( T1, T2, T3, T4 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ) ) ); return 1; }
   };

   template<typename RVal, typename T, typename T1, typename T2, typename T3>
   struct mem_functor < RVal, T, T1, T2, T3 >
   {
      static int invoke ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal ( T::* )( T1, T2, T3 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal (UAPI T::* )( T1, T2, T3 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ) ) ); return 1; }
   };

   template<typename RVal, typename T, typename T1, typename T2>
   struct mem_functor < RVal, T, T1, T2 >
   {
      static int invoke ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal ( T::* )( T1, T2 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal (UAPI T::* )( T1, T2 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ) ) ); return 1; }
   };

   template<typename RVal, typename T, typename T1>
   struct mem_functor < RVal, T, T1 >
   {
      static int invoke ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal ( T::* )( T1 )> ( L ) )( read<T1> ( L, 2 ) ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal (UAPI T::* )( T1 )> ( L ) )( read<T1> ( L, 2 ) ) ); return 1; }
   };

   template<typename RVal, typename T>
   struct mem_functor < RVal, T >
   {
      static int invoke ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal ( T::* )( )> ( L ) )( ) ); return 1; }
	  static int invokeU ( lua_State *L ) { push ( L, ( read<T*> ( L, 1 )->*upvalue_<RVal (UAPI T::* )( )> ( L ) )( ) ); return 1; }
   };

   // class member functor (without return value)
   template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
   struct mem_functor < void, T, T1, T2, T3, T4, T5 >
   {
      static int invoke ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void( T::* )( T1, T2, T3, T4, T5 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ), read<T5> ( L, 6 ) ); return 0; }
	  static int invokeU ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void(UAPI T::* )( T1, T2, T3, T4, T5 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ), read<T5> ( L, 6 ) ); return 0; }
   };

   template<typename T, typename T1, typename T2, typename T3, typename T4>
   struct mem_functor < void, T, T1, T2, T3, T4 >
   {
      static int invoke ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void( T::* )( T1, T2, T3, T4 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ) ); return 0; }
	  static int invokeU ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void(UAPI T::* )( T1, T2, T3, T4 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ) ); return 0; }
   };

   template<typename T, typename T1, typename T2, typename T3>
   struct mem_functor < void, T, T1, T2, T3 >
   {
      static int invoke ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void( T::* )( T1, T2, T3 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ) ); return 0; }
	  static int invokeU ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void(UAPI T::* )( T1, T2, T3 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ) ); return 0; }
   };

   template<typename T, typename T1, typename T2>
   struct mem_functor < void, T, T1, T2 >
   {
      static int invoke ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void( T::* )( T1, T2 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ) ); return 0; }
	  static int invokeU ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void(UAPI T::* )( T1, T2 )> ( L ) )( read<T1> ( L, 2 ), read<T2> ( L, 3 ) ); return 0; }
   };

   template<typename T, typename T1>
   struct mem_functor < void, T, T1 >
   {
      static int invoke ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void( T::* )( T1 )> ( L ) )( read<T1> ( L, 2 ) ); return 0; }
	  static int invokeU ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void(UAPI T::* )( T1 )> ( L ) )( read<T1> ( L, 2 ) ); return 0; }
   };

   template<typename T>
   struct mem_functor < void, T >
   {
      static int invoke ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void( T::* )( )> ( L ) )( ); return 0; }
	  static int invokeU ( lua_State *L )  { ( read<T*> ( L, 1 )->*upvalue_<void(UAPI T::* )( )> ( L ) )( ); return 0; }
   };

   // class member functor (non-managed)
   template<typename T, typename T1>
   struct mem_functor < int, T, lua_State*, T1 >
   {
      static int invoke ( lua_State *L ) { return ( read<T*> ( L, 1 )->*upvalue_<int( T::* )( lua_State*, T1 )> ( L ) )( L, read<T1> ( L, 2 ) ); }
	  static int invokeU ( lua_State *L ) { return ( read<T*> ( L, 1 )->*upvalue_<int(UAPI T::* )( lua_State*, T1 )> ( L ) )( L, read<T1> ( L, 2 ) ); }
   };

   template<typename T>
   struct mem_functor < int, T, lua_State* >
   {
      static int invoke ( lua_State *L ) { return ( read<T*> ( L, 1 )->*upvalue_<int( T::* )( lua_State* )> ( L ) )( L ); }
	  static int invokeU ( lua_State *L ) { return ( read<T*> ( L, 1 )->*upvalue_<int(UAPI T::* )( lua_State* )> ( L ) )( L ); }
   };

   // push_functor
   template<typename RVal, typename T>
   void push_functor ( lua_State *L, RVal ( T::*func )( ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T>::invoke, 1 );
   }

   template<typename RVal, typename T>
   void push_functor ( lua_State *L, RVal ( T::*func )( ) const )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1 ) const )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1, T2 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1, T2 ) const )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1, T2, T3 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1, T2, T3 ) const )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1, T2, T3, T4 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3, T4>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1, T2, T3, T4 ) const )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3, T4>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1, T2, T3, T4, T5 ) )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3, T4, T5>::invoke, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
   void push_functor ( lua_State *L, RVal ( T::*func )( T1, T2, T3, T4, T5 ) const )
   {
      ( void ) func;
      lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3, T4, T5>::invoke, 1 );
   }

#ifndef _WIN64
   // push_functor
   template<typename RVal, typename T>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T>::invokeU, 1 );
   }

   template<typename RVal, typename T>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( ) const )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1 ) const )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1, T2 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1, T2 ) const )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1, T2, T3 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1, T2, T3 ) const )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1, T2, T3, T4 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3, T4>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1, T2, T3, T4 ) const )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3, T4>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1, T2, T3, T4, T5 ) )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3, T4, T5>::invokeU, 1 );
   }

   template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
   void push_functor ( lua_State *L, RVal (UAPI T::*func )( T1, T2, T3, T4, T5 ) const )
   {
	   ( void ) func;
	   lua_pushcclosure ( L, mem_functor<RVal, T, T1, T2, T3, T4, T5>::invokeU, 1 );
   }
#endif//_WIN64
   // constructor
   template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
   int constructor ( lua_State *L )
   {
      // 类T构造函数，参数分别为T1 T2 T3 T4 T5 
      // 透过new构建在C++堆上
      new( lua_newuserdata ( L, sizeof ( val2user<T> ) ) ) val2user<T> ( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ), read<T5> ( L, 6 ) );
      // 给实例赋上metatable
      push_meta ( L, class_name<typename class_type<T>::type>::name ( ) );
      lua_setmetatable ( L, -2 );

      return 1;
   }

   template<typename T, typename T1, typename T2, typename T3, typename T4>
   int constructor ( lua_State *L )
   {
      new( lua_newuserdata ( L, sizeof ( val2user<T> ) ) ) val2user<T> ( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ), read<T4> ( L, 5 ) );
      push_meta ( L, class_name<typename class_type<T>::type>::name ( ) );
      lua_setmetatable ( L, -2 );

      return 1;
   }

   template<typename T, typename T1, typename T2, typename T3>
   int constructor ( lua_State *L )
   {
      new( lua_newuserdata ( L, sizeof ( val2user<T> ) ) ) val2user<T> ( read<T1> ( L, 2 ), read<T2> ( L, 3 ), read<T3> ( L, 4 ) );
      push_meta ( L, class_name<typename class_type<T>::type>::name ( ) );
      lua_setmetatable ( L, -2 );

      return 1;
   }

   template<typename T, typename T1, typename T2>
   int constructor ( lua_State *L )
   {
      new( lua_newuserdata ( L, sizeof ( val2user<T> ) ) ) val2user<T> ( read<T1> ( L, 2 ), read<T2> ( L, 3 ) );
      push_meta ( L, class_name<typename class_type<T>::type>::name ( ) );
      lua_setmetatable ( L, -2 );

      return 1;
   }

   template<typename T, typename T1>
   int constructor ( lua_State *L )
   {
      new( lua_newuserdata ( L, sizeof ( val2user<T> ) ) ) val2user<T> ( read<T1> ( L, 2 ) );
      push_meta ( L, class_name<typename class_type<T>::type>::name ( ) );
      lua_setmetatable ( L, -2 );

      return 1;
   }

   template<typename T>
   int constructor ( lua_State *L )
   {
      new( lua_newuserdata ( L, sizeof ( val2user<T> ) ) ) val2user<T> ( );
      push_meta ( L, class_name<typename class_type<T>::type>::name ( ) );
      lua_setmetatable ( L, -2 );

      return 1;
   }

   // destroyer
   template<typename T>
   int destroyer ( lua_State *L )
   {
      // 删除C++堆上分配的内存
      ( ( user* ) lua_touserdata ( L, 1 ) )->~user ( );
      return 0;
   }

   // global function
   template<typename F>
   void def ( lua_State* L, const char* name, F func )
   {
      // 传入函数指针
      lua_pushlightuserdata ( L, ( void* ) func );
      // 压入函数（实际上压入的是functor<>::invoke 真正的函数指针绑定在闭包上)
      push_functor ( L, func );
      // 设置名字
      lua_setglobal ( L, name );
   }

   // global variable
   // 设置全局变量
   template<typename T>
   void set ( lua_State* L, const char* name, T object )
   {
      push ( L, object );
      lua_setglobal ( L, name );
   }

   template<typename T>
   // 获取全局变量
   // get<T>()
   T get ( lua_State* L, const char* name )
   {
      lua_getglobal ( L, name );
      return pop<T> ( L );
   }

   // 设置全局变量
   template<typename T>
   void decl ( lua_State* L, const char* name, T object )
   {
      set ( L, name, object );
   }


   template<typename RVal>
   RVal call ( lua_State* L, const char* name )
   {
      lua_pushcclosure ( L, on_error, 0 );
      int errfunc = lua_gettop ( L );

      lua_getglobal ( L, name );
      if ( lua_isfunction ( L, -1 ) )
      {
         lua_pcall ( L, 0, 1, errfunc );
      }
      else
      {
         print_error ( L, "lua_tinker::call() attempt to call global `%s' (not a function)", name );
      }

      lua_remove ( L, errfunc );
      return pop<RVal> ( L );
   }

   template<typename RVal, typename T1>
   RVal call(lua_State* L, const char* name, T1 arg)
   {
	   lua_pushcclosure(L, on_error, 0);
	   int errfunc = lua_gettop(L);
	   lua_getglobal(L, name);
	   if(lua_isfunction(L,-1))
	   {
		   push(L, arg);
		   if(lua_pcall(L, 1, 1, errfunc) != 0)
		   {
			   lua_pop(L, 1);
		   }
	   }
	   else
	   {
		   print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
	   }

	   lua_remove(L, -2);
	   return pop<RVal>(L);
   }

   template<typename RVal, typename T1, typename T2>
   RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2)
   {
	   lua_pushcclosure(L, on_error, 0);
	   int errfunc = lua_gettop(L);
	   lua_getglobal(L, name);
	   if(lua_isfunction(L,-1))
	   {
		   push(L, arg1);
		   push(L, arg2);
		   if(lua_pcall(L, 2, 1, errfunc) != 0)
		   {
			   lua_pop(L, 1);
		   }
	   }
	   else
	   {
		   print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
	   }

	   lua_remove(L, -2);
	   return pop<RVal>(L);
   }

   template<typename RVal, typename T1, typename T2, typename T3>
   RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3)
   {
	   lua_pushcclosure(L, on_error, 0);
	   int errfunc = lua_gettop(L);
	   lua_getglobal(L, name);
	   if(lua_isfunction(L,-1))
	   {
		   push(L, arg1);
		   push(L, arg2);
		   push(L, arg3);
		   if(lua_pcall(L, 3, 1, errfunc) != 0)
		   {
			   lua_pop(L, 1);
		   }
	   }
	   else
	   {
		   print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
	   }

	   lua_remove(L, -2);
	   return pop<RVal>(L);
   }


   // class helper
   int meta_get ( lua_State *L );
   int meta_set ( lua_State *L );
   void push_meta ( lua_State *L, const char* name );

   // class init
   template<typename T>
   void class_add ( lua_State* L, const char* name )
   {
      // 通过类名设置类table
      // 如果该类没有注册，在lua中是获取不到类信息的
      class_name<T>::name ( name );

      lua_newtable ( L );

      lua_pushstring ( L, "__name" );
      lua_pushstring ( L, name );
      lua_rawset ( L, -3 );

      lua_pushstring ( L, "__index" );
      lua_pushcclosure ( L, meta_get, 0 );
      lua_rawset ( L, -3 );

      lua_pushstring ( L, "__newindex" );
      lua_pushcclosure ( L, meta_set, 0 );
      lua_rawset ( L, -3 );

      lua_pushstring ( L, "__gc" );
      lua_pushcclosure ( L, destroyer<T>, 0 );
      lua_rawset ( L, -3 );

      lua_setglobal ( L, name );
   }

   // Tinker Class Inheritence
   template<typename T, typename P>
   void class_inh ( lua_State* L )
   {
      // 获取类table
      push_meta ( L, class_name<T>::name ( ) );
      if ( lua_istable ( L, -1 ) )
      {
         // 压入父类名字
         lua_pushstring ( L, "__parent" );
         push_meta ( L, class_name<P>::name ( ) );
         lua_rawset ( L, -3 );
      }
      lua_pop ( L, 1 );
   }

   // Tinker Class Constructor
   template<typename T, typename F>
   void class_con ( lua_State* L, F func )
   {
      // 获取类table
      push_meta ( L, class_name<T>::name ( ) );
      if ( lua_istable ( L, -1 ) )
      {
         // 创建新的table __call
         lua_newtable ( L );
         lua_pushstring ( L, "__call" );
         // 压入构造函数
         lua_pushcclosure ( L, func, 0 );
         lua_rawset ( L, -3 );
         // 设置__call为类table的metatable
         lua_setmetatable ( L, -2 );
      }
      lua_pop ( L, 1 );
   }

   // Tinker Class Functions
   template<typename T, typename F>
   void class_def ( lua_State* L, const char* name, F func )
   {
      // 获取类table
      push_meta ( L, class_name<T>::name ( ) );
      if ( lua_istable ( L, -1 ) )
      {
         // 压入函数到类table
         lua_pushstring ( L, name );
         new( lua_newuserdata ( L, sizeof ( F ) ) ) F ( func );
         push_functor ( L, func );
         lua_rawset ( L, -3 );
      }
      lua_pop ( L, 1 );
   }

   // Tinker Class Variables
   template<typename T, typename BASE, typename VAR>
   void class_mem ( lua_State* L, const char* name, VAR BASE::*val )
   {
      // 获取类table
      push_meta ( L, class_name<T>::name ( ) );
      if ( lua_istable ( L, -1 ) )
      {
         // 压入类参数
         lua_pushstring ( L, name );
         new( lua_newuserdata ( L, sizeof ( mem_var<BASE, VAR> ) ) ) mem_var<BASE, VAR> ( val );
         lua_rawset ( L, -3 );
      }
      lua_pop ( L, 1 );
   }

   template<typename T>
   struct class_name
   {
      // global name
      static const char* name ( const char* name = NULL )
      {
         static char temp[256] = "";
         if ( name != NULL ) strncpy ( temp, name, sizeof ( temp ) - 1 );
         return temp;
      }
   };

   // Table Object on Stack
   struct table_obj
   {
      table_obj ( lua_State* L, int index );
      ~table_obj ( );

      void inc_ref ( );
      void dec_ref ( );

      bool validate ( );

      template<typename T>
      void set ( const char* name, T object )
      {
         if ( validate ( ) )
         {
            lua_pushstring ( m_L, name );
            push ( m_L, object );
            lua_settable ( m_L, m_index );
         }
      }

      template<typename T>
      T get ( const char* name )
      {
         if ( validate ( ) )
         {
            lua_pushstring ( m_L, name );
            lua_gettable ( m_L, m_index );
         }
         else
         {
            lua_pushnil ( m_L );
         }

         return pop<T> ( m_L );
      }

      template<typename T>
      T get ( int num )
      {
         if ( validate ( ) )
         {
            lua_pushinteger ( m_L, num );
            lua_gettable ( m_L, m_index );
         }
         else
         {
            lua_pushnil ( m_L );
         }

         return pop<T> ( m_L );
      }

      lua_State*      m_L;
      int             m_index;
      const void*     m_pointer;
      int             m_ref;
   };

   // Table Object Holder
   struct table
   {
      table ( lua_State* L );
      table ( lua_State* L, int index );
      table ( lua_State* L, const char* name );
      table ( const table& input );
      ~table ( );

      template<typename T>
      void set ( const char* name, T object )
      {
         m_obj->set ( name, object );
      }

      template<typename T>
      T get ( const char* name )
      {
         return m_obj->get<T> ( name );
      }

      template<typename T>
      T get ( int num )
      {
         return m_obj->get<T> ( num );
      }

      table_obj*      m_obj;
   };

} // namespace lua_tinker

typedef lua_tinker::table LuaTable;

#endif //_LUA_TINKER_H_
