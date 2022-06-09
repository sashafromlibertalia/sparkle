export {};
declare global {
    interface IConfig {
        token: string;
        pollingGroupId: number;
    }

    interface ICommand {
        name: string,
        description: string,
    }

    interface IUglify {
        regex: RegExp,
        replaceTo: string,
    }

    interface IUser {
        first_name: string;
        last_name: string;
    }
}
