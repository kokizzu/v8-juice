/* AUTO-GENERATED CODE! EDIT AT YOUR OWN RISK! */
#if !defined(DOXYGEN)

template <typename RV>
struct Signature< RV () >
{
    typedef RV ReturnType;
    enum { IsConst = 0 };
    typedef void Context;
    typedef RV (*FunctionType)();
    typedef tmp::NilType Head;
    typedef Head Tail;
};
template <typename RV>
struct Signature< RV (*)() > : Signature<RV ()>
{};

template <typename T, typename RV>
struct Signature< RV (T::*)() > : Signature<RV ()>
{
    typedef T Context;
    typedef RV (T::*FunctionType)();
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS

template <typename RV>
struct Signature< RV () const > : Signature<RV ()>
{
    enum { IsConst = 1 };
};
template <typename T, typename RV>
struct Signature< RV (T::*)() const > : Signature<RV () const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)() const;
};

#endif /* CVV8_CONFIG_ENABLE_CONST_OVERLOADS */
//! Specialization for 1 arg(s).
template <typename RV, typename A1>
struct Signature< RV (A1) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1);
    typedef A1 Head;
    typedef Signature< RV () > Tail;
};

//! Specialization for 1 arg(s).
template <typename RV, typename A1>
struct Signature< RV (*)(A1) > : Signature<RV (A1)>
{};

//! Specialization for T non-const methods taking 1 arg(s).
template <typename T, typename RV, typename A1>
struct Signature< RV (T::*)(A1) > : Signature<RV (A1)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 1 arg(s).
template <typename RV, typename A1>
struct Signature< RV (A1) const > : Signature<RV (A1)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 1 arg(s).
template <typename T, typename RV, typename A1>
struct Signature< RV (T::*)(A1) const > : Signature<RV (A1) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 2 arg(s).
template <typename RV, typename A1, typename A2>
struct Signature< RV (A1, A2) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2);
    typedef A1 Head;
    typedef Signature<RV ( A2)> Tail;
};

//! Specialization for 2 arg(s).
template <typename RV, typename A1, typename A2>
struct Signature< RV (*)(A1, A2) > : Signature<RV (A1, A2)>
{};

//! Specialization for T non-const methods taking 2 arg(s).
template <typename T, typename RV, typename A1, typename A2>
struct Signature< RV (T::*)(A1, A2) > : Signature<RV (A1, A2)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 2 arg(s).
template <typename RV, typename A1, typename A2>
struct Signature< RV (A1, A2) const > : Signature<RV (A1, A2)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 2 arg(s).
template <typename T, typename RV, typename A1, typename A2>
struct Signature< RV (T::*)(A1, A2) const > : Signature<RV (A1, A2) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 3 arg(s).
template <typename RV, typename A1, typename A2, typename A3>
struct Signature< RV (A1, A2, A3) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3)> Tail;
};

//! Specialization for 3 arg(s).
template <typename RV, typename A1, typename A2, typename A3>
struct Signature< RV (*)(A1, A2, A3) > : Signature<RV (A1, A2, A3)>
{};

//! Specialization for T non-const methods taking 3 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3>
struct Signature< RV (T::*)(A1, A2, A3) > : Signature<RV (A1, A2, A3)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 3 arg(s).
template <typename RV, typename A1, typename A2, typename A3>
struct Signature< RV (A1, A2, A3) const > : Signature<RV (A1, A2, A3)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 3 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3>
struct Signature< RV (T::*)(A1, A2, A3) const > : Signature<RV (A1, A2, A3) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 4 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4>
struct Signature< RV (A1, A2, A3, A4) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4)> Tail;
};

//! Specialization for 4 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4>
struct Signature< RV (*)(A1, A2, A3, A4) > : Signature<RV (A1, A2, A3, A4)>
{};

//! Specialization for T non-const methods taking 4 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4>
struct Signature< RV (T::*)(A1, A2, A3, A4) > : Signature<RV (A1, A2, A3, A4)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 4 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4>
struct Signature< RV (A1, A2, A3, A4) const > : Signature<RV (A1, A2, A3, A4)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 4 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4>
struct Signature< RV (T::*)(A1, A2, A3, A4) const > : Signature<RV (A1, A2, A3, A4) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 5 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct Signature< RV (A1, A2, A3, A4, A5) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5)> Tail;
};

