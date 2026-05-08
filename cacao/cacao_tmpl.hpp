
#pragma once

#include <list>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <utility>

#include "cacao_fs.hpp"
#include "cacao_logical.hpp"
#include "cacao_regex.hpp"
#include "cacao_stack.hpp"
#include "cacao_str.hpp"


namespace cacao {


// Forward declarations
class Variable;



class Template
    :   protected std::string_view
{
private:
    virtual void init( const Stack< Variable >& intern_stack_of_var );

public:
    typedef std::list< Variable* > env_variables_list_t;
    typedef std::pair< cacao::Logical::Regex*, enum RegexTypeCacao > regex_t;
    typedef std::list< regex_t* > tmpl_regex_t;

    class Regex
        :   public regex_t
        ,   protected cacao::Logical::RegexEnumeration
    {
    public:
        typedef std::pair< std::string, std::string > tmpl_regex_var_t;
        typedef std::list< tmpl_regex_var_t* > tmpl_regex_vars_list_t;
        typedef std::pair< cacao::Range*, cacao::PrefixSuffix* > tmpl_var_occurence_t;
        typedef std::pair< tmpl_var_occurence_t, tmpl_regex_var_t > tmpl_var_t;
        typedef std::list< tmpl_var_t* > tmpl_var_list_t;

        tmpl_regex_vars_list_t& vars;
        tmpl_var_list_t& vars_in_template;

        Regex( const std::string rgx_str );

        virtual void parse();
        virtual void evaluate( const std::string datastr );
    };

    typedef std::pair< cacao::PrefixSuffix*, cacao::Template::Regex* > tmpl_regex_t;
    typedef std::list< tmpl_regex_t* > tmpl_regex_list_t;

    tmpl_regex_list_t& tmpl_rgxs;

    enum Type {
        SelectionRegex, /* Find values in any sequence of characters, and where a value is located */
        ActionVariables, /* Set in variable into the selected value */
        Formatter, /* Format, what a variable/a template got */
        VarFinder, /* Find variables in a sequence of characters with the functionality of templates */
        Traversion, /* Contempt trajection from one template to a later subversion */
        CacaoTemplate, /* Define your own templates in `*.cacao` files */
        undefined /* Something else */
    } tmpltype = Type::undefined;

    env_variables_list_t& vars;
    const std::string name;

    Template( const std::string tmpl_name, const std::string tmpl_str
            , const Template::Type ttype
    );
    Template( const RegexCacao::Match& tmpl_rgxm, const Template::Type ttype );
    Template( const Template* tmpl_other ); // # TODO
    Template( Template& another );

    static Template* fromRegex( const RegexCacao::Match& tmpl_rgxm );
    static Template* fromCode( const std::string tmpl_str );
    static Template* fromFile( const PathLike path );
    

    virtual env_variables_list_t* parse() const;

    void readToEnv( Stack< Variable >* env ) const;
    void readVars( Stack< Variable >* env ) const;

    operator std::basic_string<char>() const;

    virtual void set( const std::string tmpl_str_update );
    virtual bool make( const Stack< Variable >& intern, const Stack< Variable >* stack = nullptr );

    virtual std::string transform( const std::string str
                                 , Stack<Variable>* stack = new Stack<Variable>("default_constructed") ) const;
    virtual std::string transformTo( Stack<Variable>* stack, const cacao::Template& tmpl2 ) const;

    class util {
    public:
        typedef std::variant< RegexCacao*, Variable* > EnclosedContentVariant;
        typedef std::tuple< PrefixSuffix*, EnclosedContentVariant* > ContentTuple;
        typedef std::list< ContentTuple* > ContentList;

        static const Range& findEnclosedContent( const std::string_view cacao_str_part, const unsigned int element_idx
                                               , const size_t search_start_pos = 0, const size_t search_end_pos = 0 );

        static const EnclosedContentVariant& parseEnclosedContent( const std::string_view cacao_str_part );
        static const ContentTuple* parseContentTuple( const std::string_view cacao_str_part, const unsigned int element_idx
                                                    , const size_t search_start_pos = 0, const size_t search_end_pos = 0 );

        static const ContentList& findEnclosedElements( const std::string cacao_str );
    };

    virtual const std::string_view assembleTemplate( const std::string str, const Stack< Variable >& stack ) const;

    virtual const std::string_view findIn( const std::string str, const Stack< Variable >& stack );
};



class TemplateFindUp
    :   public Template
{
private:
    virtual void init( const Stack< Variable >& intern_stack_of_var );

public:
    TemplateFindUp( const std::string tmpl_str );
    TemplateFindUp( const RegexCacao::Match& tmpl_rgxm );

    const std::string_view assembleTemplate( const std::string str, const Stack< Variable >& stack ) const;

    virtual const std::string_view findIn( const std::string str, const Stack< Variable >& stack ) const;
};



class TemplateActions
    :   public Template
{
    
};


class TemplateFormatter
    :   public Template
{
public:
    

    class util {
    public:
        class FormatInserter
            :   protected std::string_view
        {
        public:
            FormatInserter( const std::string format_printf_str, const Variable& var );

            const std::string str() const;
            const Variable& var() const;

            const std::string insertFormat( const Stack< Variable >* stack = nullptr ) const;
        };

        typedef std::tuple< PrefixSuffix*, FormatInserter* > FormatTuple;        
        typedef std::list< FormatTuple* > FormatList;
        
        static std::list< Variable* >& parseVariableList( const std::string& varlist_str );
        static FormatList& parseFormats( const std::string format_str );
    };
};


} // namespace cacao

