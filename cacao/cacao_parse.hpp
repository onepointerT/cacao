
#pragma once


#include "cacao_regex.hpp"
#include "cacao_str.hpp"
#include "cacao_transition.hpp"
#include "cacao_vte.hpp"


namespace cacao {


class Parse
    :   public cacao::Transition
    ,   public cacao::StringDict
    ,   public cacao::RegexCacao
{
public:
    typedef typename cacao::Environment::Template Template;

    Parse( const Transition& transition, const cacao::RegexType regex_type = Cacao, const std::string name = "" );
    Parse( const std::string tmpl_str, const std::string code_text, const cacao::RegexType regex_type = Cacao, const std::string name = "" );
    Parse( const RegexCacao& regex, const std::string code_text, const cacao::RegexType regex_type = Cacao, const std::string name = "" );

    unsigned int parse();
};


} // namespace cacao