//! Specialization for 5 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct Signature< RV (*)(A1, A2, A3, A4, A5) > : Signature<RV (A1, A2, A3, A4, A5)>
{};

//! Specialization for T non-const methods taking 5 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5) > : Signature<RV (A1, A2, A3, A4, A5)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 5 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct Signature< RV (A1, A2, A3, A4, A5) const > : Signature<RV (A1, A2, A3, A4, A5)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 5 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5) const > : Signature<RV (A1, A2, A3, A4, A5) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 6 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct Signature< RV (A1, A2, A3, A4, A5, A6) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6)> Tail;
};

//! Specialization for 6 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6) > : Signature<RV (A1, A2, A3, A4, A5, A6)>
{};

//! Specialization for T non-const methods taking 6 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6) > : Signature<RV (A1, A2, A3, A4, A5, A6)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 6 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct Signature< RV (A1, A2, A3, A4, A5, A6) const > : Signature<RV (A1, A2, A3, A4, A5, A6)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 6 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6) const > : Signature<RV (A1, A2, A3, A4, A5, A6) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 7 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7)> Tail;
};

//! Specialization for 7 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7)>
{};

//! Specialization for T non-const methods taking 7 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 7 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 7 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 8 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7, A8)> Tail;
};

//! Specialization for 8 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8)>
{};

//! Specialization for T non-const methods taking 8 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 8 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 8 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 9 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7, A8, A9)> Tail;
};

//! Specialization for 9 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{};

//! Specialization for T non-const methods taking 9 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 9 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 9 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 10 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10)> Tail;
};

//! Specialization for 10 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{};

//! Specialization for T non-const methods taking 10 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 10 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 10 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 11 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)> Tail;
};

//! Specialization for 11 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{};

//! Specialization for T non-const methods taking 11 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 11 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 11 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 12 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)> Tail;
};

//! Specialization for 12 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{};

//! Specialization for T non-const methods taking 12 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 12 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 12 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 13 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)> Tail;
};

//! Specialization for 13 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{};

//! Specialization for T non-const methods taking 13 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 13 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 13 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 14 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)> Tail;
};

//! Specialization for 14 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{};

//! Specialization for T non-const methods taking 14 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 14 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 14 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
//! Specialization for 15 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15);
    typedef A1 Head;
    typedef Signature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)> Tail;
};

//! Specialization for 15 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct Signature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)>
{};

//! Specialization for T non-const methods taking 15 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15);
};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
//! Specialization for 15 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct Signature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)>
{
    static const bool IsConst = true;
};

//! Specialization for T const methods taking 15 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct Signature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) const > : Signature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) const>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) const;
};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/
template <typename T, typename RV,  typename A0 >
struct MethodSignature< T, RV ( A0) > : Signature< RV (T::*)( A0) >
{
};
template <typename T, typename RV,  typename A0 >
struct MethodSignature< T, RV (*)( A0) > : MethodSignature< T, RV ( A0) >
{
};

template <typename T, typename RV,  typename A0 >
struct MethodSignature< T, RV (T::*)( A0) > :
    MethodSignature< T, RV ( A0) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0 >
struct MethodSignature< T, RV ( A0) const > : Signature< RV (T::*)( A0) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0 >
struct MethodSignature< T, RV (T::*)( A0) const > :
    MethodSignature< T, RV ( A0) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0 >
struct ConstMethodSignature< T, RV ( A0) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0) const > {};
#else
    Signature< RV (T::*)( A0) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0 >
struct ConstMethodSignature< T, RV ( A0) > : ConstMethodSignature< T, RV ( A0) const >
{
};
#endif

template <typename T, typename RV,  typename A0 >
struct ConstMethodSignature< T, RV (T::*)( A0) const > :
    ConstMethodSignature< T, RV ( A0) const >
{};

template <typename T, typename RV,  typename A0, typename A1 >
struct MethodSignature< T, RV ( A0, A1) > : Signature< RV (T::*)( A0, A1) >
{
};
template <typename T, typename RV,  typename A0, typename A1 >
struct MethodSignature< T, RV (*)( A0, A1) > : MethodSignature< T, RV ( A0, A1) >
{
};

template <typename T, typename RV,  typename A0, typename A1 >
struct MethodSignature< T, RV (T::*)( A0, A1) > :
    MethodSignature< T, RV ( A0, A1) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1 >
