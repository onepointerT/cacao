
#pragma once

#include <string>
#include <utility>
#include <vector>


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
};


typedef std::string uuid_t;


uuid_t genuuidv4();
uuid_t genid();


} // namespace cacao