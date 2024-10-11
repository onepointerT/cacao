
#pragma once

#include <string_view>

#include "config.hpp"
#include "configstack.hpp"
#include "cacao_fs.hpp"
#include "cacao_str.hpp"


namespace cacao {


class Environment {
public:
    class Stack;
    class Template;

    class Variable
        :   protected std::string_view
    {
    public:
        enum Type {
            store,
            read,
            create,
            undefined
        } vartype = Type::undefined;

        std::string value;
        Template* tmpl;

        Variable( const std::string var_str );

        static Variable* fromCode( const std::string var_str
                            , Variable* variable = nullptr
        );
        static Range* findVariable( const std::string str
                            , const unsigned int start = 0
                            , const int  end = -1
        );

        typedef std::vector< Range* > range_list_t;
        typedef std::pair< Environment::Stack*, range_list_t* > varenv_t;

        static varenv_t* findVariables( const std::string str
                            , const unsigned int start = 0
                            , const int  end = -1
        );

        using std::string_view::substr;

        operator std::basic_string<char>() const;
    };


    class Template
        :   protected std::string
    {
    public:
        const std::string name;

        Template( const std::string tmpl_name, const std::string tmpl_str );

        static Template* fromCode( const std::string tmpl_str );
        static Template* fromFile( const PathLike path );
        static Range* findContentUntil( const std::string str
                        , const Range* var_range_current
                        , const Range* var_range_next
        );
        static Range* findContentUntilDelim( const std::string str
                        , const char* delim = ""
        );
        static std::string templateFormtter( Stack* stack
                                , const Variable* current_variable
                                , const std::string var_value
        );

        typedef std::vector< Variable* > env_variable_list_t;

        env_variable_list_t* read() const;
        std::string transformTo( Stack* stack, const Template* tmpl2 );
        std::string transform( const std::string str
                        , Stack* stack = nullptr
        );
        void readToEnv( Stack* env );

        operator std::basic_string<char>() const;
    };

    class Stack
        :   protected cacao::Stack< Environment::Variable >
    {
        
    public:
        typedef cacao::Stack< Environment::Variable > stack_t;

        Stack( Config* configuration = nullptr );

        using typename stack_t::iterator;
        using typename stack_t::const_iterator;
        using stack_t::size;
        using stack_t::begin;
        using stack_t::end;
        using stack_t::pos;
        using stack_t::pop;
        using stack_t::push;
        using stack_t::contains;

        void addvar( Environment::Variable* variable );
        void addvar( const std::string key, Environment::Variable* value );
        
        Environment::Variable*& get( const std::string key );
        std::string getValue( const std::string key );
    };
};






} // namespace cacao