struct MethodSignature< T, RV ( A0, A1) const > : Signature< RV (T::*)( A0, A1) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1 >
struct MethodSignature< T, RV (T::*)( A0, A1) const > :
    MethodSignature< T, RV ( A0, A1) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1 >
struct ConstMethodSignature< T, RV ( A0, A1) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1) const > {};
#else
    Signature< RV (T::*)( A0, A1) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1 >
struct ConstMethodSignature< T, RV ( A0, A1) > : ConstMethodSignature< T, RV ( A0, A1) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1) const > :
    ConstMethodSignature< T, RV ( A0, A1) const >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct MethodSignature< T, RV ( A0, A1, A2) > : Signature< RV (T::*)( A0, A1, A2) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct MethodSignature< T, RV (*)( A0, A1, A2) > : MethodSignature< T, RV ( A0, A1, A2) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2) > :
    MethodSignature< T, RV ( A0, A1, A2) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct MethodSignature< T, RV ( A0, A1, A2) const > : Signature< RV (T::*)( A0, A1, A2) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2) const > :
    MethodSignature< T, RV ( A0, A1, A2) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct ConstMethodSignature< T, RV ( A0, A1, A2) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1, A2) const > {};
#else
    Signature< RV (T::*)( A0, A1, A2) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1, A2) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct ConstMethodSignature< T, RV ( A0, A1, A2) > : ConstMethodSignature< T, RV ( A0, A1, A2) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1, A2) const > :
    ConstMethodSignature< T, RV ( A0, A1, A2) const >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct MethodSignature< T, RV ( A0, A1, A2, A3) > : Signature< RV (T::*)( A0, A1, A2, A3) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct MethodSignature< T, RV (*)( A0, A1, A2, A3) > : MethodSignature< T, RV ( A0, A1, A2, A3) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3) > :
    MethodSignature< T, RV ( A0, A1, A2, A3) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct MethodSignature< T, RV ( A0, A1, A2, A3) const > : Signature< RV (T::*)( A0, A1, A2, A3) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3) const > :
    MethodSignature< T, RV ( A0, A1, A2, A3) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1, A2, A3) const > {};
#else
    Signature< RV (T::*)( A0, A1, A2, A3) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1, A2, A3) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3) > : ConstMethodSignature< T, RV ( A0, A1, A2, A3) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3) const > :
    ConstMethodSignature< T, RV ( A0, A1, A2, A3) const >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4) > : Signature< RV (T::*)( A0, A1, A2, A3, A4) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct MethodSignature< T, RV (*)( A0, A1, A2, A3, A4) > : MethodSignature< T, RV ( A0, A1, A2, A3, A4) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4) > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4) const > : Signature< RV (T::*)( A0, A1, A2, A3, A4) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4) const > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1, A2, A3, A4) const > {};
#else
    Signature< RV (T::*)( A0, A1, A2, A3, A4) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1, A2, A3, A4) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4) > : ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4) const > :
    ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4) const >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct MethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5) > : MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5) > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) const > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5) const > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5) const > {};
#else
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1, A2, A3, A4, A5) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) > : ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5) const > :
    ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) const >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct MethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5, A6) > : MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6) > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) const > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6) const > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6) const > {};
#else
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1, A2, A3, A4, A5, A6) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) > : ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6) const > :
    ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) const >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct MethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5, A6, A7) > : MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) const > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) const > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) const > {};
#else
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1, A2, A3, A4, A5, A6, A7) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) > : ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) const > :
    ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) const >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct MethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) > : MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) const > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const > {};
#else
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) > : ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const > :
    ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) const >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct MethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > : MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{};

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const > : Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const >
{
    enum { IsConst = 1 };
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct MethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const > :
    MethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const >
{};
#endif /*CVV8_CONFIG_ENABLE_CONST_OVERLOADS*/

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const > :
#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const > {};
#else
    Signature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
    //enum { IsConst = 1 };
    static const bool IsConst = true;
    typedef RV (T::*FunctionType)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const;
};
#endif

#if defined(CVV8_CONFIG_ENABLE_CONST_OVERLOADS) && CVV8_CONFIG_ENABLE_CONST_OVERLOADS
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > : ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const >
{
};
#endif

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct ConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const > :
    ConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const >
{};

#endif // if !defined(DOXYGEN)
