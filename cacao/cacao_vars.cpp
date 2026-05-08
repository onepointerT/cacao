
#include "cacao_vars.hpp"


#include "cacao_str.hpp"


namespace cacao {


Variable::Type Variable::getType( const std::string_view var_str ) {
    if ( var_str.starts_with("###") ) return Variable::Type::create;
    else if ( var_str.starts_with("##") ) return Variable::Type::read;
    else if ( var_str.starts_with("#") ) return Variable::Type::store;
    else if ( var_str.starts_with("$") ) return Variable::Type::use;
    else return Variable::Type::undefined;
}

bool Variable::is_variable( const std::string_view var_str ) {
    return Variable::getType(var_str) != Variable::Type::undefined;
}

const std::string Variable::getVarName( const std::string_view var_str, const size_t pos_start ) {
    const RegexCacao::Match& m = regex_detect_varname->matchOnce(var_str.data());
    if ( m.str().size() == 0 ) {
        size_t pos_varname_start = var_str.find_first_of( " $", pos_start );
        if ( pos_varname_start == var_str.npos ) pos_varname_start = var_str.find_first_of( '$', pos_start );
        if ( pos_varname_start != var_str.npos ) { // Lookup variable
            size_t pos_varname_end = var_str.find_first_of( ", ", pos_varname_start );
            if ( pos_varname_end == var_str.npos ) pos_varname_end = var_str.find_first_of( ' ', pos_varname_start );
            if ( pos_varname_end == var_str.npos ) pos_varname_end = var_str.find_first_of( '#', pos_varname_start );
            if ( pos_varname_end == var_str.npos ) return "";
            return var_str.substr( pos_varname_start, pos_varname_end - 1 - pos_varname_start ).data();
        } else return "";
    } return m.str();
}

const std::string Variable::getVarNameFulfilled( const std::string_view var_str, const Variable::var_env_t& intern
                                    , const Stack< Variable >* stack, const size_t pos_start ) {
    
    std::string_view sv = var_str;
    if ( ! var_str.starts_with( "$" ) ) {
        sv = Variable::getVarName( var_str );
        if ( sv.length() == 0 ) return "";
    }

    size_t pos_main_variable = var_str.find_first_of( sv.data() );
    if ( pos_main_variable == var_str.npos ) {
        const RegexCacao::Match& m_varname = regex_detect_varname->matchOnce( var_str.data() );
        if ( m_varname.length() > 0 ) {
            sv = m_varname.str();
            pos_main_variable = var_str.find_first_of( sv );
            if ( pos_main_variable == var_str.npos ) return "";
        } else return "";
    }

    // Now fulfill all unknown variables inside of the variable name
    size_t pos_next_variable = var_str.find_first_of( '$', pos_main_variable + 1 );
    unsigned short var_index = 2;
    unsigned short var_index_before = 1;
    std::list< cacao::Range* > var_insight_where;
    // As long as there is a new variat found inside of the `var_str` behind `pos_start`
    while ( pos_next_variable != var_str.npos ) {
        Str::Pattern* use_variat_prefix = new Str::Pattern( "$", "#", "\\s" );
        const typename Str::position_list_t& pos_list = use_variat_prefix->find( var_str.data() );
        if ( pos_list.size() == 0 ) return Variable::getVarName( sv, pos_main_variable );

        for ( typename Str::position_list_t::const_reverse_iterator plIt : { pos_list.rbegin(), pos_list.rend() } ) {
            pos_next_variable = plIt->start;
            const size_t len_next_var = plIt->length();
            var_insight_where.push_front( new cacao::Range(pos_next_variable, pos_next_variable+len_next_var
                                                , var_str.substr(pos_next_variable, len_next_var) ) );
        }

        break;
    } // All variables found

    // Now let's set them in
    Str::Trajection::List& var_trajs = *new Str::Trajection::List();
    if ( var_insight_where.size() > 0 ) {
        for ( const Range* rng : var_insight_where ) {
            const std::string varname = rng->substr( 1, rng->size() - 2 ); // variat variable names are like `...$varname#...`
            Variable* var = nullptr;
            if ( intern.contains(varname) ) const Variable* var = intern.get(varname);
            else if ( stack != nullptr & stack->contains(varname) ) const Variable* var = stack->get(varname);

            Str::Trajection* var_traj = nullptr;
            if ( var == nullptr ) var_traj = new Str::Trajection( rng->substr(), "$$" );
            else {
                const std::string var_value = var->getFormatted( stack );
                var_traj = new Str::Trajection( rng->substr(), var_value );
            } var_trajs.push_front( var_traj );
        }
    }

    std::string* s_result = new std::string( sv );
    if ( var_trajs.size() > 0 ) var_trajs.refactorAllOccurrences( *s_result );

    return *s_result
}


const std::string Variable::getVarNamePure(const std::string_view var_str, const var_env_t& intern
                                        , const Stack< Variable >* stack = nullptr, const size_t pos_start ) {
    const std::string& varname_fulfilled = Variable::getVarNameFulfilled( var_str, intern, stack, pos_start );
    std::string_view varname_sv = varname_fulfilled;
    if ( varname_fulfilled.starts_with( '$' ) ) varname_sv.remove_prefix(1);
    if ( varname_fulfilled.ends_with( ", ") ) varname_sv.remove_suffix(2);
    else if ( varname_fulfilled.ends_with( ' ' ) || varname_fulfilled.ends_with( '#' ) ) varname_sv.remove_suffix(1);
    return varname_sv.data();
}

Variable::Variable( const std::string var_str
        , const std::string tmpl
        , const std::string tmpl_actions
        , const std::string tmpl_formatter
)   :   std::string_view( var_str )
    ,   vartype( getType(*this) )
    ,   env_intern( *new var_env_t("intern_env_var") )
    ,   name( getVarName(*this) )
    ,   value()
    ,   tmpl( new TemplateFindUp( *(Template::fromCode( tmpl )) ) )
    ,   actions( new TemplateActions( *(Template::fromCode( tmpl_actions )) ) )
    ,   formatter( new TemplateFormatter( *(Template::fromCode( tmpl_formatter )) ) )
{}

Variable::Variable( const std::string var_str
        , const Template* tmpl_tmpl
        , const Template* tmpl_actions
        , const Template* tmpl_formatter
)   :   std::string_view( var_str )
    ,   vartype( getType(*this) )
    ,   env_intern( *new var_env_t("intern_env_var") )
    ,   name( getVarName(*this) )
    ,   value()
    ,   tmpl( *tmpl_tmpl )
    ,   actions( *tmpl_actions )
    ,   formatter( *tmpl_formatter )
{}


Variable::Variable( const RegexCacao::Match& rgxm )
    :   std::string_view( rgxm.str() )
    ,   vartype( getType(*this) )
    ,   name( getVarName(*this) )
    ,   value()
    ,   tmpl( nullptr )
    ,   actions( nullptr )
    ,   formatter( nullptr )
{
    const RegexCacao::Match::List& mlbraces = regex_detect_braces->match( rgxm.str() );
    if ( vartype == Type::create ) {
        
    } else if ( vartype == Type::read ) {
        
    } else if ( vartype == Type::store ) {

    }
}


TemplateFindUp* Variable::makeTemplateFindUp( const RegexCacao::Match& rgxm_braces ) {
    const RegexCacao::Match& rgxm_bracecontent = regex_detect_inbraces->matchOnce( rgxm_braces.str() );
    if ( rgxm_bracecontent.empty() ) return nullptr;
    return new TemplateFindUp( rgxm_bracecontent.str() );
}

TemplateActions* Variable::makeTemplateActions( const RegexCacao::Match& rgxm_braces ) {

}

TemplateFormatter* Variable::makeTemplateFormatter( const RegexCacao::Match& rgxm_braces ) {

}


Variable* Variable::fromRegex( const std::string rgx
                   , const std::string var_str
                   , const RegexTypeCacao rtype_cacao ) {

    RegexCacao* rgxc = new RegexCacao( rgx, toRegexType(rtype_cacao) );

    const RegexCacao::Match& m = rgxc->matchOnce( var_str );
    return fromRegex( m, rtype_cacao );
}


Variable* Variable::fromRegex( const RegexCacao::Match& var_rgxm
                                                       , const RegexTypeCacao rtype_cacao) {
    const std::string_view var_str = var_rgxm.str();

    if ( ! Variable::is_variable( var_str ) ) {
        // # TODO Exceptionally find the variable, e.g. inside of ' '
    }


}

Variable* Variable::fromCode( const std::string var_str ) {

}

Range* findVariable( const std::string str
                    , const unsigned int start
                    , const int  end
) {

}


Variable::varenv_pos_t* Variable::findVariables( const std::string str
                            , const unsigned int start
                            , const int  end
) {
    // # TODO
}


Variable::operator std::basic_string<char>() const {
    return this->getValue();
}


const std::string Variable::getSelection( const Stack< Variable >* stack ) const {
    if ( this->tmpl == nullptr ) return this->value;
    return this->tmpl->assembleTemplate( this->value, *stack ).data();
}

const std::string Variable::getAction( const Stack< Variable >* stack ) const {
    const std::string selection_str = this->getSelection( stack );
    if ( this->actions == nullptr ) return selection_str;
    return this->actions->assembleTemplate( selection_str, *stack ).data();
}

const std::string Variable::getFormatted( const Stack< Variable >* stack ) const {
    if ( this->formatter == nullptr ) return this->getAction( stack );
    return this->formatter->assembleTemplate( this->getAction(stack), *stack ).data();
}


const std::string Variable::getValue( const Stack< Variable >* stack ) const {
    if ( stack == nullptr ) return this->value;
    return this->getFormatted( stack );
}


const Variable* Variable::util::lookup( const std::string varname, const Stack< Variable >* stack ) {
    return stack->get(varname);
}


const std::string Variable::util::lookupValue( const std::string varname, const Stack< Variable >* stack ) {
    const Variable* var = stack->get(varname);
    if ( var == nullptr ) return "";
    return var->getValue( stack );
}

bool Variable::util::saveValueToStack( const std::string varname, const std::string value, Stack< Variable >& stack ) {
    Variable* var = stack.get(varname);
    if ( var == nullptr ) return false;
    Variable* var_stack = stack.get(varname);
    if ( var_stack == nullptr ) {
        var_stack = new Variable( varname, value );
        stack.push( varname, var_stack );
    } else {
        var_stack->value = value;
    }
    return true;
}

bool Variable::util::set_in( std::string& str, const PrefixSuffix& presuf
                           , const Variable& var, const Stack< Variable >* stack
) {
    const std::string var_value = Variable::util::lookupValue( var.name, stack );

    const size_t pos_prefix_end = str.find_first_of( *presuf.first ) + presuf.first->length();
    const size_t pos_suffix_start = str.find_first_of( *presuf.second );
    size_t pos_var_start = pos_prefix_end == str.npos ? 0 : pos_prefix_end + 1;
    size_t count_var_end = pos_suffix_start == str.npos ? str.npos : pos_suffix_start - pos_prefix_end;
    
    str.replace( pos_var_start, count_var_end, var_value );
    return true;
}


std::string& Variable::setIn( const std::string& str, const PrefixSuffix& presuf, const Stack< Variable >* stack ) {
    std::string* str_replacement = new std::string( str );
    if ( ! Variable::util::set_in( *str_replacement, presuf, *this, stack ) ) return *new std::string(str);
    return *str_replacement;
}


} // namespace cacao

