
#include "cacao_vte.hpp"



namespace cacao {


Environment::Variable::Variable( const std::string var_str )
    :   std::string_view()
    ,   vartype( Type::undefined )
    ,   value()
    ,   tmpl()
{
    std::string vn = Str::replaceAll(var_str, "#", "", true);
    if ( vn.contains('{') ) {
        std::string* s = new std::string( vn.substr( 0, vn.find_first_of('{') ) );
        this->swap( *new std::string_view( *s ) );
    } else if ( vn.contains(' ') ) {
        std::string* s = new std::string( vn.substr( 0, vn.find_first_of(' ') ) );
        this->swap( *new std::string_view( *s ) );
    } else {
        this->swap( *new std::string_view( vn ) );
    }

    const unsigned int vt = Str::countprefix( var_str, "#" );
    switch ( vt )
    {
    case 1:
        vartype = Type::store;
        break;
    
    case 2:
        vartype = Type::read;
        break;
    case 3:
        vartype = Type::create;
        break;
    }

    Variable::fromCode( var_str, this );
}



Environment::Variable* Environment::Variable::fromCode( const std::string var_str
                                                      , Variable* variable 
) {
    if ( variable == nullptr ) {
        variable = new Variable( var_str );
    }

    const int is_there_a_formatter = Str::findr( var_str, "}{" );
    if ( is_there_a_formatter > -1 ) {
        variable->tmpl = new Template( *variable
            , var_str.substr( is_there_a_formatter
                            , var_str.length() - 2 // var strings will end with "}{...}"
                               - is_there_a_formatter + 2
        ) );
    }

    const int is_there_a_value = Str::find( var_str, "{" );
    if ( is_there_a_value > -1 ) {
        const int pos_value_end = Str::find( var_str, "}" ) - 1;
        variable->value = var_str.substr( is_there_a_value + 1, pos_value_end - is_there_a_value + 1 );
    } 

    return variable;
}


Range* Environment::Variable::findVariable( const std::string str
                                    , const unsigned int start
                                    , const int end
) {
    const int pos_var_start = Str::find( str, "#", start, end );
    if ( pos_var_start > -1 ) {
        const int has_tmpl = Str::find( str, "}{", pos_var_start, end );
        int pos_var_end = 0;
        if ( has_tmpl > -1 ) {
            pos_var_end = 1 + Str::find( str, "}", has_tmpl+2, end );
        } else {
            pos_var_end = 1 + Str::find( str, "}", pos_var_start, end );
        }
        return new Range( pos_var_start, pos_var_end
                    , str.substr(pos_var_start, pos_var_end-pos_var_start)
        );
    }
    return nullptr;
}


Environment::Variable::varenv_t* Environment::Variable::findVariables(
                                          const std::string str
                                        , const unsigned int start
                                        , const int end
) {
    range_list_t* rng_list = new range_list_t();
    unsigned int pos_start_next_variable = start;
    for ( Range* rng = findVariable( str, pos_start_next_variable, end )
        ; rng != nullptr; pos_start_next_variable = rng->end + 1
    ) {
        rng_list->push_back( rng );
    }

    Environment::Stack* varenv = new Environment::Stack();
    for ( Range* rng : *rng_list ) {
        Variable* variable = new Variable( *rng );
        varenv->addvar( variable );
    }

    return new varenv_t( varenv, rng_list );
}


Environment::Variable::operator std::basic_string<char>() const {
    return *this;
}


Environment::Template::operator std::basic_string<char>() const {
    return substr();
}


Environment::Template::Template( const std::string tmpl_name, const std::string tmpl_str )
    :   std::string( tmpl_str )
    ,   name( tmpl_name )
{}

Environment::Template* Environment::Template::fromCode( const std::string tmpl_str ) {
    int pos_tmpl_name = Str::find( tmpl_str, ">>" );
    if ( pos_tmpl_name > -1 ) {
        const int pos_name_linebreak = Str::find( tmpl_str, "\n", pos_tmpl_name );
        pos_tmpl_name += 2;
        std::string tmpl_name = tmpl_str.substr(pos_tmpl_name, pos_name_linebreak-1-pos_tmpl_name);
        return new Template( tmpl_name, tmpl_str );
    }
    return new Template( "", tmpl_str );
}


Environment::Template* Environment::Template::fromFile( const PathLike path ) {
    File* fd = new File( path.data() );
    return fromCode( fd->read().data() );
}


Range* Environment::Template::findContentUntil( const std::string str
                , const Range* var_range_current
                , const Range* var_range_next
) {
    // Find the content between the two variables current and next
    const unsigned int pos_variable_end = 1 + var_range_current->end;
    const int pos_next_variable = Str::find( str, "#", pos_variable_end + 1 );
    std::string content_between = str.substr(pos_variable_end + 1, pos_next_variable-pos_variable_end+1);

    // Now the left and the right delimiter is set. Find its position in text
    Range* delim_tmpl = Str::find_delim_in_tmpl( str, var_range_current, var_range_next );

    // Find out with what the var_range_current is delimited
    // We're searching for the characters left and right of var_range_current and left of var_range_next
    unsigned int start = 0;
    if ( var_range_current->start <= 0 ) {
        start = 0;
    } else {
        start = var_range_current->start - 1;
    }

    // We have content, that delimits the following content
    Range* delim_rng_before = nullptr;
    Range* delim_rng_between = nullptr;
    if ( var_range_current->length() > 0 ) {
        delim_rng_before = new Range(var_range_current->end, var_range_current->end, *var_range_current);
    } else if ( var_range_current->start == 0 ) {
        delim_rng_before = new Range( 0, 0, "" );
    } else if ( str.substr(start, var_range_current->start - start + 1)
                    .rfind( "} ", 2 ) != std::string::npos
    ) { // We have a delimiter before in out template
        // Find out the end of the precending variable
        const int pos_var_end_before = Str::findr( str, "} ", var_range_current->start - 1 );
        delim_rng_between = new Range( pos_var_end_before + 1, var_range_current->start - 1
                                 , str.substr( pos_var_end_before + 1, var_range_current->start - pos_var_end_before - 2 )
        );
        delim_rng_before = new Range( var_range_current->end + 1, var_range_current->end + 1, *delim_rng_between );
    } else { // If there is a delimiting string before, use it. Else find out the template content of the variable
        const int pos_var_content_end = var_range_current->start - 1;
        const int pos_var_end_before = Str::findr( str, "} ", var_range_current->start - 1 );
        Range* var_range_current_new = new Range( pos_var_end_before, pos_var_content_end
                    , str.substr( pos_var_end_before, pos_var_content_end - pos_var_end_before + 1)
        );

        delim_rng_before = findContentUntil( str, var_range_current_new, var_range_current );
    }

    // Now the left and the right delimiter is set. Find its position in text
    const int pos_delim_left = Str::find( str, delim_rng_before->data(), 0, Str::findr( str, delim_tmpl->data() ) );
    const int pos_delim_right = Str::findr( str, delim_tmpl->data(), 0, pos_delim_left );

    // Return everything from the left to the right delimiter
    return new Range( pos_delim_left + 1, pos_delim_right - 1, str.substr( pos_delim_left + 1, pos_delim_right - pos_delim_left - 2));
}



Range* Environment::Template::findContentUntilDelim( const std::string str
                                    , const char* delim
) {
    std::string delimiter( delim );

    // If delim.length is 0, we're searching for all content until the first delimiter
    if ( delimiter.length() == 0 ) { // All content, including content of a variable until the next variable counts
        const int pos_next_hashtag = Str::find( str, "#{" );
        if ( pos_next_hashtag < 0 ) {
            return new Range( 0, str.length(), str );
        }

        Variable::varenv_t* var_rng_l = Variable::findVariables( str, pos_next_hashtag );
        std::string result = str.substr( 0, pos_next_hashtag - 1 );

        unsigned int idx = 0;
        for ( typename Environment::Stack::iterator it = var_rng_l->first->begin()
            ; it != var_rng_l->first->end(); ++it, idx++
        ) {
            Variable* var = it->second;
            result += *var;
            if ( idx + 1 < var_rng_l->first->size() ) {
                result += *findContentUntil( str
                                , var_rng_l->second->at(idx)
                                , var_rng_l->second->at(idx+1)
                );
            } else {
                result += str.substr( var_rng_l->second->at(idx)->end );
            }

            return new Range( pos_next_hashtag, pos_next_hashtag+result.length(), result);
        }
    } else if ( const int pos_delim = Str::find( str, delim ) > -1 ) {
        return new Range( 0, pos_delim, str.substr( 0, pos_delim-1 ) );
    }
    return new Range( 0, 0, "" );
}



std::string Environment::Template::templateFormtter( Environment::Stack* stack
                        , const Variable* current_variable
                        , const std::string var_value
) {
    Template* tmpl = new Template( *current_variable, current_variable->tmpl->substr() );
    return tmpl->transform( var_value, stack );
}


Environment::Template::env_variable_list_t* Environment::Template::read() const {
    env_variable_list_t* var_list = new env_variable_list_t();

    Str::token_list_t lane_list = Str::splitat( "\n", *this );
    for ( unsigned int l = 0; l < lane_list.size(); l++ ) {
        Variable::varenv_t* var_rng_l = Variable::findVariables( lane_list.at( l ) );

        unsigned int r = 0;
        for ( Range* rng = var_rng_l->second->at(r)
            ; r < var_rng_l->second->size(); r++
        ) {
            Variable* var = new Variable( *rng );
            const unsigned int pos_tmpl_snippet = 2 + Str::find( *rng, "}{" );
            if ( pos_tmpl_snippet > 1 ) {
                Template* tmpl = fromCode( rng->substr( pos_tmpl_snippet
                                                    , rng->size() - 2 - pos_tmpl_snippet ) );
                var->tmpl = tmpl;
            }
        }
    }

    return var_list;
}


std::string Environment::Template::transformTo( Environment::Stack* stack, const Template* tmpl2 ) {
    return transform( *tmpl2, stack ); 
}


std::string Environment::Template::transform( const std::string str
                , Environment::Stack* stack
) {
    Variable::varenv_t* var_rng_l = Variable::findVariables( *this );

    std::string result;
    unsigned int idx = 0;
    for ( Range* rng = var_rng_l->second->at( idx )
        ; idx < var_rng_l->second->size(); idx++
    ) {
        Variable* var = var_rng_l->first->pos( idx );
        Range* current_rng = findContentUntilDelim( str, var->value.c_str() );

        if ( var->vartype == Variable::store ) {
            stack->push(*var, new Variable( *current_rng ) );

            if ( var->tmpl != nullptr ) {
                result += "###" + std::string(*var) + "{}" + this->templateFormtter(stack, var, var->value);
            } else {
                result += "###" + std::string(*var) + "{}";
            }
        }
    }

    return result;
}


void Environment::Template::readToEnv( Environment::Stack* env ) {
    env_variable_list_t* vars = read();
    for ( unsigned int i = 0; i < vars->size(); i++ ) {
        env->addvar( new Variable( *vars->at(i) ) );
    }
}



Environment::Stack::Stack( Config* configuration )
    :   cacao::Stack< Environment::Variable >( "env_stack", *configuration )
{}

void Environment::Stack::addvar( Environment::Variable* variable ) {
    stack_t::at( *variable ) = variable;
}


void Environment::Stack::addvar( const std::string key, Environment::Variable* value ) {
    stack_t::at( key ) = value;
}

Environment::Variable*& Environment::Stack::get( const std::string key ) {
    if ( stack_t::contains( key ) ) {
        return stack_t::at( key );
    }
    Variable* var = new Variable( "#" + key );
    addvar( var );
    return stack_t::at( key );
}

std::string Environment::Stack::getValue( const std::string key ) {
    if ( ! stack_t::contains( key ) ) {
        return "";
    }
    return stack_t::at( key )->value;
}


} // namespace cacao

