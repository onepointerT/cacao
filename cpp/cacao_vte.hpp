
#pragma once

#include <string_view>

#include "config.hpp"
#include "configstack.hpp"
#include "cacao_fs.hpp"
#include "cacao_regex.hpp"
#include "cacao_str.hpp"


namespace cacao {


class Environment {
public:
    class Stack;
    class Template;
    class Variable;

    typedef std::vector< Variable* > env_variables_list_t;

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

        static Variable::Type getType( const std::string_view var );

        std::string value;
        const Template* tmpl;
        const Template* actions;
        const Template* formatter;

        Variable( const std::string var_str
                , const std::string tmpl = ""
                , const std::string actions = ""
                , const std::string formatter = ""
        );
        Variable( const std::string var_str
                , const Template* tmpl_tmpl = nullptr
                , const Template* tmpl_actions = nullptr
                , const Template* tmpl_formatter = nullptr
        );

        static Variable* fromRegex( const std::string rgx
                                  , const std::string var_str
                                  , const RegexTypeCacao rtype_cacao
        );
        static Variable* fromRegex( const RegexCacao::Match& var_rgxm
                                  , const RegexTypeCacao rtype_cacao
        );
        static Variable* fromCode( const std::string var_str );
        static Range* findVariable( const std::string str
                            , const unsigned int start = 0
                            , const int  end = -1
        );

        typedef std::vector< Range* > range_list_t;
        typedef std::pair< Environment::Stack*, range_list_t* > varenv_pos_t;

        static varenv_pos_t* findVariables( const std::string str
                            , const unsigned int start = 0
                            , const int  end = -1
        );

        using std::string_view::substr;

        operator std::basic_string<char>() const;
    };


    class Template
        :   protected std::string_view
    {
    public:
        enum Type {
            SelectionRegex,
            ActionVariables,
            Formatter,
            undefined
        } tmpltype = Type::undefined;

        const std::string name;

        env_variables_list_t& vars;

        Template( const std::string tmpl_name, const std::string tmpl_str
                , const Template::Type ttype
        );

        static Template* fromRegex( const RegexCacao::Match& tmpl_rgxm
                                  , Template* tmpl = nullptr );
        static Template* fromCode( const std::string tmpl_str
                                  , Template* tmpl = nullptr );;
        static Template* fromFile( const PathLike path
                                  , Template* tmpl = nullptr );
        static Range* findContentUntil( const std::string str
                        , const Range* var_range_current
                        , const Range* var_range_next
        );
        static Range* findContentUntilDelim( const std::string str
                        , const char* delim = ""
        );
        static std::string templateFormatter( Stack* stack
                                , const Variable* current_variable
                                , const std::string var_value
        );

        env_variables_list_t* parse() const;

        std::string transformTo( Stack* stack, const Template* tmpl2 );
        std::string transform( const std::string str
                        , Stack* stack = nullptr
        );
        void readToEnv( Stack* env ) const;
        void readVars( Stack* env ) const;

        operator std::basic_string<char>() const;

        void set( const std::string tmpl_str_update );
    };

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

