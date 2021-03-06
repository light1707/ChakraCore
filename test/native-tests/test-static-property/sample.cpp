//-------------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Copyright (c) 2021 ChakraCore Project Contributors. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------

#include "ChakraCore.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>

#define FAIL_CHECK(cmd)                     \
    do                                      \
    {                                       \
        JsErrorCode errCode = cmd;          \
        if (errCode != JsNoError)           \
        {                                   \
            printf("Error %d at '%s'\n",    \
                errCode, #cmd);             \
            return 1;                       \
        }                                   \
    } while(0)

using namespace std;

int main()
{
    JsRuntimeHandle runtime;
    JsContextRef context;
    JsValueRef result;
    unsigned currentSourceContext = 0;

    const char* script = "(()=>{return \'SUCCESS\';})()";

    // Create a runtime.
    JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime);

    // Create an execution context.
    JsCreateContext(runtime, &context);

    // Now set the current execution context.
    JsSetCurrentContext(context);

    JsValueRef fname;
    FAIL_CHECK(JsCreateString("sample", sizeof("sample") - 1, &fname));

    JsValueRef object;
    FAIL_CHECK(JsCreateObject(&object));

    JsValueRef scriptSource;
    FAIL_CHECK(JsCreateExternalArrayBuffer((void*)script, (unsigned int)strlen(script),
                                           nullptr, nullptr, &scriptSource));

    FAIL_CHECK(JsObjectSetProperty(object, fname, fname, false));

    JsValueRef fname_copy;
    FAIL_CHECK(JsObjectGetProperty(object, fname, &fname_copy));

    bool hasObject = false;
    FAIL_CHECK(JsObjectHasProperty(object, fname, &hasObject));

    bool hasOwn = false;
    FAIL_CHECK(JsObjectHasOwnProperty(object, fname, &hasOwn));

    JsValueRef pdesc;
    FAIL_CHECK(JsObjectGetOwnPropertyDescriptor(object, fname, &pdesc));

    if (!hasOwn || !hasObject || fname_copy != fname || pdesc == nullptr)
    {
        fprintf(stderr, "JsObject Has/Get/Set etc. Property... failed\n");
    }

    JsValueRef deletion_result;
    FAIL_CHECK(JsObjectDeleteProperty(object, fname, false, &deletion_result));

    FAIL_CHECK(JsObjectHasProperty(object, fname, &hasObject));

    bool delete_result = false;
    FAIL_CHECK(JsBooleanToBool(deletion_result, &delete_result));

    if (hasObject || delete_result == false)
    {
        fprintf(stderr, "JsObjectDeleteProperty failed\n");
    }

    // Run the script.
    FAIL_CHECK(JsRun(scriptSource, currentSourceContext++, fname, JsParseScriptAttributeNone, &result));

    // Convert your script result to String in JavaScript; redundant if your script returns a String
    JsValueRef resultJSString;
    FAIL_CHECK(JsConvertValueToString(result, &resultJSString));

    // Project script result back to C++.
    char *resultSTR = nullptr;
    size_t stringLength;
    FAIL_CHECK(JsCopyString(resultJSString, nullptr, 0, &stringLength));
    resultSTR = (char*) malloc(stringLength + 1);
    FAIL_CHECK(JsCopyString(resultJSString, resultSTR, stringLength, nullptr));
    resultSTR[stringLength] = 0;

    printf("Result -> %s \n", resultSTR);
    free(resultSTR);

    // Dispose runtime
    JsSetCurrentContext(JS_INVALID_REFERENCE);
    JsDisposeRuntime(runtime);

    return 0;
}
