#include "GGMNode.h"

long GenIndex(const std::string &bin);
class PrefixPrf {

public:
  explicit PrefixPrf(int width) : width_(width) {}
  GGMNode Cons(const std::string &prefix, const std::string &masterKey);
  std::string Eval(const GGMNode &consKey, const std::string &x);
  std::string DirectEval(const std::string &x, const std::string &masterKey);
  int get_level() const;

private:
  int width_;
};