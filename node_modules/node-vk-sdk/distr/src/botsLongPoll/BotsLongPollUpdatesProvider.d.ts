import { BaseUpdateProvider } from "./BaseUpdateProvider";
import { VKApi } from "../VKApi";
export declare class BotsLongPollUpdatesProvider implements BaseUpdateProvider {
    private api;
    private groupId;
    private server;
    private key;
    private ts;
    private updatesCallback;
    constructor(api: VKApi, groupId: number);
    getUpdates(callback: (update: any) => void): void;
    private init();
    private poll();
}
