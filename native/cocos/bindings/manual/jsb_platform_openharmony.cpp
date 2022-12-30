/****************************************************************************
 Copyright (c) 2022 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include "jsb_platform.h"

#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "cocos/bindings/manual/jsb_global_init.h"
#include "cocos/platform/FileUtils.h"

#include <regex>

#if CC_USE_ETS
#include <plugins/ets/runtime/ets_vm_api.h>
#include <plugins/ets/runtime/ts2ets/ts2ets.h>
#include <sstream>
#endif

using namespace cc;

static std::unordered_map<std::string, std::string> _fontFamilyNameMap;

const std::unordered_map<std::string, std::string> &getFontFamilyNameMap() {
    return _fontFamilyNameMap;
}

static bool JSB_loadFont(se::State &s) {
    // TODO(qgh):Currently it does not support loading OpenHarmony fonts, it may be supported in the future
    return true;
}
SE_BIND_FUNC(JSB_loadFont)

#if CC_USE_ETS
static bool JSB_createEtsRuntime(se::State &s) {
    const auto &args = s.args();
    size_t argc = args.size();
    if (argc != 1) {
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
        return false;
    }

    bool ok = true;
    ccstd::string path;
    ok &= sevalue_to_native(args[0], &path);
    SE_PRECONDITION2(ok, false, "Error processing arguments");

    std::stringstream stdlib_abc;
    stdlib_abc << path << "/etsstdlib.abc";
    std::stringstream path_abc;
    path_abc << path << "/index.abc";

    panda::ets::CreateRuntime(stdlib_abc.str().c_str(), path_abc.str().c_str(), true, true);

    return true;
}

static bool JSB_destroyEtsRuntime(se::State &s) {
    const auto &args = s.args();
    size_t argc = args.size();
    if (argc != 0) {
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
        return false;
    }

    panda::ets::ExecuteMain();
    return true;
}

static bool JSB_executeEtsMain() {
    const auto &args = s.args();
    size_t argc = args.size();
    if (argc != 0) {
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
        return false;
    }

    panda::ets::ExecuteMain();
    return true;
}

static napi_value _SE(JSB_invokeEtsMethod)(napi_env env, napi_callback_info info) {
    size_t argc = 0;
    napi_status status;
    NODE_API_CALL(status, env, napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr));

    std::vector<napi_value> args(argc);
    NODE_API_CALL(status, env, napi_get_cb_info(env, info, &argc, args.data(), nullptr, nullptr));

    return InvokeEtsMethodImpl(env, args.data(), args.size(), false);
}

#endif

bool register_platform_bindings(se::Object *obj) {
    __jsbObj->defineFunction("loadFont", _SE(JSB_loadFont));

#if CC_USE_ETS
    __jsbObj->defineFunction("createEtsRuntime", _SE(JSB_createEtsRuntime));
    __jsbObj->defineFunction("destroyEtsRuntime", _SE(JSB_destroyEtsRuntime));
    __jsbObj->defineFunction("executeEtsMain", _SE(JSB_executeEtsMain));
    __jsbObj->defineFunction("invokeEtsMethod", _SE(JSB_invokeEtsMethod));
#endif
    return true;
}
