
// Copyright 2024 (C) The Abseil Authors, Cacao Authors.

#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


namespace cacao {



class PathLike
    :   public std::string_view
{
 public:
  // Elements between the path seperators like '/' and '\\' in an vector.
  typedef typename std::vector< std::string > path_elements_t;

  PathLike( const std::string path );
  PathLike( const PathLike& other );

  // Append some letters to the current path. It makes
  //  * this + '/' + pjoin
  void join( const std::string pjoin );
  // Append some letters to the current path-like. It makes
  //  * this->join(pjoin)
  void operator/( const std::string pjoin );

  // Get a vector with only every directory element. The last element
  // is either a directory with files or a file itself.
  path_elements_t path_elements() const;

  // Get a path object that is compatible with StdC++17.
  std::filesystem::path path() const;

  // Gives the representation for the current operating system as string.
  const std::string repr() const;

  // Returns, if it is a valid path on the current operating system.
  bool valid() const;
  // Returns, if it is a valid path on the current operating system.
  static bool is_valid_path( const PathLike& p );
  // Returns, if it is a valid path on the current operating system.
  static bool is_valid_path( const std::string& p );
};




// The file's status, that has permissions and information on the file's or directory's type.
// (https://en.cppreference.com/w/cpp/filesystem/file_status)
using file_status = std::filesystem::file_status;
// The file's or directory's type. (https://en.cppreference.com/w/cpp/filesystem/file_type)
using file_type = std::filesystem::file_type;
// The permissions of a file or a directory. (https://en.cppreference.com/w/cpp/filesystem/perms)
using permissions = std::filesystem::perms;


// The class that represents a real existent path and can do operations
// on the filesystem itself.
class Path
    :   public PathLike
{
 public:
  Path( const std::string p );
  Path( const Path& p );

  // Check, if a file or a directory exists at this path's location.
  bool exists() const;

  // Copy the current path to another directory or, if it is a file, it is
  // also possible to copy it a file's location.
  bool copy( PathLike& to_target ) const;
  // Rename the data file or directory at the current path's location to pl.
  void rename( PathLike& pl );
  
  // Get the `file_status` consisting of permissions and the `file_type` at the current path.
  const file_status status() const;
  // Get the `permissions` for the current path.
  permissions get_permissions() const;
  // Set the `permissions` for the current path.
  bool set_permissions( permissions perms );

  // Check, if this is a regular file.
  bool is_regular_file() const;
  // Check, if this is a directory.
  bool is_directory() const;
};




// The default type of file's size
typedef std::uintmax_t file_size;
// The openmodes available for reading and writing files.
// (https://en.cppreference.com/w/cpp/io/ios_base/openmode)
typedef typename std::ios_base::openmode file_openmode;


// The class that handles most file objects on filesystem.
// It is simply a `cacao::Path` with functions on file's size and
// functions for read and write.
// It can be inherited from, e.g. for socket or other file types.
class File
    :   public Path
{
 public:
  File( const std::string p );
  
  // Get the file's size on filesystem.
  file_size fsize() const;
  // Resize the file to another size.
  bool resize( const file_size size_bytes_new ) const;

  // Read from the file and obtain a `string_view`.
  virtual const std::string_view read(
    file_openmode mode = std::ios_base::in ) const;
  // Write to the file on filesystem.
  virtual void write( const std::string_view s,
    file_openmode mode = std::ios_base::app | std::ios_base::out ) const;

  // Write to the file on filesystem with a stream operator.
  File& operator<<( const std::string_view in );
  // Read from the file and obtain an appendation to a `std::string`.
  File& operator>>( std::string out );
};


typedef std::filesystem::directory_entry DirectoryEntry;
typedef std::filesystem::directory_iterator DirectoryIterator;
class Directory;



class DirectoryContent
    :   public std::filesystem::directory_entry
    ,   public Path
{
public:

    // The Type of the stored directory content.
    const enum class Type : int {
        File = 1,
        Directory = 2,
        GenericPath = 3
    } type = Type::GenericPath;
    
    // Construct directory content from a std::filesystem::directory_iterator
    DirectoryContent( const DirectoryIterator& iterator );
    // Construct directory content from a std::filesystem::directory_entry
    DirectoryContent( const DirectoryEntry& entry );
    // Construct `cacao::DirectoryContent` from a `cacao::Directory`.
    DirectoryContent( Directory& dir );
    // Construct `cacao::DirectoryContent` from a `cacao::File`.
    DirectoryContent( File& file );
    // Construct `cacao::DirectoryContent` from a generic `cacao::Path`.
    DirectoryContent( Path& path );
    // Construct `cacao::DirectoryContent` from another `cacao::DirectoryContent`.
    DirectoryContent( const DirectoryContent& dc );

    const std::string pathstr() const;
    // Get this `cacao::DirectoryContent` as `cacao::Path` object.
    const Path path() const;

    // Get a pointer to the `cacao::Directory`, if not a `nullptr`.
    Directory* get_directory() const;
    // Get a pointer to the `cacao::File`, if not a `nullptr`.
    File* get_file() const;
    // Get a pointer to the `cacao::Path` or convert to.
    Path* get_path() const;
    int get_type() const;
    
    // Check, which type of directory content is stored here or use a switch-case on
    // `DirectoryContent::Type` or compate `get_^* != nullptr` with the `get_..`
    // functions above.
    bool is( const DirectoryContent::Type dct ) const;
    // Check, which type of directory content is stored in parameter `dc` or use a
    // switch-case on `DirectoryContent::Type` or compate `get_^* != nullptr` with the
    // `get_..` functions above.
    static DirectoryContent::Type is_type( const DirectoryContent& dc );

    // Used for sorting in `std::set< T >`.
    auto operator<=>(const DirectoryContent& rhs) const;
    // Used for sorting in `std::set< T* >`.
    auto operator<=>(const DirectoryContent*rhs) const;
    // Used for key comparator in `std::set< T >`.
    operator std::basic_string<char>() const;
};


// A sorted `std::set< DirectoryContent >` that is able to seperate between files and directories.
class DirectoryContentList
    :   protected std::set< DirectoryContent >
{
 public:
    typedef typename std::set< DirectoryContent > BaseDirectoryContentList;

    using BaseDirectoryContentList::emplace;
    using BaseDirectoryContentList::insert;
    using BaseDirectoryContentList::size;
    using BaseDirectoryContentList::iterator;
    using BaseDirectoryContentList::const_iterator;
    using BaseDirectoryContentList::begin;
    using BaseDirectoryContentList::end;
    using BaseDirectoryContentList::cbegin;
    using BaseDirectoryContentList::cend;

    DirectoryContentList();
    DirectoryContentList( const DirectoryContentList& dcl );

    // Get only all files as a `cacao::DirectoryContentList`.
    DirectoryContentList get_files() const;
    // Get only all files as a `cacao::DirectoryContentList`.
    DirectoryContentList get_directories() const;

    friend class Directory;
};


// A directory. Stores all content in a compatible sorted `cacao::DirectoryContentList`
// that can be accessed to with `cacao::Directory::contents` and fills it with the
// result of C++17's iterators (`cacao::DirectoryContent(*it)`) on the directories path.
// `cacao::Directory` can be inherited from.
class Directory
    :   public Path
{
 protected:
    DirectoryContentList m_contents;

 public:
    // Get all directory content like gotten from C++17's
    // `std::filesystem::recursive_directory_iterator` as a fully-featured sorted set of
    // bivariate object's of the type `cacao::DirectoryContent`.
    static DirectoryContentList scan_directory( const Path& p, const bool recursive = false );

    Directory( const std::string p );

    // Update the sorted contents set with the current directory contents and get the number of items.
    // One may call `cacao::Directory::contents()` when called this function.
    size_t scandir( const bool recursive = false );
    // Get the sorted-by-path set of unique generic or specific path objects.
    const DirectoryContentList contents() const;
};




} // namespace cacao


// Append a new sublevel `rhs` to the `lhs` path.
cacao::PathLike operator/( const cacao::PathLike& lhs, const cacao::PathLike& rhs );
// Append a new sublevel `rhs` to the `lhs` path.
cacao::PathLike operator/( const cacao::PathLike& lhs, const std::string& rhs );
// Append a new sublevel `rhs` to the `lhs` path.
cacao::PathLike operator/( const std::string& lhs, const std::string& rhs );

