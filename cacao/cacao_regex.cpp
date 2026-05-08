// Copyright (C) 2026 The OnePointer Authors.
//

#include "cacao_regex.hpp"

#include <cstdint>
#include <cstring>
#include <ranges>

#include "cacao_tmpl.hpp"
#include "cacao_vars.hpp"


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
    if ( rtype_cacao == RegexTypeCacao::CacaoType ) return RegexType::Cacao;
    else if ( rtype_cacao == RegexTypeCacao::CacaoICase ) return RegexType::CacaoRegex;
    else if ( rtype_cacao == RegexTypeCacao::ECMA ) return RegexType::CacaoECMA;
    return RegexType::Cacao;
}


RegexCacao::RegexCacao( const std::string regex, const RegexType rtype )
    :   std::regex( regex, toRegexConstants(rtype) )
    ,   str( regex )
    ,   type( rtype )
    ,   matchDict( *new Match::Dict() )
{}


std::list<Variable*>& RegexCacao::find_env_variables( const std::string mstr, const RegexTypeCacao rtc ) {
    std::list< Variable* >* vars = new std::list< Variable* >();

    RegexType rtype = cacao::toRegexType( rtc );

    const Match::List& lmatching_vars = regex_var->match( mstr );
    const Matching& matching_vars = { lmatching_vars
                        , RegexCacao::MatchingMap::calculate_prefix_suffix(mstr, lmatching_vars) };
    for ( typename Match::List::const_iterator mIt = matching_vars.first.cbegin()
        ; mIt != matching_vars.first.cend(); ++mIt
    ) {
        const Match* match_var = *mIt;

        Template* tmpl = nullptr;
        Template* action = nullptr;
        Template* formatter = nullptr;
        unsigned int listpos = 0;
        const Match::List& matching_braces = regex_braces->match( match_var->data() );
        for ( typename Match::List::const_iterator rbIt = matching_braces.cbegin()
            ; rbIt != matching_braces.cend() && listpos <= 2; ++rbIt, listpos++
        ) {
            // #TODO: Upgrade Template pointers for variable types.
            if ( listpos == 0 ) {
                const Match* tmpl_match = *rbIt;
                const std::string_view tmpl_content = tmpl_match->substr( 2, tmpl_match->size() - 4 );
                tmpl = Template::fromCode( tmpl_content.data() );
                tmpl->vars = *tmpl->parse();
            } else if ( listpos == 1 ) {
                const Match* action_match = *rbIt;
                const std::string_view action_content = action_match->substr( 2, action_match->size() - 4 );
                action = Template::fromCode( action_content.data() );
                action->vars = *action->parse();
            } else if ( listpos = 2 ) {
                const Match* formatter_match = *rbIt;
                const std::string_view formatter_content = formatter_match->substr( 2, formatter_match->size() - 4 );
                formatter = Template::fromCode( formatter_content.data() );
                formatter->vars = *formatter->parse();
            } else break;
        }

        Variable* var
                = new Variable( match_var.data(), tmpl, action, formatter );
        vars->push_front( var );
    }

    return *vars;
}

std::list<Variable*>& RegexCacao::find_env_variables_store( const std::string mstr, const RegexTypeCacao rtc ) {
    
    std::list< Variable* >* lenv_vars_store
                = new std::list< Variable* >();

    const Match::List& mlenv_vars_store = regex_var_store->match( mstr );

    for ( typename Match::List::const_iterator cIt = mlenv_vars_store.cbegin()
        ; cIt != mlenv_vars_store.cend(); ++cIt
    ) {
        Match::List::value_type m = *cIt;

        Match& mvarname = regex_detect_varname->matchOnce( m->str() );
        
        const Match::List& mlvarbraces = regex_detect_braces->match( m->str() );
        if ( mlvarbraces.size() < 1 || mlvarbraces.size() > 2 ) return *new std::list<Variable*>();
        unsigned int braceidx = 1;
        Template* tmpl = nullptr;
        Template* action = nullptr;
        for ( typename Match::List::const_iterator mbIt = mlvarbraces.cbegin()
            ; mbIt != mlvarbraces.cend(); ++mbIt, braceidx++
        ) {
            Match& minbrace = regex_detect_inbraces->matchOnce( mbIt->str() );
            if ( braceidx == 1 ) {
                tmpl = new Template( mvarname.str() + "_tmpl", minbrace.str(), Template::Type::SelectionRegex );
            } else if ( braceidx == 2 ) {
                action = new Template( mvarname.str() + "_setter", minbrace.str(), Template::Type::ActionVariables );
            } else break;
        }

        Variable* var = new Variable( m.str(), tmpl, action );
        lenv_vars_store->push_front( var );
    }

    return *lenv_vars_store;
}

