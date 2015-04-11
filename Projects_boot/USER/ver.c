
#if  defined(DMP_TYPE_INPUT)
const char Ver[] = { "SoftVer-B.SINA.PB-A.01.00.00&"__DATE__ };
#elif defined(DMP_TYPE_OUTPUT)
const char Ver[] = { "SoftVer-B.SQFA.PB-A.01.00.00&"__DATE__ };
#elif defined(DMP_TYPE_MOTOR)
const char Ver[] = { "SoftVer-B.SBJA.PB-A.01.00.00&"__DATE__ };
#else
#error "must define DMP_DEV_XXX"
#endif