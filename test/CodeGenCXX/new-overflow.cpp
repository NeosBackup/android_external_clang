// RUN: %clang_cc1 -triple i386-unknown-unknown %s -emit-llvm -o - | FileCheck %s

// rdar://problem/9246208

// CHECK: [[OVR_T:%.*]] = type { i32, i1 }

// Basic test.
namespace test0 {
  struct A {
    A();
    int x;
  };

  typedef A elt;

  // CHECK:    define [[A:%.*]]* @_ZN5test04testEs(i16 signext
  // CHECK:      [[N:%.*]] = sext i16 {{%.*}} to i32
  // CHECK-NEXT: [[T0:%.*]] = call [[OVR_T]] @llvm.umul.with.overflow.i32(i32 [[N]], i32 4)
  // CHECK-NEXT: [[T1:%.*]] = extractvalue [[OVR_T]] [[T0]], 1
  // CHECK-NEXT: [[T2:%.*]] = extractvalue [[OVR_T]] [[T0]], 0
  // CHECK-NEXT: [[T3:%.*]] = select i1 [[T1]], i32 -1, i32 [[T2]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T3]])
  // CHECK:      icmp ult i32 {{.*}}, [[N]]
  elt *test(short s) {
    return new elt[s];
  }
}

// test0 with a nested array.
namespace test1 {
  struct A {
    A();
    int x;
  };

  typedef A elt[100];

  // CHECK:    define [100 x [[A:%.*]]]* @_ZN5test14testEs(i16 signext
  // CHECK:      [[N:%.*]] = sext i16 {{%.*}} to i32
  // CHECK-NEXT: [[T0:%.*]] = call [[OVR_T]] @llvm.umul.with.overflow.i32(i32 [[N]], i32 400)
  // CHECK-NEXT: [[T1:%.*]] = extractvalue [[OVR_T]] [[T0]], 1
  // CHECK-NEXT: [[T2:%.*]] = extractvalue [[OVR_T]] [[T0]], 0
  // CHECK-NEXT: [[T3:%.*]] = mul i32 [[N]], 100
  // CHECK-NEXT: [[T4:%.*]] = select i1 [[T1]], i32 -1, i32 [[T2]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T4]])
  // CHECK:      icmp ult i32 {{.*}}, [[T3]]
  elt *test(short s) {
    return new elt[s];
  }
}

// test1 with an array cookie.
namespace test2 {
  struct A {
    A();
    ~A();
    int x;
  };

  typedef A elt[100];

  // CHECK:    define [100 x [[A:%.*]]]* @_ZN5test24testEs(i16 signext
  // CHECK:      [[N:%.*]] = sext i16 {{%.*}} to i32
  // CHECK-NEXT: [[T0:%.*]] = call [[OVR_T]] @llvm.umul.with.overflow.i32(i32 [[N]], i32 400)
  // CHECK-NEXT: [[T1:%.*]] = extractvalue [[OVR_T]] [[T0]], 1
  // CHECK-NEXT: [[T2:%.*]] = extractvalue [[OVR_T]] [[T0]], 0
  // CHECK-NEXT: [[T3:%.*]] = mul i32 [[N]], 100
  // CHECK-NEXT: [[T4:%.*]] = call [[OVR_T]] @llvm.uadd.with.overflow.i32(i32 [[T2]], i32 4)
  // CHECK-NEXT: [[T5:%.*]] = extractvalue [[OVR_T]] [[T4]], 1
  // CHECK-NEXT: [[T6:%.*]] = or i1 [[T1]], [[T5]]
  // CHECK-NEXT: [[T7:%.*]] = extractvalue [[OVR_T]] [[T4]], 0
  // CHECK-NEXT: [[T8:%.*]] = select i1 [[T6]], i32 -1, i32 [[T7]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T8]])
  // CHECK:      icmp ult i32 {{.*}}, [[T3]]
  elt *test(short s) {
    return new elt[s];
  }
}

// test0 with a 1-byte element.
namespace test4 {
  struct A {
    A();
  };

  typedef A elt;

  // CHECK:    define [[A:%.*]]* @_ZN5test44testEs(i16 signext
  // CHECK:      [[N:%.*]] = sext i16 {{%.*}} to i32
  // CHECK-NEXT: [[T0:%.*]] = icmp slt i32 [[N]], 0
  // CHECK-NEXT: [[T1:%.*]] = select i1 [[T0]], i32 -1, i32 [[N]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T1]])
  // CHECK:      icmp ult i32 {{.*}}, [[N]]
  elt *test(short s) {
    return new elt[s];
  }
}

// test4 with no sext required.
namespace test5 {
  struct A {
    A();
  };

  typedef A elt;

