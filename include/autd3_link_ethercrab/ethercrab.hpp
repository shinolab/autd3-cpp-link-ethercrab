#pragma once

#include <chrono>
#include <optional>
#include <string>

#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"
#include "autd3_link_ethercrab/native_methods.hpp"

namespace autd3::link {

class Status {
  native_methods::Status _inner;
  std::string _msg;

 public:
  AUTD3_API static const Status Lost;
  AUTD3_API static const Status Error;
  AUTD3_API static const Status StateChanged;
  AUTD3_API static const Status Resumed;

  friend struct EtherCrab;

  explicit Status(const native_methods::Status inner, std::string msg) : _inner(inner), _msg(std::move(msg)) {}

  bool operator==(const Status& that) const { return _inner == that._inner && _msg == that._msg; }

  friend std::ostream& operator<<(std::ostream& os, const Status& s);
};

inline const Status Status::Lost = Status(native_methods::Status::Lost, "");
inline const Status Status::Error = Status(native_methods::Status::Error, "");
inline const Status Status::StateChanged = Status(native_methods::Status::StateChanged, "");
inline const Status Status::Resumed = Status(native_methods::Status::Resumed, "");

inline std::ostream& operator<<(std::ostream& os, const Status& s) {
  os << s._msg;
  return os;
}

template <class F>
concept ethercrab_err_handler_f = requires(F f, const uint16_t slave, const Status status) {
  { f(slave, status) } -> std::same_as<void>;
};

struct EtherCrabOption {
  std::optional<std::string> ifname = std::nullopt;
  std::chrono::nanoseconds state_check_period = std::chrono::milliseconds(100);
  std::chrono::nanoseconds sync0_period = std::chrono::milliseconds(2);
  std::chrono::nanoseconds sync_tolerance = std::chrono::microseconds(1);
  std::chrono::nanoseconds sync_timeout = std::chrono::seconds(10);

  operator native_methods::EtherCrabOption() const {
    return native_methods::EtherCrabOption{
        .ifname = ifname.has_value() ? ifname.value().c_str() : nullptr,
        .dc_configuration_sync0_period = native_methods::to_duration(sync0_period),
        .state_check_period = native_methods::to_duration(state_check_period),
        .sync_tolerance = native_methods::to_duration(sync_tolerance),
        .sync_timeout = native_methods::to_duration(sync_timeout),
    };
  }
};

struct EtherCrab final {
  using err_handler_t = void (*)(uint16_t, Status);

  explicit EtherCrab(const err_handler_t err_handler, EtherCrabOption option) : err_handler(err_handler), option(std::move(option)) {
    _native_err_handler = +[](const void* context, const uint32_t slave, const native_methods::Status status) {           // LCOV_EXCL_LINE
      const std::string msg(128, ' ');                                                                                    // LCOV_EXCL_LINE
      (void)AUTDLinkEtherCrabStatusGetMsg(status, const_cast<char*>(msg.c_str()));                                        // LCOV_EXCL_LINE
      (*reinterpret_cast<err_handler_t>(const_cast<void*>(context)))(static_cast<uint16_t>(slave), Status(status, msg));  // LCOV_EXCL_LINE
    };  // LCOV_EXCL_LINE
  }

  [[nodiscard]] native_methods::LinkPtr resolve() const {
    return validate(native_methods::AUTDLinkEtherCrab(reinterpret_cast<void*>(_native_err_handler), reinterpret_cast<void*>(err_handler), option));
  }

  err_handler_t err_handler;
  EtherCrabOption option;

 private:
  using native_err_handler_t = void (*)(const void*, uint32_t, native_methods::Status);
  native_err_handler_t _native_err_handler;
};

}  // namespace autd3::link
