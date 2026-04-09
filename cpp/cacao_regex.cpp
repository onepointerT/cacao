// Copyright (C) 2026 The OnePointer Authors.
//

#include "cacao_regex.hpp"

#include <cstring>
#include <ranges>

namespace cacao {


std::regex_constants::syntax_option_type toRegexConstants( const RegexType rtype ) {
    std::regex_constants::syntax_option_type syntxotype = std::regex_constants::ECMAScript;
    if ( rtype == RegexType::StlRegex ) syntxotype = std::regex_constants::basic;
    else if ( rtype == RegexType::ICase ) syntxotype = std::regex_constants::icase;
    else if ( rtype == RegexType::PosixRegex ) syntxotype = std::regex_constants::egrep;
    else if ( rtype == RegexType::CacaoRegex ) syntxotype = std::regex_constants::extended;
    return syntxotype;
}

std::regex_constants::syntax_option_type toRegexConstants( const RegexTypeCacao rtype_cacao ) {
    return toRegexConstants( toRegexType(rtype_cacao) );
}

RegexType toRegexType( const RegexTypeCacao rtype_cacao ) {
    if ( rtype_cacao == RegexTypeCacao::Cacao ) return RegexType::Cacao;
    else if ( rtype_cacao == RegexTypeCacao::CacaoRegex ) return RegexType::CacaoRegex;
    else if ( rtype_cacao == RegexTypeCacao::CacaoECMA ) return RegexType::CacaoECMA;
    return RegexType::Cacao;
}


RegexCacao::RegexCacao( const std::string regex, const RegexType rtype )
    :   std::regex( regex, toRegexConstants(rtype) )
    ,   str( regex )
    ,   type( rtype )
    ,   matchDict( *new Match::Dict() )
{}


std::list<Environment::Variable*> const& RegexCacao::find_env_variables( const std::string mstr, const RegexTypeCacao rtc ) {
    std::list< Environment::Variable* >* vars = new std::list< Environment::Variable* >();

    RegexType rtype = RegexType::Cacao;
    if ( rtc == RegexTypeCacao::CacaoRegex ) rtype = RegexType::CacaoRegex;
    else if ( rtc == RegexTypeCacao::CacaoECMA ) rtype = RegexType::CacaoECMA;

    const Match::List const& lmatching_vars = regex_var->match( mstr );
    const Matching& matching_vars = { lmatching_vars
                        , RegexCacao::MatchingMap::calculate_prefix_suffix(mstr, lmatching_vars) };
    for ( typename Match::List::const_iterator mIt = matching_vars.first.cbegin()
        ; mIt != matching_vars.first.cend(); ++mIt
    ) {
        const Match& match_var = *mIt;

        Environment::Template* tmpl = nullptr;
        Environment::Template* action = nullptr;
        Environment::Template* formatter = nullptr;
        unsigned int listpos = 0;
        const Match::List& matching_braces = regex_braces->match( match_var.data() );
        for ( typename Match::List::const_iterator rbIt = matching_braces.cbegin()
            ; rbIt != matching_braces.cend() && listpos <= 2; ++rbIt, listpos++
        ) {
            // #TODO: Upgrade Template pointers for variable types.
            if ( listpos == 0 ) {
                const Match& tmpl_match = *rbIt;
                const std::string_view tmpl_content = tmpl_match.substr( 2, tmpl_match.size() - 4 );
                tmpl = Environment::Template::fromCode( tmpl_content.data() );
                tmpl->vars = *tmpl->parse();
            } else if ( listpos == 1 ) {
                const Match& action_match = *rbIt;
                const std::string_view action_content = action_match.substr( 2, action_match.size() - 4 );
                action = Environment::Template::fromCode( action_content.data() );
                action->vars = *action->parse();
            } else if ( listpos = 2 ) {
                const Match& formatter_match = *rbIt;
                const std::string_view formatter_content = formatter_match.substr( 2, formatter_match.size() - 4 );
                formatter = Environment::Template::fromCode( formatter_content.data() );
                formatter->vars = *formatter->parse();
            } else break;
        }

        Environment::Variable* var
                = new Environment::Variable( match_var.data(), tmpl, action, formatter );
        vars->push_front( var );
    }

    return *vars;
}

std::list<Environment::Variable*> const& RegexCacao::find_env_variables_store( const std::string mstr, const RegexTypeCacao rtc ) {
    
    std::list< Environment::Variable* >* lenv_vars_store
                = new std::list< Environment::Variable* >();

    const Match::List const& mlenv_vars_store = regex_var_store->match( mstr );

    for ( typename Match::List::const_iterator cIt = mlenv_vars_store.cbegin()
        ; cIt != mlenv_vars_store.cend(); ++cIt
    ) {
        Match::List::value_type m = *cIt;

        Match mvarname = regex_detect_varname->matchOnce( m.str() );
        
        const Match::List const& mlvarbraces = regex_detect_braces->match( m.str() );
        if ( mlvarbraces.size() < 1 || mlvarbraces.size() > 2 ) return *new std::list<Environment::Variable*>();
        unsigned int braceidx = 1;
        Environment::Template* tmpl = nullptr;
        Environment::Template* action = nullptr;
        for ( typename Match::List::const_iterator mbIt = mlvarbraces.cbegin()
            ; mbIt != mlvarbraces.cend(); ++mbIt, braceidx++
        ) {
            Match minbrace = regex_detect_inbraces->matchOnce( mbIt->str() );
            if ( braceidx == 1 ) {
                tmpl = new Environment::Template( mvarname.str() + "_tmpl", minbrace.str(), Environment::Template::Type::SelectionRegex );
            } else if ( braceidx == 2 ) {
                action = new Environment::Template( mvarname.str() + "_setter", minbrace.str(), Environment::Template::Type::ActionVariables );
            } else break;
        }

        Environment::Variable* var = new Environment::Variable( m.str(), tmpl, action );
        lenv_vars_store->push_front( var );
    }

    return *lenv_vars_store;
}

std::list<Environment::Variable*> const& RegexCacao::find_env_variables_read( const std::string mstr, const RegexTypeCacao rtc ) {
 
    std::list< Environment::Variable* >* lenv_vars_read
                    = new std::list< Environment::Variable* >();

    const Match::List const& mlenv_vars_read = regex_var_read->match( mstr );

    for ( typename Match::List::const_iterator cIt = mlenv_vars_read.cbegin()
        ; cIt != mlenv_vars_read.cend(); ++cIt
    ) {
        Match::List::value_type m = *cIt;

        Match mvarname = regex_detect_varname->matchOnce( m.str() );
        
        const Match::List const& mlvarbraces = regex_detect_braces->match( m.str() );
        if ( mlvarbraces.size() < 1 || mlvarbraces.size() > 3 ) return *new std::list<Environment::Variable*>();
        unsigned int braceidx = 1;
        Environment::Template* tmpl = nullptr;
        Environment::Template* action = nullptr;
        Environment::Template* format = nullptr;
        for ( typename Match::List::const_iterator mbIt = mlvarbraces.cbegin()
            ; mbIt != mlvarbraces.cend(); ++mbIt, braceidx++
        ) {
            Match minbrace = regex_detect_inbraces->matchOnce( mbIt->str() );
            if ( braceidx == 1 ) {
                tmpl = new Environment::Template( mvarname.str() + "_tmpl", minbrace.str(), Environment::Template::Type::SelectionRegex );
            } else if ( braceidx == 2 ) {
                action = new Environment::Template( mvarname.str() + "_setter", minbrace.str(), Environment::Template::Type::ActionVariables );
            } else if ( braceidx == 3 ) {
                format = new Environment::Template( mvarname.str() + "_format", minbrace.str(), Environment::Template::Type::Formatter );
            } else break;
        }

        Environment::Variable* var = new Environment::Variable( m.str(), tmpl, action, format );
        lenv_vars_read->push_front( var );
    }

    return *lenv_vars_read;
}

std::list<Environment::Variable*> const& RegexCacao::find_env_variables_set( const std::string mstr, const RegexTypeCacao rtc ) {

    std::list< Environment::Variable* >* lenv_vars_set
                    = new std::list< Environment::Variable* >();

    const Match::List const& mlenv_vars_set = regex_var_set->match( mstr );

    for ( typename Match::List::const_iterator cIt = mlenv_vars_set.cbegin()
        ; cIt != mlenv_vars_set.cend(); ++cIt
    ) {
        Match::List::value_type m = *cIt;

        Match mvarname = regex_detect_varname->matchOnce( m.str() );
        
        const Match::List const& mlvarbraces = regex_detect_braces->match( m.str() );
        if ( mlvarbraces.size() != 1 ) return *new std::list<Environment::Variable*>();
        Match minbrace = regex_detect_inbraces->matchOnce( mlvarbraces[0].str() );
        Environment::Template* format 
                = new Environment::Template( mvarname.str() + "_format", minbrace.str(), Environment::Template::Type::Formatter );

        Environment::Variable* var = new Environment::Variable( m.str(), nullptr, nullptr, format );
        lenv_vars_set->push_front( var );
    }

    return *lenv_vars_set;
}


RegexCacao::Match::Match( const std::string matching_str, const size_t pos )
    :   std::string_view( matching_str )
    ,   position( pos )
{}


RegexCacao::Match::Result& fit_in( const std::string regex1, const std::string regex2, std::regex_constants::syntax_option_type regex_type ) {
    std::basic_regex<char> regex( regex1, std::regex_constants::ECMAScript | std::regex::multiline );
    std::smatch* regex_results 
                = new std::smatch();
    std::regex_match( regex2, *regex_results, regex, std::regex_constants::match_default );
    return *regex_results;
}


bool RegexCacao::Match::is_matching( const std::string str_regex, const RegexType regex_type ) const {
    Result& result = fit_in( this->str(), str_regex );
    return ! result.empty() && result.ready();
}

RegexCacao::Match const& RegexCacao::Match::regex( const std::string str_regex, const RegexType regex_type ) const {
    Result& result = fit_in( *this, str_regex );
    return *new Match( result.str(), result.position() );
}


const RegexCacao::Match::List const& RegexCacao::Match::regexMatchAll( const std::string str_regex, const RegexType regex_type = RegexType::EcmaRegex ) const {
    return *new Match::List( Match::List::matchlist( str_regex, this->str(), regex_type ) );
}

std::string RegexCacao::Match::str() const {
    return this->data();
}

bool RegexCacao::Match::empty() const {
    return this->length() == 0;
}



RegexCacao::Match::List::List( const std::string regex, const std::string mstr, const RegexType rtype )
    :   std::list< Match >( matchlist(regex, mstr, rtype) )
    ,   rgx( regex )
    ,   str( mstr )
    ,   rtype( rtype )
    ,   it( this->begin() )
{}

RegexCacao::Match::List::List( const std::list< Match > const& other )
    :   std::list< Match >( other )
    ,   rgx( "" )
    ,   str( "" )
    ,   rtype( RegexType::ICase )
    ,   it( this->begin() )
{}

bool RegexCacao::Match::List::has_next() const {
    return this->it != --(this->end());
}

RegexCacao::Match RegexCacao::Match::List::next() {
    ++this->it;
    return *it;
}

std::list<RegexCacao::Match>& RegexCacao::Match::List::matchlist( const std::string regex, const std::string str, const RegexType rtype ) {
    
    std::regex_constants::syntax_option_type regex_type = std::regex_constants::ECMAScript;
    if ( rtype == RegexType::StlRegex ) regex_type = std::regex_constants::basic;
    else if ( rtype == RegexType::PosixRegex ) regex_type = std::regex_constants::egrep;
    else if ( rtype == RegexType::CacaoRegex ) regex_type = std::regex_constants::extended;
    else if ( rtype == RegexType::ICase ) regex_type = std::regex_constants::icase;

    std::regex_constants::match_flag_type mft = std::regex_constants::match_continuous;

    std::smatch* matches = new std::smatch();
    std::regex* rgx = new std::regex(regex, regex_type);
    std::regex_search( str, *matches, *rgx, mft );

    std::list<RegexCacao::Match>* rlist = new std::list<RegexCacao::Match>();
    for ( unsigned int m = 0; m < matches->size(); m++ ) {
        std::smatch sm = matches[m];

        RegexCacao::Match* rgx_match = new Match(sm.str(), sm.position());
        rlist->push_back( *rgx_match );
    }

    return *rlist;
}


RegexCacao::Match::List::operator std::basic_string<char>() const {
    return this->str;
}


RegexCacao::Match::List::operator std::basic_regex<char>() const {
    return std::basic_regex<char>( this->rgx, toRegexConstants(this->rtype) );
}

RegexCacao::Match::List::operator RegexType() const {
    return this->rtype;
}


const RegexCacao::Match::List const& RegexCacao::match( const std::string mstr ) const {
    Match* m = new Match( mstr, 0 );
    return m->regexMatchAll(this->str.data(), this->type);
}


const RegexCacao::Match::Dict const& RegexCacao::matchAll( const std::initializer_list<std::string> il_mstr ) const {
    Match::Dict* dct = new Match::Dict();
    for ( const std::string mstr : il_mstr ) {
        const Match::List& m = this->match( mstr );
        if ( m.size() > 0 ) (*dct)[mstr] = m;
    }

    return *dct;
}

const RegexCacao::Match const& RegexCacao::matchOnce( const std::string mstr ) const {
    return (new Match(mstr, 0))->regex( this->str.data(), this->type );
}

 
RegexCacao::operator std::basic_regex<char>() const {
    return *this;
}

RegexCacao::operator std::basic_string<char>() const {
    return this->str.data();
}

RegexCacao::operator RegexType() const {
    return this->type;
}


RegexCacao::MatchingMap::MatchingMap( const std::string str_or_regex, const Type mtype )
    :   std::map< std::string, MatchingDict >()
    ,   std::string_view( str_or_regex )
    ,   type( mtype )
{}


RegexCacao::MatchingMap::mapped_type& RegexCacao::MatchingMap::operator[]( const key_type key ) {
    return map_type::operator[]( key );
}

const RegexCacao::MatchingMap::mapped_type& RegexCacao::MatchingMap::operator[]( const key_type key ) const {
    return map_type::at(key);
}

RegexCacao::MatchingMap::iterator RegexCacao::MatchingMap::insert_or_assign_range( const_iterator start, const_iterator end ) {
    if constexpr ( std::is_same_object_iterator(start, end) ) {
        for ( typename map_type::const_iterator cIt = start; cIt != end; ++cIt ) {
            this->insert_or_assign( cIt->first, cIt->second );
        }
    } else {
        std::ranges::subrange mmr{ start, end };
        std::views::as_rvalue* mmv = new std::views::as_rvalue( mmr );
        for ( auto it : std::ranges::subrange{ mmv->begin(), mmv->end() } ) {
            this->insert_or_assign( it->first, it->second );
        }
    }
    for ( typename map_type::iterator it : *this ) {
        if ( *it == *start ) return it;
    }
    return map_type::end();
}

RegexCacao::MatchingMap::operator std::basic_string<char>() const {
    return this->substr().data();
}


RegexCacao::MatchingMap::ResultList::ResultList(  const std::string regex, const std::string rstr
                                                , const RegexType regex_type )
    :   std::list< MatchingDict const& >()
    ,   rgx( regex )
    ,   str( rstr )
    ,   rtype( regex_type )
{}



RegexCacao::MatchingMap::ResultMap::ResultMap( const std::string regex, const std::string rstr
                                             , const RegexType regex_type )
    :   std::map< std::string, Matching const& >()
    ,   rgx( regex )
    ,   str( rstr )
    ,   rtype( regex_type )
{}


const RegexCacao::MatchingMap::ResultList& RegexCacao::MatchingMap::search_by_prefix( const std::string prefix ) const {

    ResultList* rl = this->makeResultList( prefix );
    for ( typename map_type::const_iterator cIt : std::ranges::subrange{ map_type::begin(), map_type::end() } ) {
        PrefixSuffix presuf = cIt->second->second;
        if ( presuf.first.compare(prefix) == 0 ) return rl->emplace_front( cIt->second );
    }

    return *rl;
}

const RegexCacao::MatchingMap::ResultList& RegexCacao::MatchingMap::search_by_suffix( const std::string suffix ) const {
    
    ResultList* rl = this->makeResultList( suffix );
    for ( typename map_type::const_iterator cIt : std::ranges::subrange{ map_type::begin(), map_type::end() } ) {
        PrefixSuffix presuf = cIt->second->second;
        if ( presuf.second.compare(suffix) == 0 ) return rl->emplace_front( cIt->second );
    }

    return *rl;
}

const RegexCacao::MatchingMap::ResultList& RegexCacao::MatchingMap::search_by_prefixsuffix( const PrefixSuffix presuf_strs ) const {
    
    ResultList* rl = this->makeResultList( presuf_strs.first.data() + presuf_strs.second.data() );
    for ( typename map_type::const_iterator cIt : std::ranges::subrange{ map_type::begin(), map_type::end() } ) {
        PrefixSuffix presuf = cIt->second->second;
        if ( presuf.first.compare(presuf_strs.first) == 0
          && presuf.second.compare(presuf_strs.second) == 0
        ) return rl->emplace_front( cIt->second );
    }

    return *rl;
}

const RegexCacao::MatchingMap::ResultMap& RegexCacao::MatchingMap::search_by_dictKey( const std::string key ) const {

    ResultMap* rm = this->makeResultMap( *this, this->rtype );

    for ( typename map_type::const_iterator cIt : std::ranges::subrange{ map_type::begin(), map_type::end() } ) {
        for ( typename Match::Dict::const_iterator dIt : std::ranges::subrange{ cIt->second.first.cbegin(), cIt->second.first.cend() } ) {
            if ( dIt->first == key ) 
                Match::List* ml = &dIt->second;

                PrefixSuffix& presuf = this->calculate_prefix_suffix(
                                                ml->str, *ml, ml->rgx
                );

                (*rm)[key] = *new Matching{ *ml, presuf };
        }
    }

    return *rm;
}

const RegexCacao::MatchingMap::ResultMap& RegexCacao::MatchingMap::search_by_dictKeys(
                                                                               const std::initializer_list< std::string > keys
) const {

    ResultMap* rm = this->makeResultMap( *this, this->type );

    for ( const std::string k : keys ) {
        const ResultMap& k_result = this->search_by_dictKey( k );

        rm->insert_range( std::range::subrange{ k_result.cbegin(), k_result.cend() } );
    }

    return *rm;
}


const RegexCacao::MatchingMap::ResultMap& RegexCacao::MatchingMap::search_by_regex( const std::string regex ) const {

    ResultMap* rm = this->makeResultMap( *this, this->type );

    for ( typename const_iterator cIt : std::ranges::subrange{ map_type::cbegin(), map_type::cend() } ) {
        if ( this->type == Type::Regex ) {
            if ( cIt->first == regex ) {
                Match::List* ml = new Match::List( regex, *this, *this );
                
                const MatchingDict const& md = cIt->second;
                for ( typename Match::Dict::const_iterator mdIt : md ) {
                    ml->insert( *mdIt );
                }

                PrefixSuffix presuf = this->calculate_prefix_suffix( *this, *ml );
                rm->emplace( { regex, { *ml, presuf } } );
            }
        } else if ( this->type == Type::Str ) {
            Match* m = new Match( cIt->first, *this );
            if ( m->is_matching( regex, this->type ) ) {
                Match::List* ml = new Match::List( cIt->first, *this, *this );

                PrefixSuffix presuf = this->calculate_prefix_suffix( cIt->first, *ml );
                rm->emplace( { regex, { *ml, presuf } } );
            }
        }
    }
    
    return *rm;
}

const RegexCacao::MatchingMap::ResultMap& RegexCacao::MatchingMap::search_by_str( const std::string rstr ) const {

    ResultMap* rm = this->makeResultMap( *this, this->type );

    for ( typename const_iterator cIt : std::ranges::subrange{ map_type::cbegin(), map_type::cend() } ) {
        if ( this->type == Type::Str ) {
            if ( cIt->first == rstr ) {
                Match::List* ml = new Match::List( rstr, *this, *this );
                
                const MatchingDict const& md = cIt->second;
                for ( typename Match::Dict::const_iterator mdIt : md ) { // #TODO
                    ml->insert( *mdIt ); // #TODO
                }

                const PrefixSuffix& presuf = this->calculate_prefix_suffix( *this, *ml );
                rm->emplace( { rstr, { *ml, presuf } } );
            }
        } else if ( this->type == Type::Regex ) {
            Match* m = new Match( cIt->first, *this );
            if ( m->is_matching( rstr, this->type ) ) {
                Match::List* ml = new Match::List( cIt->first, *this, *this );

                const PrefixSuffix& presuf = this->calculate_prefix_suffix( cIt->first, *ml );
                rm->emplace( { rstr, { *ml, presuf } } );
            }
        }
    }
    
    return *rm;
}

const RegexCacao::PrefixSuffix& RegexCacao::MatchingMap::calculate_prefix_suffix( const std::string mstr
                                            , const std::string matching_str
                                            , const size_t str_pos
                                            , const size_t str_end
) {
    size_t pos_mstart = mstr.find_first_of( matching_str, str_pos, str_end - str_pos );
    if ( pos_mstart != mstr.npos && matching_str.length() + 1 + pos_mstart < mstr.length() - 1 ) {
        size_t pos_suffix_start = pos_mstart + matching_str.length() + 1;
        return { str.substr( 0, pos_mstart ), mstr.substr( pos_suffix_start ) };
    } else if ( pos_mstart != mstr.npos ) return { mstr.substr(0, pos_mstart), "" };
    return { "", "" };
}

const RegexCacao::PrefixSuffix& RegexCacao::MatchingMap::calculate_prefix_suffix( const std::string mstr
                                            , const Match::List const& matching_list
                                            , const std::string regex
                                            , const size_t str_pos
                                            , const size_t str_end
) {
    size_t pos_smallest = str_pos;
    size_t pos_greatest = str_end == 0 ? mstr.length() - 1 : str_end;

    for ( typename Matching::List::const_iterator mIt : std::ranges::contiguous_range{ matching_list.cbegin(), matching_list.cend() }) {
        const Match const& m = *mIt;
        size_t pos_start = mstr.find_first_of( m.str() );
        if ( pos_start != mstr.npos ) {
            size_t pos_end = pos_start + m.length();
            if ( pos_smallest > pos_start ) pos_smallest = pos_start;
            if ( pos_greatest < pos_end ) pos_greatest = pos_end;
        }
    }

    return { pos_smallest > 0 ? mstr.substr( 0, pos_smallest - 1 ) : mstr.substr(0, 1)
        , pos_greatest < mstr.length() - 2 ? mstr.substr( pos_greatest + 1 ) : "" };
}

RegexCacao::MatchingMap const& RegexCacao::MatchingMap::getMatchingMap( const std::string mstr
                                        , const std::string regex
                                        , const RegexType rtype
                                        , const bool matchingMapIsRegex
                                        , const bool matchAllPositions
                                        , const size_t matchingPositionStart
                                        , const size_t matchingPositionEnd
) {
    const std::string str_or_regex = matchingMapIsRegex ? regex : mstr;
    const MatchingMap::Type mtype = matchingMapIsRegex ? MatchingMap::Type::Regex
                                                        : MatchingMap::Type::Str;
    
    MatchingMap* mm = new MatchingMap( str_or_regex, mtype, rtype );
    
    RegexCacao* rc = new RegexCacao( regex, rtype );
    const Match::List const& ml = rc->match( mstr );
    Match::Dict* md = new Match::Dict();
    if ( mtype == MatchingMap::Type::Regex ) {
        (*md)[mstr] = *ml; 
        (*mm)[regex] = { *md, MatchingMap::calculate_prefix_suffix(mstr, *ml) };
    } else if ( mtype == MatchingMap::Type::Str ) {
        (*md)[regex] = *ml; 
        (*mm)[mstr] = { *md, MatchingMap::calculate_prefix_suffix(mstr, *ml) };
    }

    return *mm;
}


RegexCacao::MatchingMap::ResultList* RegexCacao::MatchingMap::makeResultList( const std::string str_or_regex, const RegexType rtype ) const {
    if ( this->type == Type::Regex ) return new ResultList( *this, str_or_regex, rtype );
    else if ( this->type == Type::Str ) return new ResultList( str_or_regex, *this, rtype );
    return new ResultList( "", "", rtype );
}


RegexCacao::MatchingMap::ResultMap* RegexCacao::MatchingMap::makeResultMap( const std::string str_or_regex, const RegexType rtype ) const {
    if ( this->type == Type::Regex ) return new ResultMap( *this, str_or_regex, rtype );
    else if ( this->type == Type::Str ) return new ResultMap( str_or_regex, *this, rtype );
    return new ResultMap( "", "", rtype );
}


RegexCacao::MatchingMap const& RegexCacao::getMatching( const std::string mstr, const std::string regex
                                , const bool matchAllPositions
                                , const size_t matchingPositionStart
                                , const size_t matchingPositionEnd ) const {
    return MatchingMap::getMatchingMap( mstr, regex, this->type, false, matchAllPositions
                                , matchingPositionStart, matchingPositionEnd );
}

RegexCacao::MatchingMap const& RegexCacao::getMatching( const std::string mstr
                            , const bool matchingMapIsRegex
                            , const bool matchAllPositions
                            , const size_t matchingPositionStart
                            , const size_t matchingPositionEnd ) const {
    return MatchingMap::getMatchingMap( mstr, this->str, this->type
                                , matchingMapIsRegex, matchAllPositions
                                , matchingPositionStart, matchingPositionEnd );
}

RegexCacao::MatchingMap const& RegexCacao::getMatching( const PrefixSuffix prefixsuffix
                            , const std::string mstr
                            , const bool matchingMapIsRegex
                            , const bool matchAllPositionsBetween
                            , const size_t matchingPositionStart
                            , const size_t matchingPositionEnd ) const {
    size_t pos_prefix_end = mstr.find_first_of( prefixsuffix.first );
    if ( pos_prefix_end != mstr.npos ) pos_prefix_end 
                                    += prefixsuffix.first.length() + 1;
    size_t pos_suffix_start = mstr.find_first_of( prefixsuffix.second );
    if ( pos_suffix_start != mstr.npos ) pos_suffix_start -= 1;

    std::string sstr;
    if ( pos_prefix_end == mstr.npos && pos_suffix_start == mstr.npos )
        sstr = mstr;
    else if ( pos_prefix_end != mstr.npos && pos_suffix_start == mstr.npos )
        sstr = mstr.substr( pos_prefix_end );
    else if ( pos_prefix_end == mstr.npos && pos_suffix_start != mstr.npos )
        sstr = mstr.substr( 0, pos_suffix_start );
    else sstr = mstr.substr( pos_prefix_end, pos_suffix_start );

    return MatchingMap::getMatchingMap( sstr, this->str.data(), this->type
                                , matchingMapIsRegex, matchAllPositionsBetween
                                , matchingPositionStart, matchingPositionEnd
    );
}

} // namespace cacao
