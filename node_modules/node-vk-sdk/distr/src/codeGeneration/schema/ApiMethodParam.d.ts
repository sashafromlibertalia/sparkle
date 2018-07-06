import { Type } from "../types/Type";
export default class ApiMethodParam {
    readonly name: string;
    readonly type: Type;
    readonly required: boolean;
    readonly description: string;
    constructor(name: string, type: Type, required: boolean, description: string);
}
