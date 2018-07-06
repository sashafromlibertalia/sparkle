import ClassField from "./ClassField";
import ClassScheme from "./ClassScheme";
export default class AliasClassScheme extends ClassScheme {
    readonly name: string;
    readonly fields: ClassField[];
    readonly aliasClass: ClassScheme;
    constructor(name: string, fields: ClassField[], aliasClass: ClassScheme);
}
