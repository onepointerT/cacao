
#pragma once

#include <list>

#include "cacao_regex.hpp"

#include <truth/bool_atomic_term.hpp>
#include <truth/bool_base.hpp>
#include <truth/boolop.hpp>


namespace cacao {

class Logical final {
public:
    class Regex
        :   public RegexCacao
        ,   public truth::BoolType
    {
    public:
        bool is_negated;

        Regex();
    };

    class RegexEnumeration
        :   public std::list< cacao::Logical::Regex* >
    {
    public:
        typedef std::pair< cacao::Logical::Regex*, truth::BoolOperator > operator_concatenation_t;
        typedef std::list< operator_concatenation_t* > op_concat_list_t;

        op_concat_list_t& terms;

        RegexEnumeration();
    };
};

} // namespace cacao

