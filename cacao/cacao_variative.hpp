
#pragma once

#include <type_traits>
#include <string_view>

#include "cacao_regex.hpp"
#include "cacao_stack.hpp"
#include "cacao_str.hpp"


namespace cacao {

// Forward declaration
class Variable;
class VariativeStack;


class VariativeName final
    :   protected std::string_view
{
private:
    typename Str::position_list_t& pos_set_ins;

    Stack< Variable >& stack_intern;
    Stack< Variable >& stack_tmpl;
    Stack< Variable >& stack;
    Stack< Variable >& stack_env;

public:
    VariativeName( const std::string vname );

    operator std::basic_string<char>() const;

    using std::string_view::const_iterator;
    using std::string_view::cbegin;
    using std::string_view::cend;
};


class VariativeStack final
    :   protected Stack< Variable >
{
public:
    VariativeStack( const std::string stack_name );

    std::string get( const std::string key ) const;
};


template< class content_type = cacao::VariativeStack >
class VariativeEnvironment
    :   protected Stack< VariateStack >
{
public:
    VariativeEnvironment();

    std::string operator$( const std::string varname );
    
};


class VariativeCollection // Singleton for all stacks.
{};


class VariativeRegex
    :   protected cacao::RegexCacao
    ,   public cacao::PrefixSuffix
{};


} // namespace cacao

