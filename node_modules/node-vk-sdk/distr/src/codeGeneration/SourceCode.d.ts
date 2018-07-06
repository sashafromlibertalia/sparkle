import CodeLine from "./CodeLine";
export default class SourceCode {
    private _lines;
    constructor(lines?: CodeLine[]);
    add(data: string, tab?: number): void;
    append(code: SourceCode, tab?: number): void;
    render(): string;
    tab(n: number): void;
    readonly lines: CodeLine[];
    private genTab(n);
}
