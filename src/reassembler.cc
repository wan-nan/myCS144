#include "reassembler.hh"

using namespace std;

void Reassembler::preProcess( uint64_t& first_index, string& data, Writer& output )
{
  // first_unassembled_index 即为 output.bytes_pushed()
  uint64_t first_unassembled_index = output.bytes_pushed();
  // 直接丢弃[first_unassembled_index, first_unacceptable_index)之外的数据
  if ( first_index + data.size() <= first_unassembled_index ) {
    data = "";
    return;
  }
  uint64_t first_unacceptable_index = first_unassembled_index + output.available_capacity();
  if ( first_index >= first_unacceptable_index ) {
    data = "";
    return;
  }

  // 去尾
  if ( first_index + data.size() > first_unacceptable_index ) {
    // data = data.substr( 0, first_unacceptable_index - first_index );
    if ( first_index < first_unassembled_index )
      data = data.substr( 0, first_unacceptable_index - first_index );
    else
      data = data.substr( first_unassembled_index, first_unacceptable_index - first_index );
  }
  // 掐头
  if ( first_index < first_unassembled_index ) {
    // data = data.substr( first_unassembled_index - first_index );
    if ( first_index + data.size() >= first_unacceptable_index )
      data
        = data.substr( first_unassembled_index - first_index, first_unacceptable_index - first_unassembled_index );
    else
      data = data.substr( first_unassembled_index - first_index );
    first_index = first_unassembled_index;
  }
}
// 处理data与map中已有字符串的重叠问题
bool Reassembler::process_overlapping( uint64_t& first_index, string& data )
{
  for ( auto iter = reassembler_buffer.begin(); iter != reassembler_buffer.end(); ) {
    uint64_t len = data.size();
    uint64_t index = iter->first;
    string str = iter->second;
    if ( first_index + len < index || index + str.size() < first_index ) {
      iter++;
      continue;
    }
    // 新插入的包围原有的
    if ( first_index < index && first_index + len > index + str.size() ) {
      bytes_in_Reassembler -= str.size();
      reassembler_buffer.erase( iter++ );
    }
    // 新插入的被原有的包围
    else if ( first_index >= index && first_index + len <= index + str.size() ) {
      // 返回false，表示不能插入buffer
      return false;
    }
    // 新串屁股重叠
    else if ( first_index < index && first_index + len - 1 >= index ) {
      data = data.substr( 0, index - first_index );
      iter++;
    }
    // 新串头部重叠
    else if ( first_index < index + str.size() && first_index + len > index + str.size() ) {
      data = data.substr( index + str.size() - first_index );
      first_index = index + str.size();
      iter++;
    } else {
      ++iter;
    }
  }
  return true;
}
void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
  // Your code here.
  (void)first_index;
  (void)data;
  (void)is_last_substring;
  (void)output;

  // 0. 没空间了，直接关闭
  // 曾经一个bug：part 0写在预处理preProcess()中，buffer用完的时候直接return了，但是insert()还会继续执行
  //             所以需要再insert()中判断，并return
  // 曾经一个bug：不能判断可用空间=0时就关闭，因为可能还没有Read，不是一定插不进去
  // if ( output.available_capacity() == 0 ) {
  //   output.close();
  //   return;
  // }
  // 0. update whole_length
  if ( is_last_substring )
    whole_length = first_index + data.size();

  // 1. preProcess first
  preProcess( first_index, data, output );
  if ( data == "" && !is_last_substring )
    return;

  // 2. process the overlapping part of the new string and strings in map
  if ( process_overlapping( first_index, data ) == false )
    return;

  // 3. insert data into map
  reassembler_buffer[first_index] = data;
  bytes_in_Reassembler += data.size();

  // 4. deliver the data in reassembler_buffer
  uint64_t first_unassembled_index = output.bytes_pushed();
  for ( auto iter = reassembler_buffer.begin(); iter != reassembler_buffer.end(); ) {
    uint64_t index = iter->first;
    string str = iter->second;
    if ( index == first_unassembled_index ) {
      output.push( str );
      first_unassembled_index += str.size();
      bytes_in_Reassembler -= str.size();
      // std::cout << "bytes_pending: " << bytes_in_Reassembler << std::endl;
      reassembler_buffer.erase( iter++ );
    } else
      iter++;
  }

  // 5. judge whether it's the end or not
  // if ( is_last_substring )
  //   whole_length = first_index + data.size();
  if ( output.bytes_pushed() == whole_length )
    output.close();
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_in_Reassembler;
}
