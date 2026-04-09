
#pragma once

#include <concepts>
#include <list>
#include <memory>
#include <ranges>
#include <regex>
#include <string>
#include <utility>
#include <vector>


namespace std {

template< class InputIt >
concept is_same_object_iterator
            = requires ( InputIt start, InputIt end )
{
    { std::ranges::begin(std::ranges::subrange{ start, end }).front()
        == std::ranges::end(std::ranges::subrange{start, end}).front() }
};

} // namespace std

namespace cacao {

class Range
    :   public std::string
{
public:
    const unsigned int start;
    const unsigned int end;


    Range( const unsigned int range_start
         , const unsigned int range_end
         , const std::string range_content
    );
};


class Str {
public:
    static unsigned int countprefix( const std::string str
                            , const char* findstr
                            , size_t pos = 0
                            , size_t pos_until = 0
    );

    static int find( const std::string str
                   , const char* findstr
                   , size_t pos = 0
                   , size_t pos_until = 0
    );

    static int findr( const std::string str
                    , const char* findstr
                    , size_t pos = 0
                    , size_t pos_until = 0
    );

    static Range* find_delim_in_tmpl( const std::string tmpl_str
                        , const Range* var_range_current
                        , const Range* var_range_next
    );

    static Range* find_delim_until( const std::string str
                        , const Range* from_range
                        , const char* delim
                        , const Range* rstart
    );

    typedef std::pair< std::string, std::string > str_pair_t;
    typedef std::pair< str_pair_t, std::string > diff_pair_t;

    static diff_pair_t diff_simple( const std::string str1
                            , const std::string str2
    );

    static std::string replaceAll( const std::string str
                            , const char* searchstr
                            , const char* replacestr
                            , const bool prefix_only = false
    );

    static std::string replacePrefix( const std::string str
                            , const char* prefixstr
                            , const char* replacestr
    );

    typedef std::vector< std::string > token_list_t;

    static token_list_t splitat( const char* delimiter
                            , const std::string str
                            , const bool include_delimiter = false
    );

    class Pattern final
        :   public std::pair< std::string, std::string >
        ,   protected std::string
    {
    private:
        std::regex_constants::syntax_option_type regex_type;    

    public:
        Pattern( const std::string delim_left, const std::string delim_right
               , const std::string pattern_between = "\s*"
               , const std::regex_constants::syntax_option_type rtype
                                = std::regex_constants::icase
        );

        typedef typename cacao::Range position_t;

        const position_t findWithin( const std::string str
                                , const std::string prefix = ""
                                , const std::string suffix = ""
        );

        const std::list< position_t > const& find( const std::string str
                                                 , const std::string prefix = ""
                                                 , const std::string suffix = ""
        );
    };
};


typedef std::string uuid_t;


uuid_t genuuidv4();
uuid_t genid();


} // namespace cacao