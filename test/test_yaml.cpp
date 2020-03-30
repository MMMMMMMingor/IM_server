#include <yaml.hpp>

int main(int argc, char *argv[]) {
  Yaml::Node root;
  Yaml::Parse(root, "../config/server.yml");

  // Print all scalars.
  std::cout << root["server"]["ip"].As<std::string>() << std::endl;

  return 0;
}