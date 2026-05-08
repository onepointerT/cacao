
#pragma once

#include <string>

#include "cacao_fs.hpp"
#include "cacao_tmpl.hpp"
#include "cacao_vte.hpp"


namespace cacao {



class Transition {
public:
    class Equivalence {
    public:
        cacao::Template* tmpl_before = nullptr;
        cacao::Template* tmpl_refactoring = nullptr;

        Equivalence( const std::string tmpl_before_str
                   , const std::string tmpl_refactoring_str
        );

        std::string transformation( Environment::Stack* stack = new Environment::Stack() );
    };


    std::string name;
    Equivalence* equiv = nullptr;


    Transition( const std::string tmpl_before_str
              , const std::string tmpl_refactoring_str
              , const std::string tname = ""
    );

    std::string transform( const std::string str
                         , Environment::Stack* stack = new Environment::Stack()
    );

    std::string transformTo( Stack<Variable>* stack, const cacao::Template* tmpl2 );

    static Transition* fromFile( const Path path );
};




} // namespace cacao

