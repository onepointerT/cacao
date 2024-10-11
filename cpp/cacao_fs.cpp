
#include "cacao_fs.hpp"

#include <fstream>

#include "cacao_str.hpp"


namespace cacao {



PathLike::PathLike( const std::string path )
    :   std::string_view( path )
{}


PathLike::PathLike( const PathLike& other )
    :   std::string_view( other.substr() )
{}


void PathLike::join( const std::string pjoin ) {
    std::string p = this->repr() + '/' + pjoin.data();
    std::string_view* sv = new std::string_view( p.begin(), p.end() );
    this->swap( *sv );
}


void PathLike::operator/( const std::string pjoin ) {
    this->join( pjoin );
}


PathLike::path_elements_t PathLike::path_elements() const {
    return Str::splitat( "/", this->data(), true );
}


std::filesystem::path PathLike::path() const {
    std::filesystem::path p( this->repr() );
    return p;
}


const std::string PathLike::repr() const {
#ifdef _MSC_BUILD 
    return Str::replaceAll( this->data(), "/", "\\" );
#else
    return this->data();
#endif
}


bool PathLike::valid() const {
    return PathLike::is_valid_path(*this);
}


bool PathLike::is_valid_path( const PathLike& p ) {
#ifdef _MSC_BUILD
    return p[1] == ':' ? true : p.find("$") != p.npos;
    // Labeled drive or network mount count as valid path.
#else
    return p[0] == '/' ? true : p.find(":///") != p.npos;
    // Unix path or POSIX-like path like smb:/// or file:///
#endif
}


bool PathLike::is_valid_path( const std::string& p ) {
    PathLike pl( p );
    return PathLike::is_valid_path( pl );
}




Path::Path( const std::string p )
    :   PathLike( p )
{}


Path::Path( const Path& p )
    :   PathLike( p )
{}


bool Path::exists() const {
    return std::filesystem::exists( this->repr() );
}


bool Path::copy( PathLike& to_target ) const {
    std::error_code ec;
    if ( std::filesystem::is_directory(to_target.repr())
      || (this->is_regular_file() && std::filesystem::is_regular_file(to_target.repr()) )
    ) {
        std::filesystem::copy( this->repr(), to_target.repr(), ec );
        return !ec;
    }
    return false;
}


void Path::rename( PathLike& pl ) {
    std::filesystem::rename( this->repr(), pl.repr() );
    this->swap( pl );
}


const file_status Path::status() const {
    return std::filesystem::status( this->repr() );
}


permissions Path::get_permissions() const {
    return this->status().permissions();
}


bool Path::set_permissions( permissions perms ) {
    std::error_code ec;
    std::filesystem::permissions( this->repr(), perms, ec );
    return !ec;
}


bool Path::is_regular_file() const {
    return std::filesystem::is_regular_file( this->status() );
}


bool Path::is_directory() const {
    return std::filesystem::is_directory( this->status() );
}





File::File( const std::string p )
    :   Path( p )
{}


file_size File::fsize() const {
    return std::filesystem::file_size( this->repr() );
}


bool File::resize( const file_size size_bytes_new ) const {
    std::error_code ec;
    std::filesystem::resize_file( this->repr(), size_bytes_new, ec );
    return !ec;
}


const std::string_view File::read(
  file_openmode mode  ) const {
    std::string* s = new std::string();
    std::fstream fstr( this->repr(), std::ios_base::in | mode );
    fstr >> *s;
    fstr.close();
    return *(new std::string_view( *s ));
}


void File::write( const std::string_view s,
  file_openmode mode ) const {
    std::fstream fstr( this->repr(), std::ios_base::out | mode );
    fstr << s << '\0';
    fstr.close();
}


File& File::operator<<( const std::string_view in ) {
  this->write( in, std::ios_base::app );
  return *this;
}


File& File::operator>>( std::string out ) {
  out.append( this->read( std::ios_base::in ) );
  return *this;
}




DirectoryContentList Directory::scan_directory( const Path& p, const bool recursive ) {
    DirectoryContentList dircontent;
    for ( const std::filesystem::directory_entry& direntry
            : std::filesystem::recursive_directory_iterator( p.repr() )
    ) {
        if ( direntry.is_directory() ) {
            Directory* dir = new Directory( direntry.path().string() );
            dircontent.insert( DirectoryContent( *dir ) );
            if ( recursive ) {
                DirectoryContentList recursive_dircontent
                    = Directory::scan_directory( dir->path().string(), true );
                dircontent.insert_range( recursive_dircontent );
            }
        } else if ( direntry.is_regular_file() ) {
            File* file = new File( direntry.path().string() );
            dircontent.insert( DirectoryContent( *file ) );
        } else {
            Path* path = new Path( direntry.path().string() );
            dircontent.insert( DirectoryContent( *path ) );
        }
    }
    return dircontent;
}


Directory::Directory( const std::string p )
    :   Path( p )
    ,   m_contents()
{}


size_t Directory::scandir( const bool recursive ) {
    m_contents = Directory::scan_directory( *this, recursive );
    return m_contents.size();
}


const DirectoryContentList Directory::contents() const {
    return m_contents;
}


DirectoryContentList::DirectoryContentList()
    :   std::set< DirectoryContent >()
{}



DirectoryContentList::DirectoryContentList( const DirectoryContentList& dcl )
    :   std::set< DirectoryContent >( dcl )
{}


DirectoryContentList DirectoryContentList::get_files() const {
    DirectoryContentList dircontent_files;
    for ( const_iterator it = this->cbegin(); it != this->cend(); ++it ) {
        if ( it->get_file() != nullptr ) {
            dircontent_files.insert( *it );
        }
    }
    return dircontent_files;
}


DirectoryContentList DirectoryContentList::get_directories() const {
    DirectoryContentList dircontent_dirs;
    for ( const_iterator it = this->cbegin(); it != this->cend(); ++it ) {
        if ( it->get_directory() != nullptr ) {
            dircontent_dirs.insert( *it );
        }
    }
    return dircontent_dirs;
}


const std::string DirectoryContent::pathstr() const {
    return repr().substr();
}


const Path DirectoryContent::path() const {
    return Path( pathstr() );
}


Directory* DirectoryContent::get_directory() const {
    return new Directory( substr().data() );
}

File* DirectoryContent::get_file() const {
    return new File( substr().data() );
}

Path* DirectoryContent::get_path() const {
    return new Path( substr().data() );
}


int DirectoryContent::get_type() const {
    return static_cast<int>(type);
}


bool DirectoryContent::is( const DirectoryContent::Type dct ) const {
    return static_cast<int>(dct) == this->get_type();
}


DirectoryContent::Type DirectoryContent::is_type( const DirectoryContent& dc ) {
    if ( dc.get_type() > 0 ) {
        return static_cast<DirectoryContent::Type>( dc.get_type() );
    }
    return DirectoryContent::Type::GenericPath;
}

auto DirectoryContent::operator<=>(const DirectoryContent& rhs) const {
    return this->path().repr() <=> rhs.path().repr();
}

auto DirectoryContent::operator<=>(const DirectoryContent*rhs) const {
    return this->path().repr() <=> rhs->path().repr();
}

DirectoryContent::operator std::basic_string<char>() const {
    return this->path().repr();
}




} // namespace cacao


cacao::PathLike operator/( const cacao::PathLike& lhs, const cacao::PathLike& rhs ) {
    cacao::PathLike pl( lhs );
    pl.join( rhs.data() );
    return pl;
}


cacao::PathLike operator/( const cacao::PathLike& lhs, const std::string& rhs ) {
    cacao::PathLike pl( lhs );
    pl.join( rhs );
    return pl;
}


cacao::PathLike operator/( const std::string& lhs, const std::string& rhs ) {
    cacao::PathLike pl( lhs );
    pl.join( rhs );
    return pl;
}
