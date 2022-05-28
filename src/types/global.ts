export interface ICommand {
    name: string,
    description: string,
}

export interface IUglify {
    regex: RegExp,
    replaceTo: string,
}

export interface IAdmin {
    name: string;
    shortLink?: string;
    list: number[];
}

export interface IConfig {
    token: string;
    pollingGroupId: number;
    parserUrl: string;
    groupName: string;
    universityName: string;
    admin: IAdmin;
}
