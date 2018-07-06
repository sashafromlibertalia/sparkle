"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
class VKApiError {
    constructor(errorCode, errorMsg, requestParams) {
        this.errorCode = errorCode;
        this.errorMsg = errorMsg;
        this.requestParams = requestParams;
    }
    static deserialize(raw) {
        return new VKApiError(raw['error_code'], raw['error_msg'], raw['request_params']);
    }
}
exports.default = VKApiError;
//# sourceMappingURL=VKApiError.js.map