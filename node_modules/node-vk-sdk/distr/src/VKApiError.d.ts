export default class VKApiError {
    readonly errorCode: number;
    readonly errorMsg: number;
    readonly requestParams: any;
    constructor(errorCode: number, errorMsg: number, requestParams: any);
    static deserialize(raw: Object): VKApiError;
}
