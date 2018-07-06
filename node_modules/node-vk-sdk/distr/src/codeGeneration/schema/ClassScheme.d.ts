import ClassField from "./ClassField";
export default class ClassScheme {
    readonly name: string;
    readonly fields: ClassField[];
    constructor(name: string, fields: ClassField[]);
}
