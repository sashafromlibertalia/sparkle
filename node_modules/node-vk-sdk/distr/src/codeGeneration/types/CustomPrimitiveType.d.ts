import { Type } from "./Type";
export default class CustomPrimitiveType implements Type {
    readonly name: string;
    constructor(name: string);
}
