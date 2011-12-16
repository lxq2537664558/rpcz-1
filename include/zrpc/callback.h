// Copyright 2011 Google Inc. All Rights Reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: nadavs@google.com <Nadav Samet>

#ifndef ZRPC_CALLBACK_H
#define ZRPC_CALLBACK_H

#include "zrpc/macros.h"

namespace zrpc {

class Closure {
 public:
  Closure() {}
  virtual ~Closure() {}

  virtual void Run() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Closure);
};

// For args = 0
namespace internal {
class FunctionClosure0 : public Closure {
 public:
  typedef void (*FunctionType)();

  FunctionClosure0(FunctionType function, bool self_deleting)
    : function_(function), self_deleting_(self_deleting) {}
  virtual ~FunctionClosure0() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_();
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
};

template <typename Class>
class MethodClosure0 : public Closure {
 public:
  typedef void (Class::*MethodType)();

  MethodClosure0(Class* object, MethodType method,
      bool self_deleting)    : object_(object), method_(method),
      self_deleting_(self_deleting) {}
  virtual ~MethodClosure0() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)();
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
};
}  // namespace internal

inline Closure* NewCallback(void (*function)()) {
  return new internal::FunctionClosure0(function, true);
}
inline Closure* NewPermanentCallback(void (*function)()) {
  return new internal::FunctionClosure0(function, false);
}

template <typename Class>
inline Closure* NewCallback(Class* object, void (Class::*method)()) {
  return new internal::MethodClosure0<Class>(object, method, true);
}

template <typename Class>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)()) {
  return new internal::MethodClosure0<Class>(object, method, false);
}

// For args = 1
namespace internal {
template <typename Arg1>
class FunctionClosure1 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1);

  FunctionClosure1(FunctionType function, bool self_deleting, Arg1 arg1)
    : function_(function), self_deleting_(self_deleting), arg1_(arg1) {}
  virtual ~FunctionClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
};

template <typename Class, typename Arg1>
class MethodClosure1 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1);

  MethodClosure1(Class* object, MethodType method, bool self_deleting,
      Arg1 arg1)    : object_(object), method_(method),
      self_deleting_(self_deleting), arg1_(arg1) {}
  virtual ~MethodClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;  Arg1 arg1_;

};
}  // namespace internal

template <typename Arg1>
inline Closure* NewCallback(void (*function)(Arg1), Arg1 arg1) {
  return new internal::FunctionClosure1<Arg1>(function, true, arg1);
}
template <typename Arg1>
inline Closure* NewPermanentCallback(void (*function)(Arg1), Arg1 arg1) {
  return new internal::FunctionClosure1<Arg1>(function, false, arg1);
}

template <typename Class, typename Arg1>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1),
    Arg1 arg1) {
  return new internal::MethodClosure1<Class, Arg1>(object, method, true, arg1);
}

template <typename Class, typename Arg1>
inline Closure* NewPermanentCallback(Class* object,
    void (Class::*method)(Arg1), Arg1 arg1) {
  return new internal::MethodClosure1<Class, Arg1>(object, method, false, arg1);
}

// For args = 2
namespace internal {
template <typename Arg1, typename Arg2>
class FunctionClosure2 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2);

  FunctionClosure2(FunctionType function, bool self_deleting, Arg1 arg1,
      Arg2 arg2)
    : function_(function), self_deleting_(self_deleting), arg1_(arg1),
        arg2_(arg2) {}
  virtual ~FunctionClosure2() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_, arg2_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};

template <typename Class, typename Arg1, typename Arg2>
class MethodClosure2 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2);

  MethodClosure2(Class* object, MethodType method, bool self_deleting,
      Arg1 arg1, Arg2 arg2)    : object_(object), method_(method),
      self_deleting_(self_deleting), arg1_(arg1), arg2_(arg2) {}
  virtual ~MethodClosure2() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_, arg2_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;  Arg1 arg1_;
  Arg2 arg2_;

};
}  // namespace internal

