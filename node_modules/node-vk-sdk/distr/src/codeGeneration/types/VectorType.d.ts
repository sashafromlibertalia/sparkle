import { Type } from "./Type";
export default class VectorType implements Type {
    readonly item: Type;
    constructor(item: Type);
}
