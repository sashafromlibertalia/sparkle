"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
class CallbackQueue {
    constructor(countPerSec) {
        this._queue = [];
        this._delay = 1000 / countPerSec;
        setInterval(() => {
            if (this._queue.length !== 0) {
                const func = this._queue.shift();
                if (func)
                    func();
            }
        }, this._delay);
    }
    push(func) {
        this._queue.push(func);
    }
}
exports.default = CallbackQueue;
//# sourceMappingURL=CallbackQueue.js.map