template <typename Arg1, typename Arg2>
inline Closure* NewCallback(void (*function)(Arg1, Arg2), Arg1 arg1,
    Arg2 arg2) {
  return new internal::FunctionClosure2<Arg1, Arg2>(function, true, arg1, arg2);
}
template <typename Arg1, typename Arg2>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2), Arg1 arg1,
    Arg2 arg2) {
  return new internal::FunctionClosure2<Arg1, Arg2>(function, false, arg1,
      arg2);
}

template <typename Class, typename Arg1, typename Arg2>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2),
    Arg1 arg1, Arg2 arg2) {
  return new internal::MethodClosure2<Class, Arg1, Arg2>(object, method, true,
      arg1, arg2);
}

template <typename Class, typename Arg1, typename Arg2>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1,
    Arg2), Arg1 arg1, Arg2 arg2) {
  return new internal::MethodClosure2<Class, Arg1, Arg2>(object, method, false,
      arg1, arg2);
}

// For args = 3
namespace internal {
template <typename Arg1, typename Arg2, typename Arg3>
class FunctionClosure3 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

  FunctionClosure3(FunctionType function, bool self_deleting, Arg1 arg1,
      Arg2 arg2, Arg3 arg3)
    : function_(function), self_deleting_(self_deleting), arg1_(arg1),
        arg2_(arg2), arg3_(arg3) {}
  virtual ~FunctionClosure3() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_, arg2_, arg3_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
};

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
class MethodClosure3 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

  MethodClosure3(Class* object, MethodType method, bool self_deleting,
      Arg1 arg1, Arg2 arg2, Arg3 arg3)    : object_(object), method_(method),
      self_deleting_(self_deleting), arg1_(arg1), arg2_(arg2), arg3_(arg3) {}
  virtual ~MethodClosure3() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_, arg2_, arg3_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;

};
}  // namespace internal

template <typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewCallback(void (*function)(Arg1, Arg2, Arg3), Arg1 arg1,
    Arg2 arg2, Arg3 arg3) {
  return new internal::FunctionClosure3<Arg1, Arg2, Arg3>(function, true, arg1,
      arg2, arg3);
}
template <typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2, Arg3),
    Arg1 arg1, Arg2 arg2, Arg3 arg3) {
  return new internal::FunctionClosure3<Arg1, Arg2, Arg3>(function, false,
      arg1, arg2, arg3);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2,
    Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
  return new internal::MethodClosure3<Class, Arg1, Arg2, Arg3>(object, method,
      true, arg1, arg2, arg3);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1,
    Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
  return new internal::MethodClosure3<Class, Arg1, Arg2, Arg3>(object, method,
      false, arg1, arg2, arg3);
}

// For args = 4
namespace internal {
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FunctionClosure4 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4);

  FunctionClosure4(FunctionType function, bool self_deleting, Arg1 arg1,
      Arg2 arg2, Arg3 arg3, Arg4 arg4)
    : function_(function), self_deleting_(self_deleting), arg1_(arg1),
        arg2_(arg2), arg3_(arg3), arg4_(arg4) {}
  virtual ~FunctionClosure4() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_, arg2_, arg3_, arg4_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;
};

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4>
class MethodClosure4 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4);

  MethodClosure4(Class* object, MethodType method, bool self_deleting,
      Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)    : object_(object),
      method_(method), self_deleting_(self_deleting), arg1_(arg1), arg2_(arg2),
      arg3_(arg3), arg4_(arg4) {}
  virtual ~MethodClosure4() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_, arg2_, arg3_, arg4_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;

};
}  // namespace internal

template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline Closure* NewCallback(void (*function)(Arg1, Arg2, Arg3, Arg4),
    Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return new internal::FunctionClosure4<Arg1, Arg2, Arg3, Arg4>(function, true,
      arg1, arg2, arg3, arg4);
}
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2, Arg3, Arg4),
    Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return new internal::FunctionClosure4<Arg1, Arg2, Arg3, Arg4>(function,
      false, arg1, arg2, arg3, arg4);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2,
    Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return new internal::MethodClosure4<Class, Arg1, Arg2, Arg3, Arg4>(object,
      method, true, arg1, arg2, arg3, arg4);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1,
    Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return new internal::MethodClosure4<Class, Arg1, Arg2, Arg3, Arg4>(object,
      method, false, arg1, arg2, arg3, arg4);
}

