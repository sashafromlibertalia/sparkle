"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ClassScheme_1 = require("./ClassScheme");
class AliasClassScheme extends ClassScheme_1.default {
    constructor(name, fields, aliasClass) {
        super(name, fields);
        this.name = name;
        this.fields = fields;
        this.aliasClass = aliasClass;
    }
}
exports.default = AliasClassScheme;
//# sourceMappingURL=AliasClassScheme.js.map