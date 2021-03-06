// Copyright [2020] <Copyright Kevin, kevin.lau.gd@gmail.com>

#ifndef FT_SRC_RISK_MANAGEMENT_COMMON_STRATEGY_NOTIFIER_H_
#define FT_SRC_RISK_MANAGEMENT_COMMON_STRATEGY_NOTIFIER_H_

#include "ipc/redis.h"
#include "cep/rms/risk_rule.h"

namespace ft {

class StrategyNotifier : public RiskRule {
 public:
  void on_order_accepted(const Order* order);

  void on_order_traded(const Order* order, const Trade* trade) override;

  void on_order_canceled(const Order* order, int canceled) override;

  void on_order_rejected(const Order* order, int error_code) override;

  RedisSession rsp_redis_;
};

}  // namespace ft

#endif  // FT_SRC_RISK_MANAGEMENT_STRATEGY_NOTIFIER_H_
