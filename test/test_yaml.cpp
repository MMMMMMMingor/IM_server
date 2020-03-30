#include <yaml.hpp>

int main(int argc, char *argv[]) {
  Yaml::Node root;
  Yaml::Parse(root, "../config/server.yml");

  // Print all scalars.
  std::cout << root["server"]["port"].As<int>() << std::endl;

  return 0;
}