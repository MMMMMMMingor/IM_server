#include "server/websocket_protocol.h"
#include "server/base64.h"
#include "server/sha1.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sstream>

const char *MAGIC_KEY = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

namespace WebSocket_Protocol {

WS_Status get_response_http(string &request, string &response) {
  // 解析http请求头信息
  WebSocket_Protocol::WS_Status ret = WEBSOCKET_UNCONNECTED;
  std::istringstream stream(request);
  std::string reqType;
  std::getline(stream, reqType);
  if (reqType.substr(0, 4) != "GET ") {
    return ret;
  }

  std::string header;
  std::string::size_type pos = 0;
  std::string websocketKey;
  while (std::getline(stream, header) && header != "\r") {
    header.erase(header.end() - 1);
    pos = header.find(": ", 0);
    if (pos != std::string::npos) {
      std::string key = header.substr(0, pos);
      std::string value = header.substr(pos + 2);
      if (key == "Sec-WebSocket-Key") {
        ret = WEBSOCKET_HANDSHAKE;
        websocketKey = value;
        break;
      }
    }
  }

  if (ret != WEBSOCKET_HANDSHAKE) {
    return ret;
  }

  // 填充http响应头信息
  response = "HTTP/1.1 101 Switching Protocols\r\n";
  response += "Connection: upgrade\r\n";
  response += "Sec-WebSocket-Accept: ";

  std::string serverKey = websocketKey + MAGIC_KEY;

  SHA1 sha;
  unsigned int message_digest[5];
  sha.Reset();
  sha << serverKey.c_str();

  sha.Result(message_digest);
  for (unsigned int &i : message_digest) {
    i = htonl(i);
  }
  serverKey = base64_encode(
      reinterpret_cast<const unsigned char *>(message_digest), 20);
  response += serverKey;
  response += "\r\n";
  response += "Upgrade: websocket\r\n\r\n";

  return ret;
}

WS_FrameType ws_decode_frame(const string &inFrame, string &outMessage) {
  int ret = WS_OPENING_FRAME;
  const char *frameData = inFrame.c_str();
  const int frameLength = inFrame.size();
  if (frameLength < 2) {
    ret = WS_ERROR_FRAME;
  }

  // 检查扩展位并忽略
  if ((frameData[0] & 0x70) != 0x0) {
    ret = WS_ERROR_FRAME;
  }

  // fin位: 为1表示已接收完整报文, 为0表示继续监听后续报文
  ret = (frameData[0] & 0x80);
  if ((frameData[0] & 0x80) != 0x80) {
    ret = WS_ERROR_FRAME;
  }

  // mask位, 为1表示数据被加密
  if ((frameData[1] & 0x80) != 0x80) {
    ret = WS_ERROR_FRAME;
  }

  // 操作码
  uint16_t payloadLength = 0;
  uint8_t payloadFieldExtraBytes = 0;
  uint8_t opcode = static_cast<uint8_t>(frameData[0] & 0x0f);
  if (opcode == WS_TEXT_FRAME) {
    // 处理utf-8编码的文本帧
    payloadLength = static_cast<uint16_t>(frameData[1] & 0x7f);
    if (payloadLength == 0x7e) {
      uint16_t payloadLength16b = 0;
      payloadFieldExtraBytes = 2;
      memcpy(&payloadLength16b, &frameData[2], payloadFieldExtraBytes);
      payloadLength = ntohs(payloadLength16b);
    } else if (payloadLength == 0x7f) {
      // 数据过长,暂不支持
      ret = WS_ERROR_FRAME;
    }
  } else if (opcode == WS_BINARY_FRAME || opcode == WS_PING_FRAME ||
             opcode == WS_PONG_FRAME) {
    // 二进制/ping/pong帧暂不处理
  } else if (opcode == WS_CLOSING_FRAME) {
    ret = WS_CLOSING_FRAME;
  } else {
    ret = WS_ERROR_FRAME;
  }

  // 数据解码
  if ((ret != WS_ERROR_FRAME) && (payloadLength > 0)) {
    // header: 2字节, masking key: 4字节
    const char *maskingKey = &frameData[2 + payloadFieldExtraBytes];
    char *payloadData = new char[payloadLength + 1];
    memset(payloadData, 0, payloadLength + 1);
    memcpy(payloadData, &frameData[2 + payloadFieldExtraBytes + 4],
           payloadLength);
    for (int i = 0; i < payloadLength; i++) {
      payloadData[i] = payloadData[i] ^ maskingKey[i % 4];
    }

    outMessage = payloadData;
    delete[] payloadData;
  }

  return static_cast<WS_FrameType>(ret);
}

WS_FrameType ws_encode_frame(const string &inMessage, string &outFrame,
                             enum WS_FrameType frameType) {
  int ret = WS_EMPTY_FRAME;
  const uint32_t messageLength = inMessage.size();
  if (messageLength > 32767) {
    // 暂不支持这么长的数据
    std::cout << "暂不支持这么长的数据" << std::endl;

    return WS_ERROR_FRAME;
  }

  uint8_t payloadFieldExtraBytes = (messageLength <= 0x7d) ? 0 : 2;
  // header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写,
  // 省略4字节
  uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
  uint8_t *frameHeader = new uint8_t[frameHeaderSize];
  memset(frameHeader, 0, frameHeaderSize);
  // fin位为1, 扩展位为0, 操作位为frameType
  frameHeader[0] = static_cast<uint8_t>(0x80 | frameType);

  // 填充数据长度
  if (messageLength <= 0x7d) {
    frameHeader[1] = static_cast<uint8_t>(messageLength);
  } else {
    frameHeader[1] = 0x7e;
    uint16_t len = htons(messageLength);
    memcpy(&frameHeader[2], &len, payloadFieldExtraBytes);
  }

  // 填充数据
  uint32_t frameSize = frameHeaderSize + messageLength;
  char *frame = new char[frameSize + 1];
  memcpy(frame, frameHeader, frameHeaderSize);
  memcpy(frame + frameHeaderSize, inMessage.c_str(), messageLength);
  frame[frameSize] = '\0';
  outFrame = frame;

  delete[] frame;
  delete[] frameHeader;
  return static_cast<WS_FrameType>(ret);
}
} // namespace WebSocket_Protocol
