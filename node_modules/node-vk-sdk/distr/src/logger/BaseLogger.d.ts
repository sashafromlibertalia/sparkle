export interface BaseLogger {
    log(prefix: string, data?: any): any;
    warn(prefix: string, data?: any): any;
    error(prefix: string, data?: any): any;
}
