"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const util_1 = require("util");
class ColorCodes {
}
ColorCodes.RED = '\x1b[31m';
ColorCodes.CYAN = '\x1b[36m';
ColorCodes.YELLOW = '\x1b[33m';
ColorCodes.RESET = '\x1b[0m';
class ConsoleLogger {
    log(prefix, data) {
        this.prepareLog(ColorCodes.CYAN, 'log', prefix, data);
    }
    warn(prefix, data) {
        this.prepareLog(ColorCodes.YELLOW, 'warn', prefix, data);
    }
    error(prefix, data) {
        this.prepareLog(ColorCodes.RED, 'error', prefix, data);
    }
    prepareLog(color, tag, prefix, data) {
        const time = new Date().toTimeString().replace(/.*(\d{2}:\d{2}:\d{2}).*/, '$1');
        process.stdout.write(`${time} ${color}[${tag}]${ColorCodes.RESET} ${prefix} ${util_1.format(data || '')} \n`);
    }
}
exports.ConsoleLogger = ConsoleLogger;
//# sourceMappingURL=ConsoleLogger.js.map