
#include "config.hpp"



namespace cacao {


StringDict::StringDict( const std::string dict_name )
    :   std::map< std::string, std::string >()
    ,   std::string( dict_name )
{}



Config::Config()
    :   std::map< std::string, StringDict* >()
{}


std::string& Config::config( const std::string dct_name, const std::string key ) {
    if ( this->find( dct_name ) != this->end() ) {
        StringDict* strdct = this->at( dct_name );
        if ( strdct->find(key) != strdct->end() ) {
            return strdct->at( key );
        } else {
            std::string* s = new std::string();
            (*strdct)[key] = *s;
            return *s;
        }
    } else {
        StringDict* strdct = new StringDict( dct_name );
        (*this)[dct_name] = strdct;
        std::string* s = new std::string();
        (*strdct)[key] = *s;
        return *s;
    }
}


void Config::operator+=( StringDict* strdct ) {
    (*this)[strdct->c_str()] = strdct;
}


} // namespace cacao

