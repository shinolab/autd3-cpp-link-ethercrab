#include "autd3.hpp"
#include "autd3_link_ethercrab.hpp"
#include "util.hpp"

int main() try {
  auto autd = autd3::Controller::open(
      std::vector{autd3::AUTD3{.pos = autd3::Point3::origin(), .rot = autd3::Quaternion::Identity()},
                  autd3::AUTD3{.pos = autd3::Point3(autd3::AUTD3::DEVICE_WIDTH, 0.0, 0.0), .rot = autd3::Quaternion::Identity()}},
      autd3::link::EtherCrab{
          [](const uint16_t idx, const autd3::link::Status status) { std::cout << "Device[" << idx << "]: " << status << std::endl; },
          autd3::link::EtherCrabOption{}});

  autd.send((autd3::Sine(150 * autd3::Hz, autd3::SineOption{}), autd3::Focus(autd.center() + autd3::Vector3(0.0, 0.0, 150.0), autd3::FocusOption{})));

  std::string in;
  getline(std::cin, in);

  autd.close();

  return 0;
} catch (std::exception& e) {
  print_err(e);
  return -1;
}
