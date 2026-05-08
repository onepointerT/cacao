
#include "cacao_vte.hpp"



namespace cacao {



Environment::Stack::Stack( Config* configuration )
    :   cacao::Stack< Variable >("env", *configuration)
{}


Environment::Stack* Environment::Stack::readEnvVarList( const env_variables_list_t& evl
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