// For args = 5
namespace internal {
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5>
class FunctionClosure5 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
      Arg5 arg5);

  FunctionClosure5(FunctionType function, bool self_deleting, Arg1 arg1,
      Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    : function_(function), self_deleting_(self_deleting), arg1_(arg1),
        arg2_(arg2), arg3_(arg3), arg4_(arg4), arg5_(arg5) {}
  virtual ~FunctionClosure5() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_, arg2_, arg3_, arg4_, arg5_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;
  Arg5 arg5_;
};

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5>
class MethodClosure5 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
      Arg5 arg5);

  MethodClosure5(Class* object, MethodType method, bool self_deleting,
      Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
      Arg5 arg5)    : object_(object), method_(method),
      self_deleting_(self_deleting), arg1_(arg1), arg2_(arg2), arg3_(arg3),
      arg4_(arg4), arg5_(arg5) {}
  virtual ~MethodClosure5() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_, arg2_, arg3_, arg4_, arg5_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;
  Arg5 arg5_;

};
}  // namespace internal

template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5>
inline Closure* NewCallback(void (*function)(Arg1, Arg2, Arg3, Arg4, Arg5),
    Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
  return new internal::FunctionClosure5<Arg1, Arg2, Arg3, Arg4, Arg5>(function,
      true, arg1, arg2, arg3, arg4, arg5);
}
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2, Arg3, Arg4,
    Arg5), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
  return new internal::FunctionClosure5<Arg1, Arg2, Arg3, Arg4, Arg5>(function,
      false, arg1, arg2, arg3, arg4, arg5);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2,
    Arg3, Arg4, Arg5), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
  return new internal::MethodClosure5<Class, Arg1, Arg2, Arg3, Arg4,
      Arg5>(object, method, true, arg1, arg2, arg3, arg4, arg5);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1,
    Arg2, Arg3, Arg4, Arg5), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
    Arg5 arg5) {
  return new internal::MethodClosure5<Class, Arg1, Arg2, Arg3, Arg4,
      Arg5>(object, method, false, arg1, arg2, arg3, arg4, arg5);
}

// For args = 6
namespace internal {
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5, typename Arg6>
class FunctionClosure6 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
      Arg5 arg5, Arg6 arg6);

  FunctionClosure6(FunctionType function, bool self_deleting, Arg1 arg1,
      Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
    : function_(function), self_deleting_(self_deleting), arg1_(arg1),
        arg2_(arg2), arg3_(arg3), arg4_(arg4), arg5_(arg5), arg6_(arg6) {}
  virtual ~FunctionClosure6() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_, arg2_, arg3_, arg4_, arg5_, arg6_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;
  Arg5 arg5_;
  Arg6 arg6_;
};

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5, typename Arg6>
class MethodClosure6 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
      Arg5 arg5, Arg6 arg6);

  MethodClosure6(Class* object, MethodType method, bool self_deleting,
      Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
      Arg6 arg6)    : object_(object), method_(method),
      self_deleting_(self_deleting), arg1_(arg1), arg2_(arg2), arg3_(arg3),
      arg4_(arg4), arg5_(arg5), arg6_(arg6) {}
  virtual ~MethodClosure6() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_, arg2_, arg3_, arg4_, arg5_, arg6_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;
  Arg5 arg5_;
  Arg6 arg6_;

};
}  // namespace internal

