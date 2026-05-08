
#include "cacao_tmpl.hpp"

#include <cstring>

#include "cacao_vars.hpp"


namespace cacao {


void Template::init( const Stack< Variable >& intern_stack_of_var ) {
    this->set( *this );
    env_variables_list_t* tmpl_vars = this->parse();
    this->vars.assign( tmpl_vars->begin(), tmpl_vars->end() );
    this->make( intern_stack_of_var );
}


Template::Template( const std::string tmpl_name, const std::string tmpl_str
                  , const Template::Type ttype )
    :   std::string_view( tmpl_str )
    ,   tmpltype( ttype )
    ,   vars( *new Template::env_variables_list_t() )
    ,   name( tmpl_name )
{}


Template* Template::fromRegex( const RegexCacao::Match& tmpl_rgxm ) {
    
}

Template* Template::fromCode( const std::string tmpl_str ) {

}

Template* Template::fromFile( const PathLike path ) {

}


Template::env_variables_list_t* Template::parse() const {

}


void Template::readToEnv( Stack< Variable >* env ) const {

}

void Template::readVars( Stack< Variable >* env ) const {

}


Template::operator std::basic_string<char>() const {

}


void Template::set( const std::string tmpl_str_update ) {

}


const Range& Template::util::findEnclosedContent( const std::string_view cacao_str_part, const unsigned int element_idx
                                                , const size_t search_start_pos, const size_t search_end_pos
) {
    std::string_view sv = cacao_str_part;
    if ( search_end_pos != 0 && search_end_pos < cacao_str_part.length() && search_start_pos < cacao_str_part.length() ) {
        sv = cacao_str_part.substr( search_start_pos, search_end_pos - search_start_pos );
    } else if ( search_start_pos != 0 && search_end_pos < cacao_str_part.length() && search_start_pos < cacao_str_part.length() ) {
        if ( search_end_pos > 0 ) sv = cacao_str_part.substr( search_start_pos, search_end_pos - search_start_pos );
        else sv = cacao_str_part.substr( search_start_pos );
    }

    size_t pos = 0;
    size_t pos_found = 0;
    for ( unsigned int elem_idx = 0; elem_idx <= element_idx; elem_idx ) {
        std::string_view sv_sub;

        for ( typename std::string_view::const_iterator sIt : { sv.cbegin(), sv.cend() } ) {
            if ( *sIt == '_' ) {
                if ( pos < sv.length() - 5 ) {
                    sv_sub = sv.substr( pos, 2 );
                    if ( sv_sub.starts_with( "_#" ) ) {
                        pos_found = pos;
                        break;
                    }
                } if ( pos < sv.length() - 7 ) {
                    sv_sub = sv.substr( pos, 3 );
                    if ( sv_sub.starts_with( "__#" ) ) {
                        pos_found = pos;
                        break;
                    }
                }
            }
            ++pos;
        }

        if ( elem_idx < element_idx ) continue;
        else {
            size_t pos_end = sv_sub == "__#" ? sv.find_first_of( "#__", pos_found )
                                             : sv.find_first_of( "#_", pos_found );

            return *new Range( pos_found, pos_end, sv.substr(pos_found, pos_end + (sv_sub == "__#" ? 3 : 2)).data() );
        }
    }

    return *new Range( 0, 0, sv.data() );
}



const Template::util::EnclosedContentVariant& Template::util::parseEnclosedContent( const std::string_view cacao_str_part ) {
    EnclosedContentVariant* ecv = new EnclosedContentVariant();
    if ( cacao_str_part.starts_with("#") ) {
        Variable* var = new Variable( cacao_str_part.data() );
        ecv->emplace<Variable*>( var );
    } else if ( cacao_str_part.contains('^') ) {
        RegexCacao* rgxc = new RegexCacao( cacao_str_part.data(), RegexType::ICase );
        ecv->emplace<RegexCacao*>( rgxc );
    }
    return *ecv;
}

const Template::util::ContentTuple* Template::util::parseContentTuple( const std::string_view cacao_str_part
                                                                     , const unsigned int element_idx
                                                                     , const size_t search_start_pos
                                                                     , const size_t search_end_pos 
) {
    const Range& rng_content = Template::util::findEnclosedContent( cacao_str_part, element_idx, search_start_pos, search_end_pos );
    if ( rng_content.start == 0 && rng_content.end == 0 ) return nullptr;
    
    const std::string prefix = rng_content.prefix();
    const std::string encl_content = rng_content.str();
    const std::string suffix = rng_content.suffix();

    const size_t pos_prefix_start = cacao_str_part.find_first_of( prefix, search_start_pos );
    const size_t pos_prefix_end = cacao_str_part.find_first_not_of( prefix, search_start_pos );
    const size_t pos_suffix_start = cacao_str_part.find_first_of( suffix, search_start_pos );
    const size_t pos_suffix_end = cacao_str_part.find_first_not_of( suffix, search_start_pos );

    const EnclosedContentVariant& ecv = Template::util::parseEnclosedContent( encl_content );
    EnclosingContent* ec_prefix = new EnclosingContent( prefix, cacao_str_part.data(), pos_prefix_start, pos_prefix_end, EnclosingContent::Prefix );
    EnclosingContent* ec_suffix = new EnclosingContent( suffix, cacao_str_part.data(), pos_suffix_start, pos_suffix_end, EnclosingContent::Suffix );

    return new Template::util::ContentTuple( { new PrefixSuffix( ec_prefix, ec_suffix ), &ecv } );
}

const Template::util::ContentList& Template::util::findEnclosedElements( const std::string cacao_str ) {

    ContentList* cl = new ContentList();

    for ( unsigned int found_elements = 0; ; ) {
        Template::util::ContentTuple* ct = Template::util::parseContentTuple( cacao_str, found_elements );
        if ( ct == nullptr ) break;
        else {
            ++found_elements;
            cl->push_front( ct );
        }
    }

    return *cl;
}



TemplateFindUp::TemplateFindUp( const std::string tmpl_str )
    :    Template( genuuidv4(), tmpl_str, Template::SelectionRegex )

{
    this->init();
}

TemplateFindUp::TemplateFindUp( const RegexCacao::Match& tmpl_rgxm )
    :   Template( Template::fromRegex(tmpl_rgxm) )
{
    this->init();
}


const std::string_view TemplateFindUp::assembleTemplate( const std::string str, const Stack< Variable >& stack ) const {

    Template::util::ContentList& cl_elements = Template::util::findEnclosedElements( this->substr().data() );

    std::string_view sv = this->substr();
    std::string sv_tmpl = this->substr().data();
    size_t additional_characters = 0;
    for ( typename Template::util::ContentList::const_iterator eIt : { cl_elements.cbegin(), cl_elements.cend() } ) {
        Template::util::ContentTuple* content = *eIt;

        // Unpack Content and prefix/suffix
        PrefixSuffix* presuf = std::get<0>(*content);
        util::EnclosedContentVariant* ecv = std::get<1>(*content);

        // Unpack variant
        RegexCacao* rgxc = nullptr;
        Variable* var = nullptr;
        if ( ecv->valueless_by_exception() ) continue;
        else if ( ecv->index() == 0 )
            rgxc = std::get<RegexCacao*>(*ecv);
        else if ( ecv->index() == 1 )
            var = std::get<Variable*>(*ecv);

        // If it is a regex, we just need to implace it for FindUp-Templates
        if ( rgxc != nullptr  ) {
            if ( ! Str::replace( sv_tmpl, *presuf, rgxc->getRegex( stack ).data() ) ) continue;
        } else if ( var != nullptr ) {
            if ( ! Variable::util::set_in( sv_tmpl, *presuf, *var, &stack ) ) continue;
        }
    }

    return sv_tmpl;
}

const std::string_view TemplateFindUp::findIn( const std::string str, const Stack< Variable* >& stack ) {
    // #TODO RegexCacao::Match& rgxm = 
}



TemplateFormatter::util::FormatInserter::FormatInserter( const std::string format_printf_str, const Variable& var )
    :   std::string_view( format_printf_str )
    ,   Variable( var )
{}

const std::string TemplateFormatter::util::FormatInserter::str() const {
    return std::string_view::data();
}

const Variable& TemplateFormatter::util::FormatInserter::var() const {
    return *this;
}

const std::string TemplateFormatter::util::FormatInserter::insertFormat( const Stack< Variable >* stack ) const {
    std::string* fstr = new std::string();
    if ( this->str().starts_with("%") ) {
        std::sprintf( fstr->data(), std::string_view::data(), this->getAction(stack) );
    } else if ( std::string_view::starts_with("_#") ) {
        const RegexCacao::Match& rgxm_varname = regex_detect_varname->matchOnce( std::string_view::data() );
        const std::string str_value = Variable::util::lookupValue( rgxm_varname.str(), stack );
        std::sprintf( fstr->data(), "%s", str_value );
    }
    return *fstr;
}

} // namespace cacao

