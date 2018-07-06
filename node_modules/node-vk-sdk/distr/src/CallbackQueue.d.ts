export default class CallbackQueue {
    private _delay;
    private _queue;
    constructor(countPerSec: number);
    push(func: Function): void;
}
