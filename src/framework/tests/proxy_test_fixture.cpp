#include <fstream>

#include <boost/property_tree/json_parser.hpp>

#include "tests/proxy_test_fixture.h"

namespace ssf {
namespace tests {

Address::Address() : addr_(""), port_("") {}

Address::Address(const std::string& addr, const std::string& port)
    : addr_(addr), port_(port) {}

Address::Address(const Address& address)
    : addr_(address.addr_), port_(address.port_) {}

Address& Address::operator=(const Address& address) {
  addr_ = address.addr_;
  port_ = address.port_;

  return *this;
}

bool Address::IsSet() { return addr_ != "" && port_ != ""; }

ssf::layer::LayerParameters Address::ToProxyParam() {
  return {{"http_addr", addr_}, {"http_port", port_}};
}

ssf::layer::LayerParameters Address::ToTCPParam() {
  return {{"addr", addr_}, {"port", port_}};
}

}  // tests
}  // ssf

ProxyTestFixture::ProxyTestFixture()
    : config_file_("./proxy/proxy.json"), config_options_() {}

ProxyTestFixture::~ProxyTestFixture() {}

void ProxyTestFixture::SetUp() {
  std::vector<ssf::tests::Address> test_addresses;
  ParseConfigFile(config_file_);
}

void ProxyTestFixture::TearDown() {}

bool ProxyTestFixture::Initialized() {
  return config_options_.count("target_addr") > 0 &&
         config_options_.count("target_port") > 0 &&
         config_options_.count("proxy_addr") > 0 &&
         config_options_.count("proxy_port") > 0;
}

std::string ProxyTestFixture::GetOption(const std::string& name) const {
  auto opt_it = config_options_.find(name);

  return opt_it != config_options_.end() ? opt_it->second : "";
}

ssf::layer::LayerParameters ProxyTestFixture::GetTcpParam() const {
  ssf::layer::LayerParameters tcp_params;
  tcp_params["addr"] = GetOption("target_addr");
  tcp_params["port"] = GetOption("target_port");

  return tcp_params;
}

ssf::layer::LayerParameters ProxyTestFixture::GetProxyParam() const {
  ssf::layer::LayerParameters proxy_params;
  proxy_params["http_addr"] = GetOption("proxy_addr");
  proxy_params["http_port"] = GetOption("proxy_port");
  proxy_params["http_username"] = GetOption("username");
  proxy_params["http_password"] = GetOption("password");

  return proxy_params;
}

bool ProxyTestFixture::ParseConfigFile(const std::string& filepath) {
  if (filepath == "") {
    return false;
  }

  std::ifstream file(filepath);
  if (!file.is_open()) {
    return false;
  }
  file.close();

  try {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(filepath, pt);
    for (const auto& child : pt) {
      config_options_[child.first] = child.second.data();
    }
    return true;
  } catch (const std::exception&) {
    return false;
  }
}
