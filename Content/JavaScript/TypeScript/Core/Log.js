"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Log = void 0;
class Log {
    static info(message) {
        console.log(`[Ays] ${message}`);
    }
    static warn(message) {
        console.warn(`[Ays] ${message}`);
    }
    static error(message) {
        console.error(`[Ays] ${message}`);
    }
}
exports.Log = Log;
//# sourceMappingURL=Log.js.map