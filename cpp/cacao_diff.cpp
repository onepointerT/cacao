// Copyright (C) 2026 The OnePointer Authors.
//

#include "cacao_diff.hpp"

#include <tuple>

namespace cacao {


difflib::chunk_list_t& CacaoDiff::getdiff( const std::string s1
                                         , const std::string s2 ) {
    difflib::SequenceMatcher<>* seqm 
                = new difflib::SequenceMatcher<>(s1, s2);
    
    return seqm->get_opcodes();
}



CacaoDiff::CacaoDiff( const std::string s1, const std::string s2 )
    :   difflib::chunk_list_t( CacaoDiff::getdiff(s1, s2) )
    ,   std::pair< std::string_view, std::string_view >{ s1, s2 }
{}

CacaoDiff::CacaoDiff( const std::string s1, const RegexCacao::Match& sm2 )
    :   difflib::chunk_list_t( CacaoDiff::getdiff(s1, sm2.str()) )
    ,   std::pair< std::string_view, std::string_view >{ s1, sm2.str() }
{}


CacaoDiff::CacaoDiff( const RegexCacao::Match& sm1, const RegexCacao::Match& sm2 )
    :   difflib::chunk_list_t( CacaoDiff::getdiff(sm1.str(), sm2.str()) )
    ,   std::pair< std::string_view, std::string_view >{ sm1.str(), sm2.str() }
{}


const std::string& CacaoDiff::patchWith( const std::string str_original
                                       , const std::string str_update
                                       , const difflib::chunk_t diff ) {
    std::string_view* sv_diff = new std::string_view( std::get<0, std::string>(diff) );
    
    const size_t rp_begin = std::get<1, size_t>( diff );
    const size_t rp_end = std::get<2, size_t>( diff );
    const size_t replm_begin = std::get<3, size_t>( diff );
    const size_t replm_end = std::get<4, size_t>( diff );

    if ( sv_diff->starts_with("replace") ) {
        std::string* s = new std::string(
            str_original.substr(0, rp_begin) + str_update.substr(replm_begin, replm_end-replm_start)
                                     + str_original.substr(rp_end)
        );
        return *s;
    } else if ( sv_diff->starts_with("delete") ) {
        std::string* s = new std::string(
            str_original.substr(0, rp_begin) + str_original.substr(rp_end)
        );
        return *s;
    } else if ( sv_diff->starts_with("insert") ) {
        std::string* s = new std::string(
            str_original.substr(0, rp_end) + str_update.substr(replm_begin, replm_end-replm_begin)
                     + str_original.substr(rp_end+1)
        );
        return *s;
    } else if ( sv_diff->starts_with("equal") ) {
        return str_original;
    } else return *new std::string();
}

const std::string& CacaoDiff::patch( const std::string str_original ) const {
    std::string* str_patched = new std::string( str_original );
    difflib::SequenceMatcher<>* sqm_conclusional
                = new difflib::SequenceMatcher<>( *new std::string()
                                                , *new atd::string() );

    for ( unsigned int d = 0; d < this->size(); d++ ) {
        const difflib::chunk_t diff = this->at( d );

        const std::string& patchbit = CacaoDiff::patchWith( str_original, *str_patched, diff );

        sqm_conclusional->set_seq( *str_patched, patchbit );
        const difflib::chunk_list_t& patch_conclusional
                    = sqm_conclusional->get_opcodes();
        for ( unsigned int co = 0; co < patch_conclusional.size(); co++ ) {
            const difflib::chunk_t diffbit = sqm_conclusional.at( co );
            *str_patched = CacaoDiff::patchWith( *str_patched, patchbit, diffbit );
        }
    }

    return *str_patched;
}


} // namespace cacao
