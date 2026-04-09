// Copyright (C) 2026 The OnePointer Authors.
//

#pragma once

#include <string>
#include <utility>

#include "cacao_regex.hpp"

#include <difflib.h>


namespace cacao {


class CacaoDiff
    :   protected difflib::chunk_list_t
    ,   protected std::pair< std::string_view, std::string_view >
{
protected:
    static difflib::chunk_list_t& getdiff( const std::string s1
                                         , const std::string s2 );

public:
    CacaoDiff( const std::string s1, const std::string s2 );
    CacaoDiff( const std::string s1, const RegexCacao::Match& sm2 );
    CacaoDiff( const RegexCacao::Match& sm1, const RegexCacao::Match& sm2 );

    static const std::string& patchWith( const std::string str_original
                                       , const std::string str_update
                                       , const difflib::chunk_t diff
    );
    const std::string& patch( const std::string str_original ) const;
};


} // namespace cacao

