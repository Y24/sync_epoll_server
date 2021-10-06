#include "demo_data.h"
DemoData::DemoData() { this->header.type = data_invalid; }
DemoData::DemoData(DataType type) { this->header.type = type; }
DemoData::DemoData(DataType type, std::string content) {
  this->header.type = type;
  this->body.content = content;
}
DemoData::DemoData(DataType type, std::string timestamp, std::string content) {
  this->header.type = type;
  this->header.timestamp = timestamp;
  this->body.content = content;
}
/// Layout: type(1 char) nTimestamp(1 char) timestamp(time_t) content
DemoData::DemoData(std::string source) {
  if (source == "")
    this->header.type = data_invalid;
  else {
    this->header.type = factory.charTo<DataType>(source[0]);
    int nTimeStamp = factory.charTo<int>(source[1]);
    this->header.timestamp = source.substr(2, nTimeStamp);
    this->body.content = source.substr(2 + nTimeStamp);
  }
}
std::string DemoData::toStr() const {
  return std::string(1, factory.toChar(header.type)) +
         std::string(1, factory.toChar(header.timestamp.size())) +
         header.timestamp + body.content;
}
long long DemoData::getSize() const {
  return 2 + header.timestamp.size() + body.content.size();
}
bool DemoData::isNull() const { return header.type == data_invalid; }
DataHeader DemoData::getHeader() const { return header; }
DataBody DemoData::getBody() const { return body; }
DataFactory::DataFactory() {}

DataFactory::~DataFactory() {}