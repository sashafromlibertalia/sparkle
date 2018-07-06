import { BaseLogger } from "./BaseLogger";
export declare class ConsoleLogger implements BaseLogger {
    log(prefix: string, data?: any): void;
    warn(prefix: string, data?: any): void;
    error(prefix: string, data?: any): void;
    private prepareLog(color, tag, prefix, data?);
}
