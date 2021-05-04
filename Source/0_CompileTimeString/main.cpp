#include <string_view>

template<typename T> constexpr auto GetTypeNameHelperFunc(T);

template<typename CharType, CharType... chars>
struct CTStringBase
{
	static constexpr std::basic_string_view<CharType> View() { return Data;  }
	static constexpr CharType Data[] = { chars..., CharType(0) };
	static constexpr size_t Size = sizeof...(chars);
	using ValueType = CharType;
};

template<typename T, size_t ... Ind>
constexpr auto CTStringHelper(std::index_sequence<Ind...>)
{
	return CTStringBase<decltype(T::Get())::value_type, T::Get()[Ind]...>{};
}

template<typename T>
constexpr auto CTString(T)
{
	return CTStringHelper<T>(std::make_index_sequence<T::Get().size()>());
}

#define CTString(s)																				\
([]{																										\
struct Tmp																							\
{																											\
static constexpr auto Get() { return std::basic_string_view{s};}			\
};																											\
return CTString(Tmp{});																		\
}())

template<size_t N, typename CharType, CharType FirstChar, CharType ... RemainChars>
constexpr auto RemovePrefix(CTStringBase<CharType, FirstChar, RemainChars...>)
{
	/*
	using StringType = CTStringBase<CharType, FirstChar, RemainChars...>;
	if constexpr (StringType::Size < N)
		return CTStringBase<CharType>();
	else if constexpr (N == 0)
		return CTStringBase<CharType, FirstChar, RemainChars...>();
	else
		return RemovePrefix<N - 1>(CTStringBase<CharType, RemainChars...>());
	*/
	using StringType = CTStringBase<CharType, FirstChar, RemainChars...>;
	if constexpr (StringType::Size < N)
		return CTStringBase<CharType>();
	else
		return CTString(StringType::View().data() + N);
}

template<typename CharType, CharType ... chars1, CharType ... chars2>
constexpr auto Concat(CTStringBase<CharType, chars1...>, CTStringBase<CharType, chars2...>)
{
	return CTStringBase<CharType, chars1..., chars2...>();
}

template<size_t N, typename CharType, CharType FirstChar, CharType ... RemainChars>
constexpr auto RemoveSuffix(CTStringBase<CharType, FirstChar, RemainChars...>)
{
	using StringType = CTStringBase<CharType, FirstChar, RemainChars...>;
	if constexpr (StringType::Size <= N)
		return CTStringBase<CharType>();
	else
		return Concat(CTStringBase<CharType, FirstChar>(), RemoveSuffix<N>(CTStringBase<CharType, RemainChars...>()));
}

template<typename Str1, typename Str2, typename ... Strs>
constexpr auto Concat(Str1, Str2, Strs...)
{
	return Concat(Concat(Str1(), Str2()), Strs()...);
}

template<typename T, size_t ... ind>
constexpr auto GetTypeNameFunc(T t, std::index_sequence<ind...>)
{
	constexpr std::basic_string_view View = __FUNCSIG__;
	auto res = CTStringBase<char, View[ind]...>();
	constexpr size_t suffixLength = sizeof...(ind) - 21 - (sizeof...(ind) - 26) / 2;
	return RemoveSuffix<suffixLength>(RemovePrefix<21>(res));
}

template<typename T>
constexpr auto GetTypeNameFunc(T t)
{
	constexpr std::basic_string_view View = __FUNCSIG__;
	std::index_sequence<View.size()> seq;
	return GetTypeNameFunc(T(), std::make_index_sequence<View.size()>());
}

template<typename T>
struct ABCDEFG
{

};

int main()
{
	auto s = CTString("baabs");
	auto ss = RemovePrefix<1>(s);
	auto sss = RemoveSuffix<3>(s);
	auto ssss = Concat(s, ss, sss);

	auto testType = GetTypeNameFunc(ABCDEFG<int>());
	auto str = testType.Data;
	testType.Size;
	return 0;
}