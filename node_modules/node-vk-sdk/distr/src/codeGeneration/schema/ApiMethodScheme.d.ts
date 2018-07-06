import ApiMethodParam from "./ApiMethodParam";
import { Type } from "../types/Type";
export default class ApiMethodScheme {
    readonly name: string;
    readonly params: ApiMethodParam[];
    readonly responseType: Type;
    readonly description: string;
    constructor(name: string, params: ApiMethodParam[], responseType: Type, description: string);
}
