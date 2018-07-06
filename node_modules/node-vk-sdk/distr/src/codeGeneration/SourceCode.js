"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const CodeLine_1 = require("./CodeLine");
class SourceCode {
    constructor(lines = []) {
        this._lines = lines;
    }
    add(data, tab = 0) {
        this._lines.push(new CodeLine_1.default(data, tab));
    }
    append(code, tab = 0) {
        code.tab(tab);
        this._lines.push(...code.lines);
    }
    render() {
        return this._lines.map(line => line.data).join('\n');
    }
    tab(n) {
        this._lines.forEach(line => line.tab(n));
    }
    get lines() {
        return this._lines;
    }
    genTab(n) {
        return new Array(n).join('    ');
    }
}
exports.default = SourceCode;
//# sourceMappingURL=SourceCode.js.map