
#pragma once

#include <map>
#include <string>

#include "cacao_str.hpp"

namespace cacao {


class Config;


class StringDict
    :   protected std::map< std::string, std::string >
    ,   protected std::string
{
protected:


public:
    typedef std::map< std::string, std::string > dict_t;
    typedef std::string string_t;
    
    using dict_t::at;
    using dict_t::const_iterator;
    using dict_t::iterator;
    using dict_t::begin;
    using dict_t::end;
    using dict_t::find;
    using dict_t::operator[];
    using dict_t::operator=;

    using string_t::operator std::basic_string_view<char, std::char_traits<char>>;
    using string_t::c_str;
    using string_t::length;
    using string_t::substr;
    using string_t::swap;

    StringDict( const std::string dict_name = "" );

    friend class Config;
};


class Config
    :   public std::map< std::string, StringDict* >
{
public:
    typedef std::map< std::string, StringDict* > map_t;

    Config();
 
    using map_t::at;
    using map_t::const_iterator;
    using map_t::iterator;
    using map_t::begin;
    using map_t::end;
    using map_t::operator[];
    using map_t::operator=;

    std::string& config( const std::string dct_name, const std::string key );
    void operator+=( StringDict* strdct );
};


} // namespace cacao

