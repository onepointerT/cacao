
#pragma once

#include <string>

#include "cacao_fs.hpp"
#include "cacao_vte.hpp"


namespace cacao {



class Transition {
    class Equivalence {
    public:
        Environment::Template* tmpl_before = nullptr;
        Environment::Template* tmpl_refactoring = nullptr;

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

    static Transition* fromFile( const Path path );
};




} // namespace cacao

