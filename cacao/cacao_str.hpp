
#pragma once

#include <array>
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
        == std::ranges::end(std::ranges::subrange{start, end}).front() };
};

} // namespace std

namespace cacao {

class Range
    :   public std::string
{
public:
    const size_t start;
    const size_t end;
    const bool enclosed;


    Range( const unsigned int range_start
         , const unsigned int range_end
         , const std::string range_content
         , const bool enclosed_range = true
    );

    std::string prefix() const;
    std::string suffix() const;
    std::string str() const;

    static Range* findContentUntil( const std::string str
                    , const Range* var_range_current
                    , const Range* var_range_next
    );
    static Range* findContentUntilDelim( const std::string str
                    , const char* delim = ""
    );
};


class EnclosingContent
    :   public Range
{
public:
    enum Type {
        Prefix,
        Suffix
    } ectype;

    EnclosingContent( const std::string cstr, const std::string complete_str
                    , const unsigned int range_start, const unsigned int range_end
                    , const EnclosingContent::Type ctype = EnclosingContent::Type::Prefix );
};


class PrefixSuffix
    :   public std::pair< EnclosingContent*, EnclosingContent* >
{
public:
    PrefixSuffix( EnclosingContent& prefix, EnclosingContent& suffix );
    PrefixSuffix( std::array<std::string, 2> presuf );

    const EnclosingContent& prefix() const;
    const EnclosingContent& suffix() const;
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

    static bool replace( std::string& str, const PrefixSuffix& presuf, const char* replacestr );

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
               , const std::string pattern_between = "\\s*"
               , const std::regex_constants::syntax_option_type rtype
                                = std::regex_constants::icase
        );

        typedef typename cacao::Range position_t;

        const position_t findWithin( const std::string str
                                , const std::string prefix = ""
                                , const std::string suffix = ""
        );

        const std::list< position_t >& find( const std::string str
                                                 , const std::string prefix = "(!( ))"
                                                 , const std::string suffix = "(!( ))"
        );
    };

    /**
     * @brief A trajection is useful especially for transforming parts inside of a string.
     *      It consists of the old string as first part of the inherited pair and as second
     *      part of the pair it holds then a `std::string_view` with the replacement.
     */
    class Trajection
        :   public std::pair< std::string_view, std::string_view >
    {
    public:
        /**
         * @brief Constructor
         * @param refactoring_from The original string that is to be replaced or searched for
         * @param refactoring_to The replacement string
         */
        Trajection( const std::string refactoring_from, const std::string refacetoring_to );

        /**
         * @brief Find the position of the original inside of `str`
         * @param str The string to search in
         * @returns The position of `this->first` inside of `str` or `str.npos`, if the original
         *      was not found
         */
        const size_t findInStr( const std::string str ) const;
        /**
         * @brief Replace the original with the second value (the replacement).
         * @param str The string to replace in
         * @returns A reference to a new string with the occurence of the original replaced by `this->second`
         */
        std::string& replaceInStr( const std::string str ) const;

        /**
         * @brief All occurences, this thus means appearences of the original inside of `str` can be replaced
         * @param str A reference to a string, where all appearences of the original (`this->first`) are replaced
         *      by `this->second`
         * @returns The number of occurences, that where replaced in `str`
         */
        unsigned short refactorAllOccurrences( std::string& str ) const;

        /**
         * @brief A list of trajections. It can refactor strings with all Trajections at once or with
         *      especially one trajection found inside of the list.
         */
        class List
            :   protected std::list< Trajection* >
        {
        public:
            /**
             * @brief Constructor
             */
            List();

            typedef typename std::list< Trajection* > list_t;
            using typename list_t::const_iterator;
            using list_t::cbegin;
            using list_t::cend;
            using list_t::size;
            using list_t::empty;
            using list_t::append_range;
            using list_t::emplace_front;
            using list_t::push_front;
            using list_t::size_type;

            /**
             * @brief Refactor a string `str` with the trajection found as positional index `pos`
             * @note See also `Trajection::replaceInStr` for more information on what is happening in this function.
             * @param pos The positional index of the `Trajection*` inside of the list. This trajection is then
             *      used as refactoring definition for `str`
             * @param str The reference to the string to replace and call with `str = (*this)[pos]->replaceInStr(str);`.
             * @returns True on success
             */
            bool refactorWith( const size_type pos, std::string& str ) const;
            /**
             * @brief Refactor a string `str` with the trajection found as positional iterator at `cIt`
             * @note See also `Trajection::replaceInStr` for more information on what is happening in this function.
             * @param pos The positional index of the `Trajection*` inside of the list. This trajection is then
             *      used as refactoring definition for `str`
             * @param str The reference to the string to replace and call with `str = (*cIt)->replaceInStr(str);`.
             * @returns True on success
             */
            bool refactorWith( const const_iterator cIt, std::string& str ) const;
            /**
             * @brief Refactor everytime, a trajection's original of this list can be found in a string `str` with
             *      ´this->refactorWith(..., str);´
             * @param str A reference to the string to replace in
             * @returns True on success.
             */
            bool refactorAllOccurrences( std::string& str ) const;
        };
    };

    typedef std::list< Str::Pattern::position_t > position_list_t;

    static bool replace( std::string_view sv, typename std::string_view::reverse_iterator svRIt
                    , const size_t _Count, const char* replacestr );
};


typedef std::string uuid_t;


uuid_t genuuidv4();
uuid_t genid();


} // namespace cacao