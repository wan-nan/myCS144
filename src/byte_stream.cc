#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  // Your code here.
  (void)data;
  totalBytesPushed += min( data.size(), capacity_ - buffer.size() );
  if ( _open == false )
    return;
  if ( buffer.size() >= capacity_ )
    return;
  buffer += data.substr( 0, min( data.size(), capacity_ - buffer.size() ) );
}

void Writer::close()
{
  // Your code here.
  _open = false;
}

void Writer::set_error()
{
  // Your code here.
  _error = true;
}

bool Writer::is_closed() const
{
  // Your code here.
  return !_open;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - buffer.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return totalBytesPushed;
}

string_view Reader::peek() const
{
  // Your code here.
  return buffer;
}

bool Reader::is_finished() const
{
  // Your code here.
  return !_open && buffer.size() == 0;
}

bool Reader::has_error() const
{
  // Your code here.
  return _error;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  (void)len;
  totalBytesPoped += min( len, buffer.size() );
  if ( len >= buffer.size() )
    buffer = "";
  else
    buffer = buffer.substr( len );
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffer.size();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return totalBytesPoped;
}
