
#include "cacao_vte.hpp"



namespace cacao {


Environment::Variable::Type Environment::Variable::getType( const std::string_view var ) {
    if ( var.starts_with("###") ) return Variable::Type::create;
    else if ( var.starts_with("##") ) return Variable::Type::read;
    else if ( var.starts_with("#") ) return Variable::Type::store;
    else return Variable::Type::undefined;
}

Environment::Variable::Variable( const std::string var_str
        , const std::string tmpl
        , const std::string actions
        , const std::string formatter
)   :   std::string_view( var_str )
    ,   vartype( getType(*this) )
    ,   value()
    ,   tmpl( Environment::Template::fromCode( tmpl ) )
    ,   actions( Environment::Template::fromCode( actions ) )
    ,   formatter( Environment::Template::fromCode( formatter ) )
{}

Environment::Variable::Variable( const std::string var_str
        , const Template* tmpl_tmpl
        , const Template* tmpl_actions
        , const Template* tmpl_formatter
)   :   std::string_view( var_str )
    ,   vartype( getType(*this) )
    ,   value()
    ,   tmpl( tmpl_tmpl )
    ,   actions( tmpl_actions )
    ,   formatter( tmpl_formatter )
{}


Environment::Variable* fromRegex( const std::string rgx
                   , const std::string var_str
                   , const RegexTypeCacao rtype_cacao ) {

    RegexCacao* rgxc = new RegexCacao( rgx, toRegexType(rtype_cacao) );

    RegexCacao::Match& m = rgxc.
}


Environment::Variable* Environment::Variable::fromRegex( const RegexCacao::Match& var_rgxm
                                                       , const RegexTypeCacao rtype_cacao) {
    Environment::Variable* var = new Environment::Variable( var_rgxm.str() );
    var->value = var_rgxm.fit_in( var_rgxm.str(), *var, toRegexConstants( toRegexType(rtype_cacao) ) ).str();
}

Environment::Variable* Environment::Variable::fromCode( const std::string var_str ) {

}

Range* findVariable( const std::string str
                    , const unsigned int start = 0
                    , const int  end = -1
) {

}


Environment::Variable::varenv_pos_t* Environment::Variable::findVariables( const std::string str
                            , const unsigned int start = 0
                            , const int  end = -1
        ) {

}


Environment::Variable::operator std::basic_string<char>() const {

}



Environment::Template::Template( const std::string tmpl_name, const std::string tmpl_str
                , const Template::Type ttype
        ) {

}


Environment::Template* Environment::Template::fromRegex( const RegexCacao::Match& tmpl_rgxm
                            , Template* tmpl ) {

}

Environment::Template* Environment::Template::fromCode( const std::string tmpl_str
                            , Template* tmpl ) {

}
 {

}

Environment::Template* Environment::Template::fromFile( const PathLike path
                            , Template* tmpl ) {

}

Range* Environment::Template::findContentUntil( const std::string str
                , const Range* var_range_current
                , const Range* var_range_next
) {

}

Range* Environment::Template::findContentUntilDelim( const std::string str
                , const char* delim
) {

}

std::string Environment::Template::templateFormatter( Stack* stack
                        , const Variable* current_variable
                        , const std::string var_value
) {

}


Environment::env_variables_list_t* Environment::Template::parse() const {

}

std::string Environment::Template::transformTo( Stack* stack, const Template* tmpl2 ) {

}

std::string Environment::Template::transform( const std::string str
                , Stack* stack
) {

}

void Environment::Template::readToEnv( Stack* env ) const {

}

void Environment::Template::readVars( Stack* env ) const {

}


Environment::Template::operator std::basic_string<char>() const {

}


void Environment::Template::set( const std::string tmpl_str_update ) {

}




Environment::Stack::Stack( Config* configuration ) {

}


Environment::Stack* readEnvVarList( const env_variables_list_t& evl
                            , Config* configuratation ) {

}

void Environment::Stack::readToStack( const env_variables_list_t& evl ) {

}

void Environment::Stack::addvar( Environment::Variable* variable ) {

}

void Environment::Stack::addvar( const std::string key, Environment::Variable* value ) {

}


Environment::Variable*& Environment::Stack::get( const std::string key ) {

}

std::string Environment::Stack::getValue( const std::string key ) {

}


Environment::Variable* Environment::Stack::getPos( const unsigned int pos ) {

}


}
 // namespace cacao