template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5, typename Arg6>
inline Closure* NewCallback(void (*function)(Arg1, Arg2, Arg3, Arg4, Arg5,
    Arg6), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6) {
  return new internal::FunctionClosure6<Arg1, Arg2, Arg3, Arg4, Arg5,
      Arg6>(function, true, arg1, arg2, arg3, arg4, arg5, arg6);
}
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5, typename Arg6>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2, Arg3, Arg4,
    Arg5, Arg6), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
    Arg6 arg6) {
  return new internal::FunctionClosure6<Arg1, Arg2, Arg3, Arg4, Arg5,
      Arg6>(function, false, arg1, arg2, arg3, arg4, arg5, arg6);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5, typename Arg6>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2,
    Arg3, Arg4, Arg5, Arg6), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
    Arg5 arg5, Arg6 arg6) {
  return new internal::MethodClosure6<Class, Arg1, Arg2, Arg3, Arg4, Arg5,
      Arg6>(object, method, true, arg1, arg2, arg3, arg4, arg5, arg6);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5, typename Arg6>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1,
    Arg2, Arg3, Arg4, Arg5, Arg6), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
    Arg5 arg5, Arg6 arg6) {
  return new internal::MethodClosure6<Class, Arg1, Arg2, Arg3, Arg4, Arg5,
      Arg6>(object, method, false, arg1, arg2, arg3, arg4, arg5, arg6);
}

// For args = 7
namespace internal {
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5, typename Arg6, typename Arg7>
class FunctionClosure7 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
      Arg5 arg5, Arg6 arg6, Arg7 arg7);

  FunctionClosure7(FunctionType function, bool self_deleting, Arg1 arg1,
      Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
    : function_(function), self_deleting_(self_deleting), arg1_(arg1),
        arg2_(arg2), arg3_(arg3), arg4_(arg4), arg5_(arg5), arg6_(arg6),
        arg7_(arg7) {}
  virtual ~FunctionClosure7() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_, arg2_, arg3_, arg4_, arg5_, arg6_, arg7_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;
  Arg5 arg5_;
  Arg6 arg6_;
  Arg7 arg7_;
};

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5, typename Arg6, typename Arg7>
class MethodClosure7 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
      Arg5 arg5, Arg6 arg6, Arg7 arg7);

  MethodClosure7(Class* object, MethodType method, bool self_deleting,
      Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6,
      Arg7 arg7)    : object_(object), method_(method),
      self_deleting_(self_deleting), arg1_(arg1), arg2_(arg2), arg3_(arg3),
      arg4_(arg4), arg5_(arg5), arg6_(arg6), arg7_(arg7) {}
  virtual ~MethodClosure7() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_, arg2_, arg3_, arg4_, arg5_, arg6_, arg7_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;
  Arg5 arg5_;
  Arg6 arg6_;
  Arg7 arg7_;

};
}  // namespace internal

template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5, typename Arg6, typename Arg7>
inline Closure* NewCallback(void (*function)(Arg1, Arg2, Arg3, Arg4, Arg5,
    Arg6, Arg7), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
    Arg6 arg6, Arg7 arg7) {
  return new internal::FunctionClosure7<Arg1, Arg2, Arg3, Arg4, Arg5, Arg6,
      Arg7>(function, true, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
    typename Arg5, typename Arg6, typename Arg7>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2, Arg3, Arg4,
    Arg5, Arg6, Arg7), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
    Arg6 arg6, Arg7 arg7) {
  return new internal::FunctionClosure7<Arg1, Arg2, Arg3, Arg4, Arg5, Arg6,
      Arg7>(function, false, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5, typename Arg6, typename Arg7>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2,
    Arg3, Arg4, Arg5, Arg6, Arg7), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
    Arg5 arg5, Arg6 arg6, Arg7 arg7) {
  return new internal::MethodClosure7<Class, Arg1, Arg2, Arg3, Arg4, Arg5,
      Arg6, Arg7>(object, method, true, arg1, arg2, arg3, arg4, arg5, arg6,
      arg7);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4, typename Arg5, typename Arg6, typename Arg7>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1,
    Arg2, Arg3, Arg4, Arg5, Arg6, Arg7), Arg1 arg1, Arg2 arg2, Arg3 arg3,
    Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7) {
  return new internal::MethodClosure7<Class, Arg1, Arg2, Arg3, Arg4, Arg5,
      Arg6, Arg7>(object, method, false, arg1, arg2, arg3, arg4, arg5, arg6,
      arg7);
}

} //namespace zrpc
#endif