  // CHECK:    define [[A:%.*]]* @_ZN5test54testEi(i32
  // CHECK:      [[N:%.*]] = load i32*
  // CHECK-NEXT: [[T0:%.*]] = icmp slt i32 [[N]], 0
  // CHECK-NEXT: [[T1:%.*]] = select i1 [[T0]], i32 -1, i32 [[N]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T1]])
  // CHECK:      icmp ult i32 {{.*}}, [[N]]
  elt *test(int s) {
    return new elt[s];
  }
}

// test0 with an unsigned size.
namespace test6 {
  struct A {
    A();
    int x;
  };

  typedef A elt;

  // CHECK:    define [[A:%.*]]* @_ZN5test64testEt(i16 zeroext
  // CHECK:      [[N:%.*]] = zext i16 {{%.*}} to i32
  // CHECK-NEXT: [[T0:%.*]] = call [[OVR_T]] @llvm.umul.with.overflow.i32(i32 [[N]], i32 4)
  // CHECK-NEXT: [[T1:%.*]] = extractvalue [[OVR_T]] [[T0]], 1
  // CHECK-NEXT: [[T2:%.*]] = extractvalue [[OVR_T]] [[T0]], 0
  // CHECK-NEXT: [[T3:%.*]] = select i1 [[T1]], i32 -1, i32 [[T2]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T3]])
  // CHECK:      icmp ult i32 {{.*}}, [[N]]
  elt *test(unsigned short s) {
    return new elt[s];
  }
}

// test1 with an unsigned size.
namespace test7 {
  struct A {
    A();
    int x;
  };

  typedef A elt[100];

  // CHECK:    define [100 x [[A:%.*]]]* @_ZN5test74testEt(i16 zeroext
  // CHECK:      [[N:%.*]] = zext i16 {{%.*}} to i32
  // CHECK-NEXT: [[T0:%.*]] = call [[OVR_T]] @llvm.umul.with.overflow.i32(i32 [[N]], i32 400)
  // CHECK-NEXT: [[T1:%.*]] = extractvalue [[OVR_T]] [[T0]], 1
  // CHECK-NEXT: [[T2:%.*]] = extractvalue [[OVR_T]] [[T0]], 0
  // CHECK-NEXT: [[T3:%.*]] = mul i32 [[N]], 100
  // CHECK-NEXT: [[T4:%.*]] = select i1 [[T1]], i32 -1, i32 [[T2]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T4]])
  // CHECK:      icmp ult i32 {{.*}}, [[T3]]
  elt *test(unsigned short s) {
    return new elt[s];
  }
}

// test0 with a signed type larger than size_t.
namespace test8 {
  struct A {
    A();
    int x;
  };

  typedef A elt;

  // CHECK:    define [[A:%.*]]* @_ZN5test84testEx(i64
  // CHECK:      [[N:%.*]] = load i64*
  // CHECK-NEXT: [[T0:%.*]] = icmp uge i64 [[N]], 4294967296
  // CHECK-NEXT: [[T1:%.*]] = trunc i64 [[N]] to i32
  // CHECK-NEXT: [[T2:%.*]] = call [[OVR_T]] @llvm.umul.with.overflow.i32(i32 [[T1]], i32 4)
  // CHECK-NEXT: [[T3:%.*]] = extractvalue [[OVR_T]] [[T2]], 1
  // CHECK-NEXT: [[T4:%.*]] = or i1 [[T0]], [[T3]]
  // CHECK-NEXT: [[T5:%.*]] = extractvalue [[OVR_T]] [[T2]], 0
  // CHECK-NEXT: [[T6:%.*]] = select i1 [[T4]], i32 -1, i32 [[T5]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T6]])
  // CHECK:      icmp ult i32 {{.*}}, [[T1]]
  elt *test(long long s) {
    return new elt[s];
  }
}

// test8 with an unsigned type.
namespace test9 {
  struct A {
    A();
    int x;
  };

  typedef A elt;

  // CHECK:    define [[A:%.*]]* @_ZN5test94testEy(i64
  // CHECK:      [[N:%.*]] = load i64*
  // CHECK-NEXT: [[T0:%.*]] = icmp uge i64 [[N]], 4294967296
  // CHECK-NEXT: [[T1:%.*]] = trunc i64 [[N]] to i32
  // CHECK-NEXT: [[T2:%.*]] = call [[OVR_T]] @llvm.umul.with.overflow.i32(i32 [[T1]], i32 4)
  // CHECK-NEXT: [[T3:%.*]] = extractvalue [[OVR_T]] [[T2]], 1
  // CHECK-NEXT: [[T4:%.*]] = or i1 [[T0]], [[T3]]
  // CHECK-NEXT: [[T5:%.*]] = extractvalue [[OVR_T]] [[T2]], 0
  // CHECK-NEXT: [[T6:%.*]] = select i1 [[T4]], i32 -1, i32 [[T5]]
  // CHECK-NEXT: call noalias i8* @_Znaj(i32 [[T6]])
  // CHECK:      icmp ult i32 {{.*}}, [[T1]]
  elt *test(unsigned long long s) {
    return new elt[s];
  }
}