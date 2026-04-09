// Copyright (C) 2026 The OnePointer Authors.
//

#pragma once

#include <initializer_list>
#include <list>
#include <map>
#include <regex>
#include <string>
#include <string_view>

#include <cacao_vte.hpp>


namespace cacao {

typedef enum {
    StlRegex,
    EcmaRegex,
    PosixRegex,
    ICase,
    Cacao,
    CacaoRegex,
    CacaoECMA
} RegexType;

std::regex_constants::syntax_option_type toRegexConstants( const RegexType rtype );

typedef enum {
    Cacao,
    CacaoRegex,
    CacaoECMA
} RegexTypeCacao;

std::regex_constants::syntax_option_type toRegexConstants( const RegexTypeCacao rtype_cacao );
RegexType toRegexType( const RegexTypeCacao rtype_cacao );

class RegexCacao
    :   public std::regex
{
public:
    const std::string_view str;
    const RegexType type;

    RegexCacao( const std::string regex, const RegexType rtype = EcmaRegex );

    static std::list<Environment::Variable*> const& find_env_variables( const std::string mstr, const RegexTypeCacao rtc = Cacao );
    static std::list<Environment::Variable*> const& find_env_variables_store( const std::string mstr, const RegexTypeCacao rtc = Cacao );
    static std::list<Environment::Variable*> const& find_env_variables_read( const std::string mstr, const RegexTypeCacao rtc = Cacao );
    static std::list<Environment::Variable*> const& find_env_variables_set( const std::string mstr, const RegexTypeCacao rtc = Cacao );

    typedef std::pair< std::string_view, std::string_view > PrefixSuffix;

    class Match
        :   public std::string_view
    {
    public:
        const size_t position;
        
        // Forward declaration
        class List;
        class Dict;
        
        Match( const std::string matching_str, const size_t pos );

        typedef std::smatch Result;

        static Result& fit_in( const std::string regex1, const std::string regex2
                        , std::regex_constants::syntax_option_type regex_type = std::regex_constants::ECMAScript
        );

        bool is_matching( const std::string str_regex, const RegexType regex_type = RegexType::Cacao ) const;
        Match const& regex( const std::string str_regex, const RegexType regex_type = RegexType::CacaoRegex ) const;
        const List const& regexMatchAll( const std::string str_regex, const RegexType regex_type = RegexType::EcmaRegex ) const;
        std::string str() const;

        bool empty() const;

        class List
            :   public std::list< Match >
        {
        public:
            const std::string rgx;
            const std::string str;
            const RegexType rtype;

            using MatchIterator = typename std::list<Match>::const_iterator;

            List( const std::string regex = "", const std::string mstr = "", const RegexType rtype = RegexType::Cacao );
            List( const std::list< Match > const& other );

            MatchIterator it;

            bool has_next() const;
            Match next();

            static std::list<Match>& matchlist( const std::string regex, const std::string str, const RegexType rtype = RegexType::Cacao );

            operator std::basic_string<char>() const;
            operator std::basic_regex<char>() const;
            operator RegexType() const;
        };

        class Dict
            :   public std::map< std::string, Match::List >
        {};
    };

    Match::Dict& matchDict;

    const Match::List const& match( const std::string mstr ) const;
    const Match::Dict const& matchAll( const std::initializer_list<std::string> il_mstr ) const;
    const Match const& matchOnce( const std::string mstr ) const;

    operator std::basic_regex<char>() const;
    operator std::basic_string<char>() const;
    operator RegexType() const;
    
    typedef std::pair< Match::List, PrefixSuffix > Matching;
    typedef std::pair< Match::Dict, PrefixSuffix > MatchingDict;

    class MatchingMap
        :   protected std::map< std::string, MatchingDict >
        ,   protected std::string_view
    {
    public:
        const enum RegexType rtype;
        enum Type {
            Regex,
            Str,
            undefined
        } type = Type::undefined;

        MatchingMap( const std::string str_or_regex, const Type mtype = Type::Regex
                   , const RegexType rtype = RegexType::ICase
        );

        typedef typename std::map< std::string, MatchingDict > map_type;
        using typename map_type::mapped_type;
        using typename map_type::const_reference;
        using typename map_type::reference;
        using typename map_type::const_iterator;
        using typename map_type::iterator;
        using typename map_type::value_type;
        using typename map_type::key_type;
        using map_type::contains;
        using map_type::lower_bound;
        using map_type::upper_bound;
        using map_type::find;
        using map_type::insert_or_assign;
        using map_type::insert_range;

        mapped_type& operator[]( const key_type key );
        const mapped_type& operator[]( const key_type key ) const;

        iterator insert_or_assign_range( const_iterator start, const_iterator end );

        operator std::basic_string<char>() const;

        class ResultList
            :   public std::list< MatchingDict const& >
        {
        public:
            const std::string_view rgx;
            const std::string_view str;
            const RegexType rtype;

            ResultList( const std::string regex, const std::string rstr
                      , const RegexType regex_type = RegexType::ICase );
        };

        class ResultMap
            :   public std::multimap< std::string, Matching const& >
        {
        public:
            const std::string_view rgx;
            const std::string_view str;
            const RegexType rtype;

            ResultMap( const std::string regex, const std::string rstr
                     , const RegexType regex_type = RegexType::ICase );
        };

        class Result
            :   public Match::List
            ,   public PrefixSuffix
        {
        public:
            const enum RegexType rgx_type;

            Result( Match::List const& ml, const PrefixSuffix prfsuf, const RegexType rtype = RegexType::Icase );
        };

        const ResultList& search_by_prefix( const std::string prefix ) const;
        const ResultList& search_by_suffix( const std::string suffix ) const;
        const ResultList& search_by_prefixsuffix( const PrefixSuffix presuf ) const;
        const ResultMap& search_by_dictKey( const std::string key ) const;
        const ResultMap& search_by_dictKeys( const std::initializer_list<std::string> keys ) const;
        const ResultMap& search_by_regex( const std::string regex ) const;
        const ResultMap& search_by_str( const std::string rstr ) const;

        static const PrefixSuffix& calculate_prefix_suffix( const std::string mstr
                                                   , const std::string matching_str
                                                   , const std::string regex
                                                   , const size_t str_pos = 0
                                                   , const size_t str_end = 0
        );
        static const PrefixSuffix& calculate_prefix_suffix( const std::string mstr
                                                   , const Match::List const& matching_list
                                                   , const size_t str_pos = 0
                                                   , const size_t str_end = 0
        );

        static MatchingMap const& getMatchingMap( const std::string mstr
                                                , const std::string regex
                                                , const RegexType rtype = RegexType::ICase
                                                , const bool matchingMapIsRegex = true
                                                , const bool matchAllPositions = true
                                                , const size_t matchingPositionStart = 0
                                                , const size_t matchingPositionEnd = 0
        );

    private:
        ResultList* makeResultList( const std::string str_or_regex, const RegexType rtype ) const;
        ResultMap* makeResultMap( const std::string str_or_regex, const RegexType rtype ) const;
    };

    MatchingMap const& getMatching( const std::string mstr, const std::string regex
                                  , const bool matchAllPositions = true
                                  , const size_t matchingPositionStart = 0
                                  , const size_t matchingPositionEnd = 0 ) const;
    MatchingMap const& getMatching( const std::string mstr
                                  , const bool matchingMapIsRegex = true
                                  , const bool matchAllPositions = true
                                  , const size_t matchingPositionStart = 0
                                  , const size_t matchingPositionEnd = 0 ) const;
    MatchingMap const& getMatching( const PrefixSuffix prefixsuffix
                                  , const std::string mstr
                                  , const bool matchingMapIsRegex = true
                                  , const bool matchAllPositionsBetween = true
                                  , const size_t matchingPositionStart = 0
                                  , const size_t matchingPositionEnd = 0 ) const;


};



// # Variable regexes
inline RegexCacao* regex_var
            = new RegexCacao( "^#^([a-z][A-Z][0-9]){+}((\{^*\}){+})", RegexType::EcmaRegex );
inline RegexCacao* regex_braces
            = new RegexCacao( "^(_\{)+(^\s*)^(\}_)+", RegexType::ICase );
inline RegexCacao* regex_var_with_braces 
            = new RegexCacao( "^#^([a-z][A-Z][0-9]){+}\{^((^#{0-1)(^([a-z][A-Z][0-9]){+})){+}\}", RegexType::EcmaRegex );
inline RegexCacao* regex_var_with_with_action
            = new RegexCacao( "#^([a-z][A-Z][0-9]){+}\{^((^#{0-1)(^([a-z][A-Z][0-9]){+})){+}\}\
                \{^((^#{0-3})(^([a-z][A-Z][0-9]){+})){+}\}", RegexType::EcmaRegex );
inline RegexCacao* regex_var_with_formatter 
            = new RegexCacao( "#^([a-z][A-Z][0-9]){+}\{^((^#{0-1)(^([a-z][A-Z][0-9]){+})){+}\}\
                \{^((^#{0-3})(^([a-z][A-Z][0-9]){+})){+}\}\{^((^#{+})(^([a-z][A-Z][0-9]){+})){+}\}", RegexType::EcmaRegex );


// # Variable types
inline RegexCacao* regex_var_store
            = new RegexCacao( "^((#){1}\s+((_\{\s+\}){1-2})_#)", RegexType::EcmaRegex );
inline RegexCacao* regex_var_read
            = new RegexCacao( "^((##){1}\s+_\{\s+\}((_{\s+}){1-2})_##)", RegexType::EcmaRegex );
inline RegexCacao* regex_var_set
            = new RegexCacao( "^((###){1}\s+_\{\s+\}_###)", RegexType::EcmaRegex );

// # Variable partial detection
inline RegexCacao* regex_detect_varname
            = new RegexCacao( "(#+)(^\s)", RegexType::EcmaRegex );
inline RegexCacao* regex_detect_braces
            = new RegexCacao( "^(\{\s+\})+", RegexType::EcmaRegex );
inline RegexCacao* regex_detect_inbraces
            = new RegexCacao( "(\{{1})(^\s+)(\}{1})", RegexType::EcmaRegex );

} // namespace cacao

