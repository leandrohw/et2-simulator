
#ifndef METHOD_H
#define METHOD_H

#include <string>
#include <map>

namespace et_simulator {

class Method;
typedef std::map<int, Method *> MethodMap;
class Method
{
 private:
  int id;
  std::string class_name;
  std::string method_name;
  std::string signature;

  static MethodMap allMethods;

 public:
  Method(int i, std::string cn, std::string mn, std::string sig)
    : id(i),
      class_name(cn),
      method_name(mn),
      signature(sig)
  {
    allMethods[i] = this;
  }

  const std::string & getClassName() const { return class_name; }
  const std::string & getMethodName() const { return method_name; }
  const std::string & getSignature() const { return signature; }

  static Method * getMethod(int id) {
    return allMethods[id];
  }
};
}
#endif
