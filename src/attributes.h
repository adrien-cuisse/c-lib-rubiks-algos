
#ifndef ATTRIBUTES_HEADER
#define ATTRIBUTES_HEADER

#if defined(__clang__) /* CLANG */
#	define IMPORTANT_RETURN __attribute__ ((warn_unused_result))
#elif defined(__GNUC__) || defined(__GNUG__) /* GCC */
#	define IMPORTANT_RETURN __attribute__ ((warn_unused_result))
#elif defined(_MSC_VER) /* MSVC */
#	error "Visibility not implemented for MSVC"
#elif defined(__MINGW32__) /* MinGW */
#	error "Visibility not implemented for MinGW"
#endif

#endif /* ATTRIBUTES_HEADER */
