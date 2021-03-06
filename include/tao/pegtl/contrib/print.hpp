// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#ifndef TAO_PEGTL_CONTRIB_PRINT_HPP
#define TAO_PEGTL_CONTRIB_PRINT_HPP

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <ios>
#include <ostream>
#include <string_view>
#include <utility>
#include <vector>

#include "../config.hpp"
#include "../demangle.hpp"
#include "../visit.hpp"

#include "print_basic_traits.hpp"
#include "print_rules_traits.hpp"

#include "internal/print_utility.hpp"

namespace TAO_PEGTL_NAMESPACE
{
   namespace internal
   {
      template< typename Name >
      struct print_names
      {
         static void visit( std::ostream& os )
         {
            os << demangle< Name >() << '\n';
         }
      };

      template< typename Name >
      struct print_debug
      {
         static void visit( std::ostream& os )
         {
            const auto first = demangle< Name >();
            os << first << '\n';

            const auto second = demangle< typename Name::rule_t >();
            if( first != second ) {
               os << " (aka) " << second << '\n';
            }

            print_subs( os, typename Name::subs_t() );

            os << '\n';
         }

      private:
         template< typename... Rules >
         static void print_subs( std::ostream& os, type_list< Rules... > /*unused*/ )
         {
            ( print_sub< Rules >( os ), ... );
         }

         template< typename Rule >
         static void print_sub( std::ostream& os )
         {
            os << " (sub) " << demangle< Rule >() << '\n';
         }
      };

      template< template< typename... > class Traits >
      struct print_rules
      {
         template< typename Rule >
         struct size
         {
            template< typename Config >
            static void visit( std::size_t& size, Config& pc )
            {
               size = std::max( size, pc.template name< Traits, Rule >().size() );
            }
         };

         template< typename Rule >
         struct impl
         {
            template< typename Config >
            static void visit( std::ostream& os, const std::size_t width, Config& pc )
            {
               if( const auto rule = pc.template name< Traits, Rule >(); !rule.empty() ) {
                  pc.top = demangle< Rule >();
                  os << std::string( 1 + width - rule.size(), ' ' ) << pc.user( rule ) << " = ";
                  Traits< typename Rule::rule_t >::template print< Traits >( os, pc );
                  os << '\n';
               }
            }
         };
      };

   }  // namespace internal

   template< typename Grammar >
   void print_names( std::ostream& os )
   {
      visit< Grammar, internal::print_names >( os );
   }

   template< typename Grammar >
   void print_debug( std::ostream& os )
   {
      visit< Grammar, internal::print_debug >( os );
   }

   template< typename Grammar, template< typename... > class Traits, typename... Args >
   void basic_print( std::ostream& os, Args&&... args )
   {
      std::size_t size = 1;
      Traits< void > pc( std::forward< Args >( args )... );
      visit< Grammar, internal::print_rules< Traits >::template size >( size, pc );
      visit< Grammar, internal::print_rules< Traits >::template impl >( os, size, pc );
   }

   template< typename Grammar, typename... Args >
   void print_basic( std::ostream& os, Args&&... args )
   {
      basic_print< Grammar, print_basic_traits >( os, std::forward< Args >( args )... );
   }

   template< typename Grammar, typename... Args >
   void print_rules( std::ostream& os, Args&&... args )
   {
      basic_print< Grammar, print_rules_traits >( os, std::forward< Args >( args )... );
   }

}  // namespace TAO_PEGTL_NAMESPACE

#endif
