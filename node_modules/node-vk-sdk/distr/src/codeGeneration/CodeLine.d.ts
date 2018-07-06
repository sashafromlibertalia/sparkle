export default class CodeLine {
    private _data;
    constructor(data: string, tab?: number);
    readonly data: string;
    tab(n: number): void;
    private genTab(n);
}