std::list<Variable*>& RegexCacao::find_env_variables_read( const std::string mstr, const RegexTypeCacao rtc ) {
 
    std::list< Variable* >* lenv_vars_read
                    = new std::list< Variable* >();

    const Match::List& mlenv_vars_read = regex_var_read->match( mstr );

    for ( typename Match::List::const_iterator cIt = mlenv_vars_read.cbegin()
        ; cIt != mlenv_vars_read.cend(); ++cIt
    ) {
        Match::List::value_type m = *cIt;

        Match& mvarname = regex_detect_varname->matchOnce( m->str() );
        
        const Match::List& mlvarbraces = regex_detect_braces->match( m->str() );
        if ( mlvarbraces.size() < 1 || mlvarbraces.size() > 3 ) return *new std::list<Variable*>();
        unsigned int braceidx = 1;
        Template* tmpl = nullptr;
        Template* action = nullptr;
        Template* format = nullptr;
        for ( typename Match::List::const_iterator mbIt = mlvarbraces.cbegin()
            ; mbIt != mlvarbraces.cend(); ++mbIt, braceidx++
        ) {
            Match& minbrace = regex_detect_inbraces->matchOnce( mbIt->str() );
            if ( braceidx == 1 ) {
                tmpl = new Template( mvarname.str() + "_tmpl", minbrace.str(), Template::Type::SelectionRegex );
            } else if ( braceidx == 2 ) {
                action = new Template( mvarname.str() + "_setter", minbrace.str(), Template::Type::ActionVariables );
            } else if ( braceidx == 3 ) {
                format = new Template( mvarname.str() + "_format", minbrace.str(), Template::Type::Formatter );
            } else break;
        }

        Variable* var = new Variable( m->str(), tmpl, action, format );
        lenv_vars_read->push_front( var );
    }

    return *lenv_vars_read;
}

