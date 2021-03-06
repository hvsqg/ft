// Copyright [2020] <Copyright Kevin, kevin.lau.gd@gmail.com>

#ifndef FT_INCLUDE_CEP_DATA_CONTRACT_TABLE_H_
#define FT_INCLUDE_CEP_DATA_CONTRACT_TABLE_H_

#include <fmt/format.h>

#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cep/data/constants.h"
#include "cep/data/contract.h"
#include "utils/string_utils.h"

namespace ft {

inline bool load_contracts(const std::string& file,
                           std::vector<Contract>* contracts) {
  std::ifstream ifs(file);
  std::string line;
  std::vector<std::string> fields;
  Contract contract;

  if (!ifs) return false;

  std::getline(ifs, line);  // skip header
  while (std::getline(ifs, line)) {
    fields.clear();
    split(line, ",", &fields);
    if (fields.empty() || fields[0].front() == '\n') continue;

    if (fields.size() != 14) return false;

    std::size_t index = 0;
    contract.ticker = std::move(fields[index++]);
    contract.exchange = std::move(fields[index++]);
    contract.name = std::move(fields[index++]);
    contract.product_type = string2product(fields[index++]);
    contract.size = std::stoi(fields[index++]);
    contract.price_tick = std::stod(fields[index++]);
    contract.long_margin_rate = std::stod(fields[index++]);
    contract.short_margin_rate = std::stod(fields[index++]);
    contract.max_market_order_volume = std::stoi(fields[index++]);
    contract.min_market_order_volume = std::stoi(fields[index++]);
    contract.max_limit_order_volume = std::stoi(fields[index++]);
    contract.min_limit_order_volume = std::stoi(fields[index++]);
    contract.delivery_year = std::stoi(fields[index++]);
    contract.delivery_month = std::stoi(fields[index++]);
    contracts->emplace_back(std::move(contract));
  }

  return true;
}

inline void store_contracts(const std::string& file,
                            const std::vector<Contract>& contracts) {
  std::ofstream ofs(file, std::ios_base::trunc);
  ofs << "ticker,"
         "exchange,"
         "name,"
         "product_type,"
         "size,"
         "price_tick,"
         "long_margin_rate,"
         "short_margin_rate,"
         "max_market_order_volume,"
         "min_market_order_volume,"
         "max_limit_order_volume,"
         "min_limit_order_volume,"
         "delivery_year,"
         "delivery_month\n";

  // std::stringstream ss;
  std::string line;
  for (const auto& contract : contracts) {
    line = fmt::format(
        "{},{},{},{},{},{},{},{},{},{},{},{},{},{}\n", contract.ticker,
        contract.exchange, contract.name, to_string(contract.product_type),
        contract.size, contract.price_tick, contract.long_margin_rate,
        contract.short_margin_rate, contract.max_market_order_volume,
        contract.min_market_order_volume, contract.max_limit_order_volume,
        contract.min_limit_order_volume, contract.delivery_year,
        contract.delivery_month);

    ofs << line;
  }

  ofs.close();
}

class ContractTable {
 public:
  static bool init(std::vector<Contract>&& vec) {
    if (!get()->is_inited_) {
      auto& contracts = get()->contracts_;
      auto& ticker2contract = get()->ticker2contract_;
      contracts = std::move(vec);
      for (std::size_t i = 0; i < contracts.size(); ++i) {
        auto& contract = contracts[i];
        contract.tid = i + 1;
        ticker2contract.emplace(contract.ticker, &contract);
      }
      get()->is_inited_ = true;
    }

    return true;
  }

  static bool init(const std::string& file) {
    if (!get()->is_inited_) {
      auto& contracts = get()->contracts_;
      auto& ticker2contract = get()->ticker2contract_;
      if (!load_contracts(file, &contracts)) return false;
      for (std::size_t i = 0; i < contracts.size(); ++i) {
        auto& contract = contracts[i];
        contract.tid = i + 1;
        ticker2contract.emplace(contract.ticker, &contract);
      }
      get()->is_inited_ = true;
    }

    return true;
  }

  static bool inited() { return get()->is_inited_; }

  static void store(const std::string& file) {
    store_contracts(file, get()->contracts_);
  }

  static const Contract* get_by_ticker(const std::string& ticker) {
    auto& ticker2contract = get()->ticker2contract_;
    auto iter = ticker2contract.find(ticker);
    if (iter == ticker2contract.end()) return nullptr;
    return iter->second;
  }

  static const Contract* get_by_index(uint32_t tid) {
    auto& contracts = get()->contracts_;
    if (tid == 0 || tid > contracts.size()) return nullptr;
    return &contracts[tid - 1];
  }

  static std::size_t size() { return get()->contracts_.size(); }

 private:
  static ContractTable* get() {
    static ContractTable ct;
    return &ct;
  }

 private:
  bool is_inited_ = false;
  std::vector<Contract> contracts_;
  std::unordered_map<std::string, Contract*> ticker2contract_;
};

}  // namespace ft

#endif  // FT_INCLUDE_CEP_DATA_CONTRACT_TABLE_H_
