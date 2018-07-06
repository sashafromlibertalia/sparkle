import { Type } from "../types/Type";
/**
 * Represents serializable primitive
 *
 *
 */
export default class CustomPrimitiveScheme {
    readonly name: string;
    readonly type: Type;
    constructor(name: string, type: Type);
}
