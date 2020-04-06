//
// Created by Mingor on 2020/4/3.
//

#include <fcntl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <message.pb.h>
#include <thread>
#include <unistd.h>
//#include <zconf.h>

using namespace google::protobuf::io;
using namespace std;

mutex m;

int main(int argc, char *argv[]) {

  int times = 10000;

  int fd = open("tmp.txt", O_WRONLY);
  ZeroCopyOutputStream *raw_output = new FileOutputStream(fd);
  auto coded_output = new CodedOutputStream(raw_output);

  auto func = [times, coded_output](int num) mutable {
    while (times--) {
      {
        lock_guard<mutex> lg(m);
        im_message::Message request;
        request.set_session_id(num);
        request.set_type(im_message::HeadType::LOGIN_REQUEST);
        auto message_notification = new im_message::MessageNotification;
        message_notification->set_json(std::to_string(times));
        request.set_allocated_notification(message_notification);

        uint32_t len = request.ByteSizeLong();
        char *buf = new char[len];
        request.SerializeToArray(buf, len);

        coded_output->WriteVarint32(len);
        coded_output->WriteRaw(buf, len);

        delete[] buf;
      }
    }
  };

  thread thread1(func, 1);
  thread thread2(func, 2);
  thread thread3(func, 3);
  thread thread4(func, 4);

  thread1.join();
  thread2.join();
  thread3.join();
  thread4.join();

  delete coded_output;
  delete raw_output;
  close(fd);

  ////////////////////////////////////////

  times = times * 4;

  int fd2 = open("tmp.txt", O_RDONLY);
  ZeroCopyInputStream *raw_input = new FileInputStream(fd2);
  auto coded_input = new CodedInputStream(raw_input);

  while (times--) {

    uint32_t len2;
    coded_input->ReadVarint32(&len2);

    char *buf2 = new char[len2];
    bool success = coded_input->ReadRaw(buf2, len2);
    im_message::Message response;
    response.ParseFromArray(buf2, len2);
//    cout << response.DebugString();

    if (!success)
      cout << "--------------------------------"  << endl;

    delete[] buf2;
  }

  delete coded_input;
  delete raw_input;
  close(fd2);
}
