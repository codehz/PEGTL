// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#include "test.hpp"

#include <string>

namespace TAO_PEGTL_NAMESPACE
{
   // clang-format off
   struct C : one< 'c' > {};
   struct B : seq< one< 'b' >, C, one< 'b' > > {};
   struct A : seq< one< 'a' >, B, one< 'a' > > {};
   // clang-format on

   template< typename >
   struct my_action
   {};

   template<>
   struct my_action< A >
   {
      static void apply0( std::string& s )
      {
         s += 'Y';
      }
   };

   template<>
   struct my_action< B >
      : change_states< int >
   {
      template< typename Input >
      static void success( const Input& /*unused*/, int& i, std::string& s )
      {
         s += std::to_string( i );
      }
   };

   template<>
   struct my_action< C >
   {
      static void apply0( int& i )
      {
         i = 42;
      }
   };

   void unit_test()
   {
      memory_input in( "abcba", "" );
      std::string s = "X";
      const auto result = parse< A, my_action >( in, s );
      TAO_PEGTL_TEST_ASSERT( result );
      TAO_PEGTL_TEST_ASSERT( s == "X42Y" );
   }

}  // namespace TAO_PEGTL_NAMESPACE

#include "main.hpp"