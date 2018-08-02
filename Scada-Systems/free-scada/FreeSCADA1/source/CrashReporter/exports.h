#ifdef CRASHREPORTER_EXPORTS
#	define CRASHREPORTER_API __declspec(dllexport)
#else
#	define CRASHREPORTER_API __declspec(dllimport)
#endif
