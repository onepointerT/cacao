
#pragma once

#include <string_view>

#include "config.hpp"
#include "cacao_fs.hpp"
#include "cacao_regex.hpp"
#include "cacao_stack.hpp"
#include "cacao_str.hpp"
#include "cacao_tmpl.hpp"
#include "cacao_vars.hpp"


namespace cacao {


class Environment {
public:
    class Stack;
    class Template;
    class Variable;

    typedef std::vector< Variable* > env_variables_list_t;

    // using cacao::Template;
    // using cacao::Variable;

    class Stack
        :   protected cacao::Stack< Environment::Variable >
    {
    protected:
        friend class Environment::Stack;

    public:
        typedef cacao::Stack< Environment::Variable > stack_t;

        Stack( Config* configuration = nullptr );

        static Stack* readEnvVarList( const env_variables_list_t& evl
                                    , Config* configuratation = nullptr );
        void readToStack( const env_variables_list_t& evl );
        
        using typename stack_t::iterator;
        using typename stack_t::const_iterator;
        using stack_t::size;
        using stack_t::begin;
        using stack_t::end;
        using stack_t::pos;
        using stack_t::pop;
        using stack_t::push;
        using stack_t::contains;
        using stack_t::at;

        void addvar( Environment::Variable* variable );
        void addvar( const std::string key, Environment::Variable* value );
        
        Environment::Variable*& get( const std::string key );
        std::string getValue( const std::string key );

        Environment::Variable* getPos( const unsigned int pos );
    };
};






} // namespace cacao

