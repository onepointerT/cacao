
#include "cacao_str.hpp"

#include <random>
#include <sstream>


namespace cacao {


Range::Range( const unsigned int range_start
         , const unsigned int range_end
         , const std::string range_content
         , const bool enclosed_content
)   :   std::string( range_content )
    ,   start( range_start )
    ,   end( range_end )
    ,   enclosed( enclosed_content )
{}


std::string Range::prefix() const {
    if ( ! this->enclosed ) return "";
    return this->substr( 0, this->start );
}

std::string Range::suffix() const {
    if ( ! this->enclosed ) return "";
    return this->end != this->npos ? this->substr( this->end - 1 ) : "";
}

std::string Range::str() const {
    if ( ! this->enclosed ) return this->substr();
    return this->substr( this->start, this->end );
}

Range* Range::findContentUntil( const std::string str
                , const Range* var_range_current
                , const Range* var_range_next
) {
    return nullptr;
}

Range* Range::findContentUntilDelim( const std::string str
                , const char* delim
) {
    return nullptr;
}


unsigned int Str::countprefix( const std::string str
                            , const char* findstr
                            , size_t pos
                            , size_t pos_until
) {
    std::string _opstr;
    if ( pos > 0 ) {
        _opstr = str.substr( pos );
    } else if ( pos_until > 0 && pos > 0 ) {
        _opstr = str.substr( pos, pos_until-pos );
    } else if ( pos_until > 0 ) {
        _opstr = str.substr( 0, pos_until );
    } else {
        _opstr = str;
    }

    unsigned int count = 0;
    std::string findstring = findstr;
    for ( unsigned int i = 0; i < _opstr.length()-findstring.length(); i++ ) {
        const std::string srch_str = _opstr.substr( i, findstring.length() );
        if ( srch_str.compare( findstring ) == 0 ) {
            count++;
        } else {
            return count;
        }
    }
    return count;
}

int Str::find( const std::string str
                   , const char* findstr
                   , size_t pos
                   , size_t pos_until
) {
    std::string _opstr;
    if ( pos > 0 ) {
        _opstr = str.substr( pos );
    } else if ( pos_until > 0 && pos > 0 ) {
        _opstr = str.substr( pos, pos_until-pos );
    } else if ( pos_until > 0 ) {
        _opstr = str.substr( 0, pos_until );
    } else {
        _opstr = str;
    }

    unsigned int count = 0;
    std::string findstring = findstr;
    for ( unsigned int i = 0; i < _opstr.length()-findstring.length(); i++ ) {
        const std::string srch_str = _opstr.substr( i, findstring.length() );
        if ( srch_str.compare( findstring ) == 0 ) {
            return i;
        }
    }
    return -1;
}

int Str::findr( const std::string str
                    , const char* findstr
                    , size_t pos
                    , size_t pos_until
) {
    std::string _opstr;
    if ( pos > 0 ) {
        _opstr = str.substr( pos );
    } else if ( pos_until > 0 && pos > 0 ) {
        _opstr = str.substr( pos, pos_until-pos );
    } else if ( pos_until > 0 ) {
        _opstr = str.substr( 0, pos_until );
    } else {
        _opstr = str;
    }

    unsigned int count = 0;
    std::string findstring = findstr;
    for ( unsigned int i = _opstr.length()-1; i > 0; i-- ) {
        const std::string srch_str = _opstr.substr( i-findstring.length(), findstring.length() );
        if ( srch_str.compare( findstring ) == 0 ) {
            return i;
        }
    }
    return -1;
}

Range* Str::find_delim_in_tmpl( const std::string tmpl_str
                        , const Range* var_range_current
                        , const Range* var_range_next
) {
    unsigned int pos_tmpl_var_current = var_range_current->start;
    unsigned int pos_tmpl_var_next = Str::find( tmpl_str, var_range_next->c_str()
                        , pos_tmpl_var_current + var_range_current->length()
    );
    if ( pos_tmpl_var_next == -1 ) {
        return nullptr;
    }

    diff_pair_t diff = diff_simple( tmpl_str.substr(pos_tmpl_var_current, pos_tmpl_var_next-pos_tmpl_var_current)
                            , *var_range_current
    );

    return new Range( var_range_current->end + 1, var_range_current->end + 1 + diff.second.length(), diff.second );
}

Range* Str::find_delim_until( const std::string str
                        , const Range* from_range
                        , const char* delim
                        , const Range* rstart
) {
    unsigned int pos_delim = Str::findr( str, delim, from_range->start, rstart->end );
    return new Range( from_range->start, pos_delim, str.substr( from_range->start, pos_delim - from_range->start ) );
}


Str::diff_pair_t Str::diff_simple( const std::string str1
                            , const std::string str2
) {
    diff_pair_t diff = std::make_pair(std::make_pair("", ""), "");

    std::string outer;
    if ( Str::find(str1, str2.c_str()) > -1 ) {
        diff.second = str2;
        outer = str1;
    } else if ( Str::find( str2, str1.c_str() ) > -1 ) {
        diff.second = str1;
        outer = str2;
    } else return diff;

    int pos_inner = Str::find( outer, diff.second.c_str() );
    int pos_end_inner = pos_inner + diff.second.length();

    diff.first.first = outer.substr( 0, pos_inner );
    diff.first.second = outer.substr( pos_end_inner + 1 );

    return diff;
}

std::string Str::replaceAll( const std::string str
                            , const char* searchstr
                            , const char* replacestr
                            , const bool prefix_only
) {
    std::string* s = new std::string( str );
    std::string ss = searchstr;

    if ( prefix_only ) {
        unsigned int count_prefix = 0;
        unsigned int cpos = 0;
        while ( s->compare( cpos, ss.length(), searchstr ) == 0 ) {
            ++count_prefix;
            cpos += ss.length();
        }
        return s->replace( 0, ss.length()*count_prefix, replacestr );
    }

    while ( size_t c = s->find_first_of( searchstr ) != std::string::npos ) {
        s->replace( c, ss.length()-c, replacestr );
    }
    return *s;
}

std::string Str::replacePrefix( const std::string str
                            , const char* prefixstr
                            , const char* replacestr
) {
    std::string* s = new std::string( str );
    std::string ps = prefixstr;
    if ( size_t c = s->find_first_of( ps ) != std::string::npos ) {
        s->replace( c, ps.length(), replacestr );
    }
    return *s;
}


bool Str::replace( std::string& str, const PrefixSuffix& presuf, const char* replacestr ) {
    const size_t pos_prefix_end = presuf.prefix().str().length() > 0
                                            ? str.find_first_of( presuf.prefix().str() ) + presuf.prefix().str().length()
                                            : 0;
    const size_t pos_suffix_start = presuf.suffix().str().length() == 0
                                        ? str.npos
                                        : str.find_first_of( presuf.suffix().str() );

    size_t original_length = ( pos_suffix_start != str.npos ? pos_suffix_start : str.length() )
                        - ( pos_prefix_end != str.npos ? pos_prefix_end + 1 : 0 );
    
    if ( pos_prefix_end != str.npos ) {
        if ( pos_suffix_start != str.npos ) {
            str.erase( pos_prefix_end != 0 ? pos_prefix_end + 1 : 0, original_length );
            str.insert( pos_prefix_end != 0 ? pos_prefix_end + 1 : 0, replacestr );
        } else {
            str.erase( pos_prefix_end != 0 ? pos_prefix_end + 1 : 0, original_length);
            str.insert( pos_prefix_end != 0 ? pos_prefix_end + 1 : 0, replacestr );
        }
        return true;
    } else if ( pos_prefix_end == str.npos ) {
        str.erase();
        str += replacestr;
        return true;
    } else return false;
}


Str::token_list_t Str::splitat( const char* delimiter
                            , const std::string str
                            , const bool include_delimiter
) {
    token_list_t* tokens = new token_list_t();
    unsigned int current_first_pos = 0;
    std::string ds = delimiter;
    for ( unsigned int idx = 0; idx < str.length(); idx++ ) {
        if ( str.substr( idx, ds.length() ).compare(delimiter) == 0 ) {
            if ( include_delimiter ) {
                tokens->push_back( str.substr( current_first_pos, idx + ds.length() - current_first_pos ) );
            } else {
                tokens->push_back( str.substr( current_first_pos, idx - 1 - current_first_pos ) );
            }
            current_first_pos = idx + ds.length() + 1;
            idx = current_first_pos - 1;
        }
    }
    return *tokens;
}


Str::Pattern::Pattern( const std::string delim_left, const std::string delim_right
                     , const std::string pattern_between
                     , const std::regex_constants::syntax_option_type rtype
)   :   std::pair< std::string, std::string >{ delim_left, delim_right }
    ,   std::string( pattern_between )
    ,   regex_type( rtype )
{}

const Str::Pattern::position_t Str::Pattern::findWithin( const std::string str
                                                       , const std::string prefix
                                                       , const std::string suffix
) {
    std::string regex_str = prefix + *this + suffix;
    std::regex rgx( regex_str, regex_type );
    std::smatch m;
    if ( std::regex_search( str, m, rgx, std::regex_constants::match_continuous ) ) {
        return *new cacao::Range( m.position(), m.position() + m.length(), m.str() );
    }

    return *new cacao::Range( 0, 0, "" );
}

const std::list< Str::Pattern::position_t >& Str::Pattern::find( const std::string str
                                                         , const std::string prefix
                                                         , const std::string suffix
) {
    std::string regex_str = "^(" + prefix + *this + suffix + ")";
    std::regex rgx( regex_str, regex_type );
    std::smatch m;
    std::list< Str::Pattern::position_t >* lst = new std::list< Str::Pattern::position_t >();
    if ( std::regex_search( str, m, rgx , std::regex_constants::match_continuous ) ) {
        lst->push_front( *new cacao::Range( m.position(), m.position() + m.length(), m.str() ) );
        if ( m.prefix().length() > 0 && std::regex_search( m.prefix().str(), rgx ) ) {
            const std::list< Str::Pattern::position_t >& lst_append_prefix = this->find( m.prefix() );
            lst->append_range( std::ranges::subrange{ lst_append_prefix.cbegin(), lst_append_prefix.cend() } );
        }
        if (  m.suffix().length() > 0 && std::regex_search( m.suffix().str(), rgx ) ) {
            const std::list< Str::Pattern::position_t >& lst_append_suffix = this->find( m.suffix() );
            lst->append_range( std::ranges::subrange{ lst_append_suffix.cbegin(), lst_append_suffix.cend() } );                
        }
    }

    return *lst;
}


Str::Trajection::Trajection( const std::string refactoring_from, const std::string refacetoring_to )
    :   std::pair< std::string_view, std::string_view >{ refactoring_from, refacetoring_to }
{}

const size_t Str::Trajection::findInStr( const std::string str ) const {
    return str.find_first_of( this->first );
}

std::string& Str::Trajection::replaceInStr( const std::string str ) const {
    const size_t pos = this->findInStr( str );
    std::string* str_new = new std::string( str );
    if ( pos == str.npos ) return *str_new; // Already here
    str_new->insert( pos, this->second );
    str_new->erase( pos + this->second.length(), this->first.length() );
    return *str_new;
}

unsigned short Str::Trajection::refactorAllOccurrences( std::string& str ) const {
    unsigned short found = 0;
    while ( size_t pos = str.find_first_of(this->first) != str.npos ) {
        ++found;
        str = this->replaceInStr( str );
    }
    return found;
}


Str::Trajection::List::List()
    :   std::list< Trajection* >()
{}

bool Str::Trajection::List::refactorWith( const size_type pos, std::string& str ) const {
    const_iterator cIt = this->begin();
    for ( size_type p = 0; p <= pos; ++cIt, ++p ) {}
    return this->refactorWith( cIt, str );
}

bool Str::Trajection::List::refactorWith( const const_iterator cIt, std::string& str ) const {
    std::string& str_result = (*cIt)->replaceInStr( str );
    if ( str_result.compare(str) != 0 ) {
        str = str_result;
        return true;
    } return false;
}

bool Str::Trajection::List::refactorAllOccurrences( std::string& str ) const {
    for ( typename const_iterator cIt = this->begin(); cIt != this->end(); ++cIt ) {
        if ( this->refactorWith( cIt, str ) ) continue;
        else return false;
    } return true;
}



bool Str::replace( std::string_view& sv, typename std::string_view::reverse_iterator svRIt, const size_t _Count, const char* replacestr ) {
    std::string str = sv.data();
    
    size_t pos = sv.size() - 1;
    for ( typename std::string_view::reverse_iterator svIt = sv.rbegin()
        ; svIt != sv.rend(); ++svIt
    ) { if ( svRIt == svIt ) break; else --pos; }

    if ( pos < 0 ) return false;

    str.insert( pos, replacestr );
    str.erase( pos + strlen(replacestr), _Count );

    sv.swap( *new std::string_view(str) );
    return true;
}



uuid_t genuuidv4() {
  std::stringstream s;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 15);
  std::uniform_int_distribution<> dis2(8, 11);

  s << std::hex;
  for (unsigned int i = 0; i < 8; i++) {
      s << dis(gen);
  }
  s << "-";
  for (unsigned int i = 0; i < 4; i++) {
      s << dis(gen);
  }
  s << "-4";
  for (unsigned int i = 0; i < 3; i++) {
      s << dis(gen);
  }
  s << "-";
  s << dis2(gen);
  for (unsigned int i = 0; i < 3; i++) {
      s << dis(gen);
  }
  s << "-";
  for (unsigned int i = 0; i < 12; i++) {
      s << dis(gen);
  }
  return s.str();
}


uuid_t genid() {
    return Str::replaceAll(genuuidv4(), "-", "");
}


} // namespace cacao
