
#include "cacao_parse.hpp"

#include "cacao_vars.hpp"


namespace cacao {


Parse::Parse( const Transition& transition, const cacao::RegexType regex_type, const std::string name )
    :   Transition( transition )
    ,   StringDict( name )
    ,   RegexCacao( transition.equiv->tmpl_refactoring == nullptr ? "" : *transition.equiv->tmpl_refactoring, regex_type )
{}


Parse::Parse( const std::string tmpl_str, const std::string code_text, const cacao::RegexType regex_type, const std::string name )
    :   Transition( code_text, tmpl_str )
    ,   StringDict( name )
{}


unsigned int Parse::parse() {
    Stack<Variable>* estack = new Stack<Variable>();
    
    this->equiv->transformation( estack );

    this->clear();
    for ( unsigned int v = 0; v < estack->size(); v++ ) {
        Variable* var = estack->getPos( v );
        this->emplace( var->substr(), var->value );
    }

    return estack->size();
}

} // namespace cacao

