import { Type } from "../types/Type";
export default class ClassField {
    readonly name: string;
    readonly type: Type;
    readonly description: string;
    constructor(name: string, type: Type, description: string);
}
