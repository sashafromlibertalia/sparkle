"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
class CodeLine {
    constructor(data, tab = 0) {
        this._data = this.genTab(tab) + data;
    }
    get data() {
        return this._data;
    }
    tab(n) {
        this._data = this.genTab(n) + this._data;
    }
    genTab(n) {
        return new Array(n).fill('    ').join('');
    }
}
exports.default = CodeLine;
//# sourceMappingURL=CodeLine.js.map