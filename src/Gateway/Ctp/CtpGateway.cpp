// Copyright [2020] <Copyright Kevin, kevin.lau.gd@gmail.com>

#include "Gateway/Ctp/CtpGateway.h"

#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>
#include <spdlog/spdlog.h>

#include "Core/Constants.h"
#include "Core/ContractTable.h"

namespace ft {

CtpGateway::CtpGateway(TradingEngineInterface *engine)
    : trade_api_(std::make_unique<CtpTradeApi>(engine)),
      md_api_(std::make_unique<CtpMdApi>(engine)) {}

CtpGateway::~CtpGateway() {}

bool CtpGateway::login(const LoginParams &params) {
  if (params.broker_id().size() > sizeof(TThostFtdcBrokerIDType) ||
      params.broker_id().empty() ||
      params.investor_id().size() > sizeof(TThostFtdcUserIDType) ||
      params.investor_id().empty() ||
      params.passwd().size() > sizeof(TThostFtdcPasswordType) ||
      params.passwd().empty()) {
    spdlog::error("[CtpGateway::login] Failed. Invalid login params");
    return false;
  }

  if (params.front_addr().empty() && params.md_server_addr().empty()) {
    spdlog::warn("[CtpGateway::login] 交易柜台和行情服务器地址都未设置");
    return false;
  }

  if (!params.front_addr().empty()) {
    spdlog::debug("[CtpGateway::login] Login into trading server");
    if (!trade_api_->login(params)) {
      spdlog::error("[CtpGateway::login] Failed to login into the counter");
      return false;
    }
  }

  if (!params.md_server_addr().empty()) {
    spdlog::debug("[CtpGateway::login] Login into market data server");
    if (!md_api_->login(params)) {
      spdlog::error("[CtpGateway::login] Failed to login into the md server");
      return false;
    }
  }

  return true;
}

void CtpGateway::logout() {
  trade_api_->logout();
  md_api_->logout();
}

uint64_t CtpGateway::send_order(const OrderReq *order) {
  return trade_api_->send_order(order);
}

bool CtpGateway::cancel_order(uint64_t order_id) {
  return trade_api_->cancel_order(order_id);
}

bool CtpGateway::query_contract(const std::string &ticker,
                                const std::string &exchange) {
  return trade_api_->query_contract(ticker, exchange);
}

bool CtpGateway::query_contracts() { return trade_api_->query_contracts(); }

bool CtpGateway::query_position(const std::string &ticker) {
  return trade_api_->query_position(ticker);
}

bool CtpGateway::query_positions() { return trade_api_->query_positions(); }

bool CtpGateway::query_account() { return trade_api_->query_account(); }

bool CtpGateway::query_trades() { return trade_api_->query_trades(); }

bool CtpGateway::query_margin_rate(const std::string &ticker) {
  return trade_api_->query_margin_rate(ticker);
}

}  // namespace ft