std::list<Variable*>& RegexCacao::find_env_variables_set( const std::string mstr, const RegexTypeCacao rtc ) {

    std::list< Variable* >* lenv_vars_set
                    = new std::list< Variable* >();

    const Match::List& mlenv_vars_set = regex_var_set->match( mstr );

    for ( typename Match::List::const_iterator cIt = mlenv_vars_set.cbegin()
        ; cIt != mlenv_vars_set.cend(); ++cIt
    ) {
        Match::List::value_type m = *cIt;

        Match& mvarname = regex_detect_varname->matchOnce( m->str() );
        
        const Match::List& mlvarbraces = regex_detect_braces->match( m->str() );
        if ( mlvarbraces.size() != 1 ) return *new std::list<Variable*>();
        Match& minbrace = regex_detect_inbraces->matchOnce( mlvarbraces[0]->str() );
        Template* format 
                = new Template( mvarname.str() + "_format", minbrace.str(), Template::Type::Formatter );

        Variable* var = new Variable( m.str(), nullptr, nullptr, format );
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

RegexCacao::Match& RegexCacao::Match::regex( const std::string str_regex, const RegexType regex_type ) const {
    Result& result = fit_in( *this, str_regex );
    return *new Match( result.str(), result.position() );
}


const RegexCacao::Match::List& RegexCacao::Match::regexMatchAll( const std::string str_regex, const RegexType regex_type = RegexType::EcmaRegex ) const {
    return *new Match::List( Match::List::matchlist( str_regex, this->str(), regex_type ) );
}

std::string RegexCacao::Match::str() const {
    return this->data();
}

bool RegexCacao::Match::empty() const {
    return this->length() == 0;
}



RegexCacao::Match::List::List( const std::string regex, const std::string mstr, const RegexType rtype )
    :   std::list< Match* >( Match::List::matchlist(regex, mstr, rtype) )
    ,   rgx( regex )
    ,   str( mstr )
    ,   rtype( rtype )
    ,   it( this->begin() )
{}

RegexCacao::Match::List::List( const std::list< Match* >& other )
    :   std::list< Match* >( other )
    ,   rgx( "" )
    ,   str( "" )
    ,   rtype( RegexType::ICase )
    ,   it( this->begin() )
{}

bool RegexCacao::Match::List::has_next() const {
    return this->it != --(this->end());
}

RegexCacao::Match& RegexCacao::Match::List::next() {
    ++this->it;
    return *(*it);
}

std::list<RegexCacao::Match*>& RegexCacao::Match::List::matchlist( const std::string regex, const std::string str, const RegexType rtype ) {
    
    std::regex_constants::syntax_option_type regex_type = cacao::toRegexConstants( rtype );
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


const RegexCacao::Match::List& RegexCacao::match( const std::string mstr ) const {
    Match* m = new Match( mstr, 0 );
    return m->regexMatchAll(this->str.data(), this->type);
}


const RegexCacao::Match::Dict& RegexCacao::matchAll( const std::initializer_list<std::string> il_mstr ) const {
    Match::Dict* dct = new Match::Dict();
    for ( const std::string mstr : il_mstr ) {
        const Match::List& m = this->match( mstr );
        if ( m.size() > 0 ) (*dct)[mstr] = *new Match::List(m);
    }

    return *dct;
}

const RegexCacao::Match& RegexCacao::matchOnce( const std::string mstr ) const {
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
    :   std::map< std::string, MatchingDict* >()
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
    iterator it = map_type::begin();
    for ( typename map_type::const_iterator cIt = start; cIt != end; ++cIt ) {
        if ( cIt != start )
            this->insert_or_assign( cIt->first, cIt->second );
        else it = this->insert_or_assign( cIt->first, cIt->second ).first;
    }
    return it;
}

RegexCacao::MatchingMap::operator std::basic_string<char>() const {
    return this->substr().data();
}


RegexCacao::MatchingMap::ResultList::ResultList(  const std::string regex, const std::string rstr
                                                , const RegexType regex_type )
    :   std::list< MatchingDict* >()
    ,   rgx( regex )
    ,   str( rstr )
    ,   rtype( regex_type )
{}



RegexCacao::MatchingMap::ResultMap::ResultMap( const std::string regex, const std::string rstr
                                             , const RegexType regex_type )
    :   std::multimap< std::string, Matching* >()
    ,   rgx( regex )
    ,   str( rstr )
    ,   rtype( regex_type )
{}


const RegexCacao::MatchingMap::ResultList& RegexCacao::MatchingMap::search_by_prefix( const std::string prefix ) const {

    ResultList* rl = new ResultList( this->data(), prefix, this->rtype );
    for ( typename map_type::const_iterator cIt : { map_type::cbegin(), map_type::cend() } ) {
        PrefixSuffix presuf = cIt->second.second;
        if ( presuf.first.compare(prefix) == 0 )
            rl->emplace_front( cIt->second );
    }

    return *rl;
}

const RegexCacao::MatchingMap::ResultList& RegexCacao::MatchingMap::search_by_suffix( const std::string suffix ) const {
    
    ResultList* rl = new ResultList( this->data(), suffix, this->rtype );
    for ( typename map_type::const_iterator cIt : { map_type::begin(), map_type::end() } ) {
        PrefixSuffix presuf = cIt->second.second;
        if ( presuf.second.compare(suffix) == 0 ) 
            rl->emplace_front( cIt->second );
    }

    return *rl;
}

const RegexCacao::MatchingMap::ResultList& RegexCacao::MatchingMap::search_by_prefixsuffix( const PrefixSuffix presuf_strs ) const {
    
    std::string psstr = presuf_strs.first.data(); psstr += "\n[...]\n"; psstr += presuf_strs.second.data();
    ResultList* rl = new ResultList( this->data(), psstr, this->rtype );
    for ( typename map_type::const_iterator cIt : { map_type::begin(), map_type::end() } ) {
        PrefixSuffix presuf = cIt->second.second;
        if ( presuf.first.compare(presuf_strs.first) == 0
          && presuf.second.compare(presuf_strs.second) == 0
        ) rl->emplace_front( cIt->second );
    }

    return *rl;
}

const RegexCacao::MatchingMap::ResultMap& RegexCacao::MatchingMap::search_by_dictKey( const std::string key ) const {

    ResultMap* rm = new ResultMap( this->data(), "$KEY", this->rtype );

    for ( typename map_type::const_iterator cIt : { map_type::begin(), map_type::end() } ) {
        for ( typename Match::Dict::const_iterator dIt : { cIt->second.first.cbegin(), cIt->second.first.cend() } ) {
            if ( dIt->first == key ) 
                Match::List& ml = *dIt->second;

                PrefixSuffix& presuf = this->calculate_prefix_suffix( ml.str, ml, ml.rgx );

                rm->insert( { key
                             , *new Matching{ &ml, presuf }
                        } );      
        }
    }

    return *rm;
}

const RegexCacao::MatchingMap::ResultMap& RegexCacao::MatchingMap::search_by_dictKeys(
                                                                               const std::initializer_list< std::string > keys
) const {

    ResultMap* rm = new ResultMap( this->data(), "$KEYS", this->rtype );

    for ( const std::string k : keys ) {
        const ResultMap& k_result = this->search_by_dictKey( k );

        rm->insert_range( std::ranges::subrange{ k_result.cbegin(), k_result.cend() } );
    }

    return *rm;
}


const RegexCacao::MatchingMap::ResultMap& RegexCacao::MatchingMap::search_by_regex( const std::string regex ) const {

    ResultMap* rm = new ResultMap( regex, this->data(), this->rtype );

    for ( typename const_iterator cIt : { map_type::cbegin(), map_type::cend() } ) {
        if ( this->type == Type::Regex ) {
            if ( cIt->first == regex ) {
                Match::List* ml = new Match::List( regex, this->substr().data(), this->rtype );
                
                const MatchingDict& md = cIt->second;
                for ( typename Match::Dict::value_type mdIt : md.first ) {
                    ml->append_range( std::ranges::subrange{ mdIt.second.cbegin(), mdIt.second.cend() } );
                }

                PrefixSuffix presuf = this->calculate_prefix_suffix( *this, *ml );
                rm->emplace( ResultMap::value_type{ regex, Matching{ *ml, presuf } } );
            }
        } else if ( this->type == Type::Str ) {
            Match* m = new Match( cIt->first, *this );
            if ( m->is_matching( regex, this->rtype ) ) {
                Match::List* ml = new Match::List( cIt->first, this->substr().data(), this->rtype );

                PrefixSuffix presuf = this->calculate_prefix_suffix( cIt->first, *ml );
                rm->emplace( ResultMap::value_type{ cIt->first, Matching{ *ml, presuf } } );
            }
        }
    }
    
    return *rm;
}

const RegexCacao::MatchingMap::ResultMap& RegexCacao::MatchingMap::search_by_str( const std::string rstr ) const {

    ResultMap* rm = new ResultMap( this->data(), rstr, this->rtype );

    for ( typename const_iterator cIt : { map_type::cbegin(), map_type::cend() } ) {
        if ( this->type == Type::Str ) {
            if ( cIt->first == rstr ) {
                Match::List* ml = new Match::List( rstr, this->data(), this->rtype );
                
                const MatchingDict& md = cIt->second;
                for ( typename Match::Dict::value_type mdIt : md.first ) {
                    ml->append_range( std::ranges::subrange{ mdIt.second.cbegin(), mdIt.second.cend() } );
                }

                const PrefixSuffix& presuf = this->calculate_prefix_suffix( *this, *ml );
                rm->emplace( ResultMap::value_type{ rstr, Matching{ *ml, presuf } } );
            }
        } else if ( this->type == Type::Regex ) {
            Match* m = new Match( cIt->first, *this );
            if ( m->is_matching( rstr, this->rtype ) ) {
                Match::List* ml = new Match::List( cIt->first, rstr, this->rtype );

                const PrefixSuffix& presuf = this->calculate_prefix_suffix( cIt->first, *ml );
                rm->emplace( ResultMap::value_type{ rstr, Matching{ *ml, presuf } } );
            }
        }
    }
    
    return *rm;
}

const RegexCacao::PrefixSuffix& RegexCacao::MatchingMap::calculate_prefix_suffix( const std::string mstr
                                            , const std::string matching_str
                                            , const std::strin regex
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
                                            , const Match::List& matching_list
                                            , const size_t str_pos
                                            , const size_t str_end
) {
    size_t pos_smallest = str_pos;
    size_t pos_greatest = str_end == 0 ? mstr.length() - 1 : str_end;

    for ( typename Matching::List::const_iterator mIt : std::ranges::contiguous_range{ matching_list.cbegin(), matching_list.cend() }) {
        const Match& m = *mIt;
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

RegexCacao::MatchingMap& RegexCacao::MatchingMap::getMatchingMap( const std::string mstr
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
    const Match::List& ml = rc->match( mstr );
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


RegexCacao::MatchingMap& RegexCacao::getMatching( const std::string mstr, const std::string regex
                                , const bool matchAllPositions
                                , const size_t matchingPositionStart
                                , const size_t matchingPositionEnd ) const {
    return MatchingMap::getMatchingMap( mstr, regex, this->type, false, matchAllPositions
                                , matchingPositionStart, matchingPositionEnd );
}

RegexCacao::MatchingMap& RegexCacao::getMatching( const std::string mstr
                            , const bool matchingMapIsRegex
                            , const bool matchAllPositions
                            , const size_t matchingPositionStart
                            , const size_t matchingPositionEnd ) const {
    return MatchingMap::getMatchingMap( mstr, this->str, this->type
                                , matchingMapIsRegex, matchAllPositions
                                , matchingPositionStart, matchingPositionEnd );
}

RegexCacao::MatchingMap& RegexCacao::getMatching( const PrefixSuffix prefixsuffix
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
