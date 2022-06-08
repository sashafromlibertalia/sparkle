export {}
declare global {
    interface IConfig {
        token: string;
        pollingGroupId: number;
        parserUrl: string;
        groupName: string;
        universityName: string;
        admin: IAdmin;
    }

    interface ICommand {
        name: string,
        description: string,
    }

    interface IUglify {
        regex: RegExp,
        replaceTo: string,
    }

    interface IAdmin {
        name: string;
        shortLink?: string;
        list: number[];
    }

    interface IUser {
        first_name: string;
        last_name: string;
    }
}
