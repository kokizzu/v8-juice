#if !defined(DOXYGEN)
// auto-generated [Mon Oct 26 22:57:46 2009] by maketypelist.pl
struct TypeList_0 : TypePair<NilType,NilType> {};

template <class T1>
struct TypeList_1 : TypePair<T1,NilType> {};

template <class T1, class T2>
struct TypeList_2 : TypePair<T1, TypePair<T2, NilType> > {};

template <class T1, class T2, class T3 >
struct TypeList_3 : TypePair<T1, TypeList_2< T2, T3 > > {};

template <class T1, class T2, class T3, class T4 >
struct TypeList_4 : TypePair<T1, TypeList_3< T2, T3, T4 > > {};

template <class T1, class T2, class T3, class T4, class T5 >
struct TypeList_5 : TypePair<T1, TypeList_4< T2, T3, T4, T5 > > {};

template <class T1 = NilType, class T2 = NilType, class T3 = NilType, class T4 = NilType, class T5 = NilType>
struct TypeList : TypeList_05<T1, T2, T3, T4, T5> {};

template <class T1, class T2, class T3, class T4>
struct TypeList <T1, T2, T3, T4, NilType> : TypeList_4<T1, T2, T3, T4> {};

template <class T1, class T2, class T3>
struct TypeList <T1, T2, T3, NilType, NilType> : TypeList_3<T1, T2, T3> {};

template <class T1, class T2>
struct TypeList<T1, T2, NilType, NilType, NilType> : TypeList_2<T1,T2> {};

template <class T1>
struct TypeList<T1, NilType, NilType, NilType, NilType> : TypeList_1<T1> {};

template <>
struct TypeList<> : TypePair<NilType,NilType> {};

#endif // if !defined(DOXYGEN)
