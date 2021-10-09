#ifndef _DEMO_DATA_H
#define _DEMO_DATA_H 1
#include <ctime>
#include <sstream>
#include <string>

#include "data_factory.h"
enum DataType {
  data_invalid,
  session_init,
  session_pair,
  delivery_data,
  conn_close,
};
struct DataHeader {
  DataType type;
  std::string timestamp;
};

struct DataBody {
  std::string content;
};
class DemoData {
 private:
  DataHeader header;
  DataBody body;
  static const DataFactory factory;

 public:
  DemoData();
  DemoData(DataType type);
  DemoData(DataType type, std::string content);
  DemoData(DataType type, std::string timestamp, std::string content);
  /// Layout: type(1 char) nTimestamp(1 char) timestamp(time_t) content
  DemoData(std::string source);
  std::string toStr() const;
  long long getSize() const;
  bool isNull() const;
  DataHeader getHeader() const;
  DataBody getBody() const;
};
#endif  // demo_data.h