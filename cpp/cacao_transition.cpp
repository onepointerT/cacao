
#include "cacao_transition.hpp"

#include "cacao_str.hpp"

namespace cacao {


Transition::Equivalence::Equivalence( const std::string tmpl_before_str
                                    , const std::string tmpl_refactoring_str
)   :   tmpl_before( new Environment::Template( "treq_before", tmpl_before_str ) )
    ,   tmpl_refactoring( new Environment::Template( "treq_refactoring", tmpl_refactoring_str ) )
{}

std::string Transition::Equivalence::transformation( Environment::Stack* stack ) {
    return tmpl_before->transformTo( stack, tmpl_refactoring );
}



Transition::Transition( const std::string tmpl_before_str
              , const std::string tmpl_refactoring_str
              , const std::string tname
)   :   name( tname )
    ,   equiv( new Equivalence( tmpl_before_str, tmpl_refactoring_str ) )
{}

std::string Transition::transform( const std::string str
                         , Environment::Stack* stack
) {
    Environment::Template* transformation_tmpl
            = new Environment::Template( "trf_" + name, equiv->transformation( stack ) );
    return transformation_tmpl->transform( str, stack );
}

Transition* Transition::fromFile( const Path path ) {
    File* fd = new File( path.data() );
    if ( fd->exists() ) {
        return nullptr;
    }
    const std::string fc = fd->read().data();

    const int pos_equiv_1_start = 5 + Str::find( fc, "----\n" );
    const int pos_equiv_2_start = 5 + Str::find( fc, "----\n" );
    if ( pos_equiv_1_start == -1 || pos_equiv_2_start == -1 ) {
        return nullptr;
    }

    std::string tname;
    if ( pos_equiv_1_start > 0 ) {
        const int pos_tmpl_name = Str::find( fc, ">>", 0, pos_equiv_1_start );
        if ( pos_tmpl_name > -1 ) {
            const int pos_name_linebreak = Str::find( fc, "\n", pos_tmpl_name );
            tname = fc.substr(pos_tmpl_name, pos_name_linebreak-pos_tmpl_name);
        } else {
            tname = genid();
        }
    } else {
        tname = genid();
    }

    return new Transition(
          fc.substr( pos_equiv_1_start + 5, pos_equiv_2_start - pos_equiv_1_start + 5 )
        , fc.substr( pos_equiv_2_start + 5 ), tname
    );
}


} // namespace cacao

