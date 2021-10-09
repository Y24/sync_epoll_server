/// Note: IOHandler wrap all the raw read and write and provide a higher-level
///
#include "io_handler.h"
IOHandler::IOHandler(int fd) : fd(fd) {}
DemoData IOHandler::read() {
  char nSizeBuf;
  if (int cnt = ::read(fd, &nSizeBuf, 1); cnt == -1) {
    inPanic(fd);
    return DemoData(data_invalid);
  } else if (cnt == 0) {
    fprintf(stderr, "server close.\n");
    close(fd);
    return DemoData(conn_close);
  } else {
    int nSize = factory.charTo<int>(nSizeBuf);
    char* sizeBuf = new char[nSize + 1];
    if (int cnt = ::read(fd, sizeBuf, nSize); cnt < nSize) {
      inPanic(fd);
      return DemoData(data_invalid);
    }
    sizeBuf[nSize] = '\0';
    auto [status, size] = factory.stringTo<long long>(std::string(sizeBuf));
    delete[] sizeBuf;
    if (!status || size <= 0) {
      inPanic(fd);
      return DemoData(data_invalid);
    }
    char* source = new char[size];
    if (long long cnt = ::read(fd, source, size); cnt < size) {
      inPanic(fd);
      delete[] source;
      return DemoData(data_invalid);
    } else {
      auto src = std::string(size, '0');
      for (int i = 0; i < size; i++) {
        src[i] = source[i];
      }
      auto res = DemoData(src);
      delete[] source;
      return res;
    }
  }
}
bool IOHandler::write(DemoData data) {
  std::string sizeStr = factory.toString<long long>(data.getSize());
  char nSize = factory.toChar<int>(sizeStr.size());
  std::string nSizeStr = std::string(1, nSize);
  if (::write(fd, nSizeStr.data(), 1) < 1 ||
      ::write(fd, sizeStr.data(), sizeStr.size()) < sizeStr.size()) {
    outPanic(fd);
    return false;
  }
  std::string src = data.toStr();
  int nSrc = src.size();
  if (int cnt = ::write(fd, src.data(), src.size()); cnt < src.size()) {
    outPanic(fd);
    return false;
  }
  return true;
}
