
#ifndef CCTREE_H
#define CCTREE_H

#include <string>
#include <map>

class Method
{
public:
  typedef std::map<int, Method *> MethodMap;

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


#endif
