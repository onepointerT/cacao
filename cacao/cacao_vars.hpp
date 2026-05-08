
#pragma once

#include <string_view>

#include "cacao_stack.hpp"
#include "cacao_regex.hpp"
#include "cacao_tmpl.hpp"


/**
 * @brief The namespace of the `cacao` library. Concurrent Aribtrary Consent Artificial objects means,
 * thus inside an arbitry sequence of characters your consentual, deterministic locatable object can be predicted
 * by regex or surroundment and is then transformed with the data variables known to the stack(s) of the library or
 * your frontend using `cacao` and enables to make your User/UI/Library/Program experience artifical. predictable. (multi)variat.
 * and dynamic. and less chaotic on user input or undeterministic incominc character sequences, the benefitial currency
 * and also concurrency of the library `cacao`.
 */
namespace cacao {



/**
 * @brief The class Variable is, what can be found in CacaoTemplates and is needed to detect and transform input strings.
 * Variables can define template's values, can be looked up in `Stack< Variable >` and `Environment::Stack` (`cacao::env`),
 * and are defined by a first template on where they are in an incoming character sequence, what to be filled in from cacao's
 * environment stacks (second brace behind a variable's name) and can be formatted like with `sprintf`, when a third brace is
 * before the closing delimiter. Variable inside of braces of a variable are written with prefixed `'#'` and an enclosing `"_#"` and
 * outside of braces, functional variables and CacaoTemplate-locatiner are written with one to three trailing and leading `'#'` and
 * two underscores two differ from the concluding and brace-enclosed variables and make parsing and reading more easer, especially they
 * and they delimiters needed to differ from every else what is on the insput strings. Lastly, a prefixing `'$'` puts the result of a
 * variadic variable value lookup (with templating) inside of a output character sequence and does not handle it as a basic, simple
 * cacao calligraphic value. A `"$."` in the formatter brace simply puts in the result of the action brace, the second brace (may it be inside of the action
 * brace itself or the formatter). The first brace
 * locates and finds the variadic variable's value, if and only if it can be found in the same sequence of characters. If it is in another
 * sequence of characters to be found, you need to program the lookups in _your_ libraries and pass the stack of the other file/string to
 * the set-in template.
 * 
 * For example `find_ref_css_classes.cacao`:
 * ```
 * #__ref_css_cls{(<a href=\s (^\s)> | ICase}__# <!-- << Match all character sequence behind the href's link and use an ICase Regex-Detection -->
 * ```
 * 
 * or `link_and_show_my_image.cacao` sets in with the cacao environment, what is written somewhere else on the page
 * ```
 * [...]
 * <img src="$##__img_src_my_img{(My image is located in the domain's subdirectory "){1}(^\s)( and won't be found, if we don't put in the link)+ | ECMA}_{##domain##/$./image.png}_{%s}__##" />
 * [...]
 * ```
 */
class Variable final
    :   protected std::string_view
{
public:
    /**
     * @brief The type of a variable is defined here. You could store a variable
     *      * (`#__varname_store{find it in your text: (^\s) \(with regualar expression/regex\) | ICase}_{$use_prefix#$.#$use_suffix#}_{format it: $surroundment_prefix#$.#$surroundment_postfix#\n\n$footer#}__#`)
     * or read a variable from your stack or cacao's environment
     *      * (`##__varname_read{<h>formatter brace only</h>\nUse e.g. %s the *varname_store* variable's value with the <b>value of env.main: "%s"</b> | $varname_store, $env.main }__##`; every formatter `"%_"` works like printf
     *          and needs the variable in the correct order behind the `'|'`.)
     * or create a variable from reading a character sequence (e.g. with regular expression grabbing) with the grabbing brace and the insetter brace:
     *      * (`###__varname_store{Url finding in a config file is like this in the same file $\\
     *              §!§^*§&§(\n){1}^\s§{1}&§(([_ ])+url=")^\s("\n)+§!&§(^(#// Comment: *))+§!§^*$$ | ICase | env.vn_store_$0#, env.vn_store_$0#_url, env.vn_store_$0#_comment§) $\\
     *              also please find the css classes here in this document §§###ref_css_cls###§§ | Cacao | env.vn_store_$0#_css§.}_{Formatter:\n<b><i>$0</i></b><a href="%s" /> | env.vn_store_$0#_url, $varname_read}__###$\\
     *      * regexes are inside of five paragraph characters (`'§'`), if not the only regex in the find-up/store brace and can be concatenated
     *          with and, or, negation, nor or nand; before the last, the type of regex is to be defined like written at `cacao::RegexType`. The previous regex
     *          then does not select everything around the lane that writes then `*[_ ]url="` and grabs only everything inside of the quotation/citatation characters and also the beyond comment, if any.
     *          a regex then stores it to the stack or env, e.g. `env.vn_store_$0#` stores it under the value of `env.stack[vn_store_$0#]` whatever the url's name now is prefixed with
     *          is then the variable's key suffix and it's name. For more simple examples without regular expressions, you may look at the documentation preamble of the class `Variable` itself.
     * or simply have a class object defined, because you e.g. use it evaluated for a template.
     */
    enum Type {
        store, /* Store a value to stack or environment, maybe also with insetter and formatter values */
        read, /* Read and optionally format a variable */
        create, /* Create a variable set on the stack */ // # TODO: Make variable sets on stack with cacao::stack_or_stackOfDicts
        use, /* Use the variable as object or object's instance in your library or program without templating and parsing. */
        undefined /* Something else. */
    } vartype = Type::undefined;

    /**
     * @brief An stack of currently used variables, optionally intern.
     * @note In CacaoTemplates, the variable names can be prefixed with `intern.`, `env.` and `stack.`.
     */
    typedef typename Stack< Variable > var_env_t;
    /**
     * @brief Use the variables intern stack environment, that thus knows at least all variat values of its own template variables.
     */
    var_env_t& env_intern;

    /**
     * @brief Find out the type ow a variable by handing in a string_view.
     * @returns The variable type as defined at `cacao::Variable::Type` enum.
     */
    static Variable::Type getType( const std::string_view var_str );
    /**
     * @brief Find out, if a handed in var_string is a variable.
     * @returns True, if `cacao::Variable::getType(var_str) != cacao::Variable::undefined`.
     */
    static bool is_variable( const std::string_view var_str );

    /**
     * @brief Get the variable's name in sight of a variable string. Variables are prefixed with `#`, `#_` or `$`.
     * @param var_str The string to search in
     * @param pos_start The position index inside of `var_str` to start search searching at
     * @param _occurence The search continues as long as it is not yet the n-th occurence of a variable inside of the string `var_str`
     * @return The name of the variable with all delimiters
     */
    static const std::string getVarName( const std::string_view var_str, const size_t pos_start = 0 );
    /**
     * @brief Get the name of a variable with all occurences of variable setted in, if the name of the variable consists of other variables
     * @note Normally, the intern stack of the variable/template and your program's/library's stack need to be passed to fulfil the completion of a varname.
     * @param var_str The string, where the variable is located in or the name of the variable with all delimiters as gotten from `Variable::getVarName`.
     * @param intern The intern variable stack of a template of another variable.
     * @param stack Your library's or program's or the current stack with variables
     * @param pos_start The position index inside of `var_str` to start search searching at
     * @return The name of the variable with all delimiters, but all variables inside are looked up or degraded to `$$`, if not found in one of the stacks.
     * @note If a variable could not be found in one of the stacks, then a double lookup-sign will be shown in the string (`$$`).
     */
    static const std::string getVarNameFulfilled( const std::string_view var_str, const var_env_t& intern
                                        , const Stack< Variable >* stack = nullptr, const size_t pos_start = 0 );

    /**
     * @brief Get the variable name with no delimiters and all occurences of variables setted in, if the name of the variable consists of other variables
     * @note Normally, the intern stack of the variable/template and your program's/library's stack need to be passed to fulfil the completion of a varname.
     * @param var_str The string, where the variable is located in or the name of the variable with all delimiters as gotten from `Variable::getVarName`.
     * @param intern The intern variable stack of a template of another variable.
     * @param stack Your library's or program's or the current stack with variables
     * @param pos_start The position index inside of `var_str` to start search searching at
     * @return The name of the variable with all delimiters, but all variables inside are looked up or degraded to `$$`, if not found in one of the stacks.
     * @note If a variable could not be found in one of the stacks, then a double lookup-sign will be shown in the string (`$$`).
     */
    static const std::string getVarNamePure(const std::string_view var_str, const var_env_t& intern
                                        , const Stack< Variable >* stack = nullptr, const size_t pos_start = 0 );

    /** @brief The name of the variable, optionally with delimiters */
    const std::string name;
    /** @brief The current latestly actualized value of the variable */
    std::string value; // # TODO: Timestamp of actualizing the value(s)
    /** @brief The template, how to find the variable inside of a text or string */
    const TemplateFindUp* tmpl;
    /** @brief The template, what to set into the found variable */
    const TemplateActions* actions;
    /** @brief The template, how to format the output of the variable */
    const TemplateFormatter* formatter;

    /**
     * @brief Make the find-up template out of a reference to a Regex-Match `rgxm_braces`,
     *      
     * @param rgxm_braces The regex with enclosing braces, like it was found with `cacao::regex_detect_braces`.
     * @returns The template, that fulfills the find up unto the variable content and value with and without regexes inside
     * @note Find-up templates can be located like `###varname_{FINDUP_TEMPLATE | storing_var_names, ...}_...###` and contain
     *      * every surrounding text including $# for where a value is catched up
     *      * regular expressions
     *      * a variable list, where the names of the intern stack variables are given for values stored in this variable
     * @note For further reference in later braces like the 2nd (action) or the 3rd (formatter) brace, the value `$.` references
     *      the values in the brace before the current brace or the latestly occurrence before the current brace.
     *      E.g. `$.storing_var_names` references the first value found by the FINDUP_TEMPLATE as far as referencing in the format
     *      `#varname$.storing_varname` references and reads the current value, as found latestly until now in the lastly passed research
     *      string of the variable, whilst `$.` references the complete match or inset-actional text from the braces before.
     *      If only one value is matched by this template, `$.` references all results from the FINDUP_TEMPLATE as value of the variable
     *      and also always the first match of the variables found.
     */
    static TemplateFindUp* makeTemplateFindUp( const RegexCacao::Match& rgxm_braces );
    /**
     * @brief Make the action template that thus completes the find-up template (`$.`) with values out of the intern stack
     *      of this variable or store some close-up regexes without disturbing the surroundment text to the `Variable::env_intern`.
     *      
     * @param rgxm_braces The regex with enclosing braces, like it was found with `cacao::regex_detect_braces`.
     * @returns The template, that fulfills the actions.
     * @note Action templates can be located like `###varname_{...}_{ACTION_TEMPLATE}_...###` or `#varname_{ACTION_TEMPLATE}_{FORMATTER}_#`
     *    and contain
     *      * one variable in-setter per stellaris lane (like this enumerating line is beginning with an `*`)
     *      * `$#` for store a variable, `$varname` for read from stack and `$.` for take from the previous brace
     *      * can be semantique lgs logics like `$# = $varname_prefix$.` like in programming languages for assembling variable content
     *      * a variable list, where the names of the intern stack variables are given for values stored and read in the enumerating ordering
     * @note For further reference in later braces like the 3rd (formatter) brace, the value `$.` references
     *      the value stored in the brace before the current brace or the latestly occurrence before the current brace.
     *      E.g. `... | $#` means to store every action taken to `$.` from first brace to the value of the `Variable`.
     */
    static TemplateActions* makeTemplateActions( const RegexCacao::Match& rgxm_braces );
    /**
     * @brief Make the formatter template that thus completes the action template (`$.`) with values out of the action template
     *      and the
     *      of this variable or store some close-up regexes without disturbing the surroundment text to the `Variable::env_intern`.
     *      
     * @param rgxm_braces The regex with enclosing braces, like it was found with `cacao::regex_detect_braces`.
     * @returns The template, that fulfills the actions.
     * @note Action templates can be located like `###varname_{...}_{ACTION_TEMPLATE}_...###` or `#varname_{ACTION_TEMPLATE}_{FORMATTER}_#`
     *    and contain
     *      * one variable in-setter per stellaris lane (like this enumerating line is beginning with an `*`)
     *      * `$#` for store a variable, `$varname` for read from stack and `$.` for take from the previous brace
     *      * can be semantique lgs logics like `$# = $varname_prefix$.` like in programming languages for assembling variable content
     *      * a variable list, where the names of the intern stack variables are given for values stored and read in the enumerating ordering
     * @note For further reference in later braces like the 3rd (formatter) brace, the value `$.` references
     *      the value stored in the brace before the current brace or the latestly occurrence before the current brace.
     *      E.g. `... | $#` means to store every action taken to `$.` from first brace to the value of the `Variable`.
     */
    static TemplateFormatter* makeTemplateFormatter( const RegexCacao::Match& rgxm_braces );

    explicit Variable( const std::string var_str = cacao::genid()
            , const std::string tmpl = ""
            , const std::string actions = ""
            , const std::string formatter = ""
    );
    Variable( const std::string var_str
            , const Template* tmpl_tmpl = nullptr
            , const Template* tmpl_actions = nullptr
            , const Template* tmpl_formatter = nullptr
    );
    Variable( const RegexCacao::Match& rgxm );
    Variable( const std::string varname, const std::string value );

    static Variable* fromRegex( const std::string rgx
                                , const std::string var_str
                                , const RegexTypeCacao rtype_cacao
    );
    static Variable* fromRegex( const RegexCacao::Match& var_rgxm
                                , const RegexTypeCacao rtype_cacao
    );
    static Variable* fromCode( const std::string var_str );
    static Range* findVariable( const std::string str
                        , const unsigned int start = 0
                        , const int  end = -1
    );

    typedef std::vector< Range* > range_list_t;
    typedef std::pair< Stack< Variable >*, range_list_t* > varenv_pos_t;

    static varenv_pos_t* findVariables( const std::string str
                        , const unsigned int start = 0
                        , const int  end = -1
    );

    using std::string_view::substr;

    operator std::basic_string<char>() const;

    const std::string getSelection( const Stack< Variable >* stack = nullptr ) const;
    const std::string getAction( const Stack< Variable >* stack = nullptr ) const;
    const std::string getFormatted( const Stack< Variable >* stack = nullptr ) const;
    const std::string getValue( const Stack< Variable >* stack = nullptr ) const;

    class util {
    public:
        static const Variable* lookup( const std::string varname, const Stack< Variable >* stack );
        static const std::string lookupValue( const std::string varname, const Stack< Variable >* stack );

        static bool saveValueToStack( const std::string varname, const std::string value, Stack< Variable >& stack );

        static bool set_in( std::string& str, const PrefixSuffix& presuf
                          , const Variable& var, const Stack< Variable >* stack = nullptr );
    };

    std::string& setIn( const std::string& str, const PrefixSuffix& presuf, const Stack< Variable >* stack = nullptr );
};







} // namespace cacao

