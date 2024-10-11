
#pragma once

#include "config.hpp"

namespace cacao {



template< typename content_type = StringDict >
class Stack
    :   protected Config
    ,   protected std::map< std::string, content_type* >
{
protected:

public:
    typedef typename std::map< std::string, content_type* > cmap_t;

    const std::string stack_type;

    using typename cmap_t::iterator;
    using typename cmap_t::const_iterator;
    using cmap_t::begin;
    using cmap_t::end;
    using cmap_t::at;
    using cmap_t::size;
    using cmap_t::contains;
    using Config::operator+=;
    using Config::operator[];

    Stack( const std::string stype, const Config& configuration )
        :   Config( configuration )
        ,   std::map< std::string, content_type* >()
        ,   stack_type( stype )
    {}

    void push( const std::string key, content_type* value ) {
        cmap_t::at(key) = value;
    }

    Config* config() {
        return this;
    }

    content_type*& pop() {
        content_type* elem = *cmap_t::begin();
        cmap_t::erase( cmap_t::begin() );
        return elem;
    }

    content_type* pos( const unsigned int pidx ) {
        if ( pidx >= cmap_t::size() ) {
            return nullptr;
        }
        unsigned int ppos = 0;
        for ( typename cmap_t::iterator it = cmap_t::begin()
            ; it != cmap_t::end(); ++it, ppos++
        ) {
            if ( ppos == pidx ) {
                return it->second;
            }
        }
        return nullptr;
    }
};


} // namespace cacao

