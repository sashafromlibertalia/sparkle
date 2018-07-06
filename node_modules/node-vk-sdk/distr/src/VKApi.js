"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : new P(function (resolve) { resolve(result.value); }).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
const CallbackQueue_1 = require("./CallbackQueue");
const VKApiError_1 = require("./VKApiError");
const req = require('tiny_request');
const REQUESTS_PER_SECOND = 3;
const TIMEOUT = 5000; // 5 seconds
const API_BASE_URL = 'https://api.vk.com/method/';
const API_VERSION = '5.73';
class VKApi {
    constructor(options) {
        this._logger = options.logger;
        this._token = options.token;
        this._timeout = options.timeout || TIMEOUT;
        this._lang = options.lang;
        this._testMode = options.testMode;
        if (options.useQueue)
            this._queue = new CallbackQueue_1.default(options.requestsPerSecond || REQUESTS_PER_SECOND);
    }
    call(method, params) {
        return __awaiter(this, void 0, void 0, function* () {
            params = this.filterParams(params);
            if (params['lang'] == undefined && this._lang != undefined)
                params['lang'] = this._lang;
            if (params['testMode'] == undefined && this._testMode != undefined)
                params['testMode'] = this._testMode;
            params['v'] = API_VERSION;
            params['access_token'] = params['access_token'] || this._token;
            if (params['access_token'] == undefined)
                delete params['access_token'];
            return new Promise((resolve, reject) => {
                let reqFunc = () => {
                    req.post({
                        url: API_BASE_URL + method,
                        query: params,
                        json: true,
                        timeout: this._timeout
                    }, (body, response, err) => {
                        this.handleResponse(method, params, body, response, err, resolve, reject);
                    });
                };
                if (this._queue)
                    this._queue.push(reqFunc);
                else
                    reqFunc();
            });
        });
    }
    /**
     * Makes api call and if there was
     * server-side error or requests limit was reached
     * repeats the call after some timeout
     */
    callWithRetry(method, params) {
        return __awaiter(this, void 0, void 0, function* () {
            return new Promise((resolve, reject) => {
                let makeCall = (resolve, reject, method, params, useQueue = true) => {
                    this.call(method, params)
                        .then(r => {
                        resolve(r);
                    })
                        .catch(e => {
                        if (e instanceof VKApiError_1.default) {
                            /**
                             * 6 - too many requests per second
                             * 10 - internal server error
                             */
                            if (e.errorCode == 6 || e.errorCode == 10) {
                                setTimeout(() => {
                                    makeCall(resolve, reject, method, params, useQueue);
                                }, 300);
                            }
                            reject(e);
                        }
                        else {
                            /**
                             * Networking error
                             */
                            setTimeout(() => {
                                makeCall(resolve, reject, method, params, useQueue);
                            }, 300);
                        }
                    });
                };
                makeCall(resolve, reject, method, params);
            });
        });
    }
    handleResponse(method, params, body, response, err, resolve, reject) {
        if (!err && response.statusCode == 200 && !body.error) {
            resolve(body.response);
            return;
        }
        if (body && body.error) {
            reject(VKApiError_1.default.deserialize(body.error));
            if (this._logger)
                this._logger.warn('VK Api error\n', {
                    response: JSON.stringify(body),
                    error: VKApiError_1.default.deserialize(body.error),
                    method,
                    params
                });
            return;
        }
        if (err) {
            if (this._logger)
                this._logger.error('VK Api:\n', {
                    'Networking error:': err,
                    method,
                    params
                });
            reject(err);
            return;
        }
        if (this._logger)
            this._logger.error('VK Api:\n', {
                'api request error: Body:': body,
                'Error:': err
            });
        reject(err);
    }
    filterParams(params) {
        for (let paramName in params) {
            if (params[paramName] == undefined) {
                delete params[paramName];
            }
        }
        return params;
    }
    /**
     * Returns detailed information on users.
     *
     * @param {{
     *   user_ids: (string[]|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UsersGetResponse>}
     */
    usersGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("users.get", params);
        });
    }
    /**
     * Returns a list of users matching the search criteria.
     *
     * @param {{
     *   q: (string|undefined),
     *   sort: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   city: (number|undefined),
     *   country: (number|undefined),
     *   hometown: (string|undefined),
     *   university_country: (number|undefined),
     *   university: (number|undefined),
     *   university_year: (number|undefined),
     *   university_faculty: (number|undefined),
     *   university_chair: (number|undefined),
     *   sex: (number|undefined),
     *   status: (number|undefined),
     *   age_from: (number|undefined),
     *   age_to: (number|undefined),
     *   birth_day: (number|undefined),
     *   birth_month: (number|undefined),
     *   birth_year: (number|undefined),
     *   online: (boolean|undefined),
     *   has_photo: (boolean|undefined),
     *   school_country: (number|undefined),
     *   school_city: (number|undefined),
     *   school_class: (number|undefined),
     *   school: (number|undefined),
     *   school_year: (number|undefined),
     *   religion: (string|undefined),
     *   interests: (string|undefined),
     *   company: (string|undefined),
     *   position: (string|undefined),
     *   group_id: (number|undefined),
     *   from_list: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UsersSearchResponse>}
     */
    usersSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("users.search", params);
        });
    }
    /**
     * Returns information whether a user installed the application.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UsersIsAppUserResponse>}
     */
    usersIsAppUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("users.isAppUser", params);
        });
    }
    /**
     * Returns a list of IDs of users and communities followed by the user.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   extended: (boolean|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UsersGetSubscriptionsResponse>}
     */
    usersGetSubscriptions(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("users.getSubscriptions", params);
        });
    }
    /**
     * Returns a list of IDs of followers of the user in question, sorted by date added, most recent first.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UsersGetFollowersResponse>}
     */
    usersGetFollowers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("users.getFollowers", params);
        });
    }
    /**
     * Reports (submits a complain about) a user.
     *
     * @param {{
     *   user_id: (number),
     *   type: (string),
     *   comment: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    usersReport(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("users.report", params);
        });
    }
    /**
     * Indexes current user location and returns nearby users.
     *
     * @param {{
     *   latitude: (number),
     *   longitude: (number),
     *   accuracy: (number|undefined),
     *   timeout: (number|undefined),
     *   radius: (number|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UsersGetNearbyResponse>}
     */
    usersGetNearby(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("users.getNearby", params);
        });
    }
    /**
     * Checks a user's phone number for correctness.
     *
     * @param {{
     *   phone: (string),
     *   client_id: (number|undefined),
     *   client_secret: (string),
     *   auth_by_phone: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    authCheckPhone(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("auth.checkPhone", params);
        });
    }
    /**
     * Registers a new user by phone number.
     *
     * @param {{
     *   first_name: (string),
     *   last_name: (string),
     *   birthday: (string),
     *   client_id: (number),
     *   client_secret: (string),
     *   phone: (string),
     *   password: (string|undefined),
     *   test_mode: (boolean|undefined),
     *   voice: (boolean|undefined),
     *   sex: (number|undefined),
     *   sid: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AuthSignupResponse>}
     */
    authSignup(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("auth.signup", params);
        });
    }
    /**
     * Completes a user's registration (begun with the [vk.com/dev/auth.signup|auth.signup] method) using an authorization code.
     *
     * @param {{
     *   client_id: (number),
     *   client_secret: (string),
     *   phone: (string),
     *   code: (string),
     *   password: (string|undefined),
     *   test_mode: (boolean|undefined),
     *   intro: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AuthConfirmResponse>}
     */
    authConfirm(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("auth.confirm", params);
        });
    }
    /**
     * Allows to restore account access using a code received via SMS. " This method is only available for apps with [vk.com/dev/auth_direct|Direct authorization] access. "
     *
     * @param {{
     *   phone: (string),
     *   last_name: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AuthRestoreResponse>}
     */
    authRestore(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("auth.restore", params);
        });
    }
    /**
     * Returns a list of posts on a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   domain: (string|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   filter: (string|undefined),
     *   extended: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WallGetResponse>}
     */
    wallGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.get", params);
        });
    }
    /**
     * Allows to search posts on user or community walls.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   domain: (string|undefined),
     *   query: (string|undefined),
     *   owners_only: (boolean|undefined),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   extended: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WallSearchResponse>}
     */
    wallSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.search", params);
        });
    }
    /**
     * Creates an empty photo album.
     *
     * @param {{
     *   title: (string),
     *   group_id: (number|undefined),
     *   description: (string|undefined),
     *   privacy_view: (string[]|undefined),
     *   privacy_comment: (string[]|undefined),
     *   upload_by_admins_only: (boolean|undefined),
     *   comments_disabled: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosCreateAlbumResponse>}
     */
    photosCreateAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.createAlbum", params);
        });
    }
    /**
     * Edits information about a photo album.
     *
     * @param {{
     *   album_id: (number),
     *   title: (string|undefined),
     *   description: (string|undefined),
     *   owner_id: (number|undefined),
     *   privacy_view: (string[]|undefined),
     *   privacy_comment: (string[]|undefined),
     *   upload_by_admins_only: (boolean|undefined),
     *   comments_disabled: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosEditAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.editAlbum", params);
        });
    }
    /**
     * Returns a list of a user's or community's photo albums.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   album_ids: (number[]|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   need_system: (boolean|undefined),
     *   need_covers: (boolean|undefined),
     *   photo_sizes: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetAlbumsResponse>}
     */
    photosGetAlbums(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getAlbums", params);
        });
    }
    /**
     * Returns a list of a user's or community's photos.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   album_id: (string|undefined),
     *   photo_ids: (string[]|undefined),
     *   rev: (boolean|undefined),
     *   extended: (boolean|undefined),
     *   feed_type: (string|undefined),
     *   feed: (number|undefined),
     *   photo_sizes: (boolean|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetResponse>}
     */
    photosGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.get", params);
        });
    }
    /**
     * Returns the number of photo albums belonging to a user or community.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetAlbumsCountResponse>}
     */
    photosGetAlbumsCount(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getAlbumsCount", params);
        });
    }
    /**
     * Returns information about photos by their IDs.
     *
     * @param {{
     *   photos: (string[]),
     *   extended: (boolean|undefined),
     *   photo_sizes: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetByIdResponse>}
     */
    photosGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getById", params);
        });
    }
    /**
     * Returns the server address for photo upload.
     *
     * @param {{
     *   album_id: (number|undefined),
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetUploadServerResponse>}
     */
    photosGetUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getUploadServer", params);
        });
    }
    /**
     * Returns the server address for owner cover upload.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   crop_x: (number|undefined),
     *   crop_y: (number|undefined),
     *   crop_x2: (number|undefined),
     *   crop_y2: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetOwnerCoverPhotoUploadServerResponse>}
     */
    photosGetOwnerCoverPhotoUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getOwnerCoverPhotoUploadServer", params);
        });
    }
    /**
     * Returns an upload server address for a profile or community photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetOwnerPhotoUploadServerResponse>}
     */
    photosGetOwnerPhotoUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getOwnerPhotoUploadServer", params);
        });
    }
    /**
     * Returns an upload link for chat cover pictures.
     *
     * @param {{
     *   chat_id: (number),
     *   crop_x: (number|undefined),
     *   crop_y: (number|undefined),
     *   crop_width: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetChatUploadServerResponse>}
     */
    photosGetChatUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getChatUploadServer", params);
        });
    }
    /**
     * Returns the server address for market photo upload.
     *
     * @param {{
     *   group_id: (number),
     *   main_photo: (boolean|undefined),
     *   crop_x: (number|undefined),
     *   crop_y: (number|undefined),
     *   crop_width: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetMarketUploadServerResponse>}
     */
    photosGetMarketUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getMarketUploadServer", params);
        });
    }
    /**
     * Returns the server address for market album photo upload.
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetMarketAlbumUploadServerResponse>}
     */
    photosGetMarketAlbumUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getMarketAlbumUploadServer", params);
        });
    }
    /**
     * Saves market photos after successful uploading.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   photo: (string),
     *   server: (number),
     *   hash: (string),
     *   crop_data: (string|undefined),
     *   crop_hash: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosSaveMarketPhotoResponse>}
     */
    photosSaveMarketPhoto(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.saveMarketPhoto", params);
        });
    }
    /**
     * Saves cover photo after successful uploading.
     *
     * @param {{
     *   photo: (string),
     *   hash: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosSaveOwnerCoverPhotoResponse>}
     */
    photosSaveOwnerCoverPhoto(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.saveOwnerCoverPhoto", params);
        });
    }
    /**
     * Saves market album photos after successful uploading.
     *
     * @param {{
     *   group_id: (number),
     *   photo: (string),
     *   server: (number),
     *   hash: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosSaveMarketAlbumPhotoResponse>}
     */
    photosSaveMarketAlbumPhoto(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.saveMarketAlbumPhoto", params);
        });
    }
    /**
     * Saves a profile or community photo. Upload URL can be got with the [vk.com/dev/photos.getOwnerPhotoUploadServer|photos.getOwnerPhotoUploadServer] method.
     *
     * @param {{
     *   server: (string|undefined),
     *   hash: (string|undefined),
     *   photo: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosSaveOwnerPhotoResponse>}
     */
    photosSaveOwnerPhoto(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.saveOwnerPhoto", params);
        });
    }
    /**
     * Saves a photo to a user's or community's wall after being uploaded.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   group_id: (number|undefined),
     *   photo: (string),
     *   server: (number|undefined),
     *   hash: (string|undefined),
     *   latitude: (number|undefined),
     *   longitude: (number|undefined),
     *   caption: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosSaveWallPhotoResponse>}
     */
    photosSaveWallPhoto(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.saveWallPhoto", params);
        });
    }
    /**
     * Returns the server address for photo upload onto a user's wall.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetWallUploadServerResponse>}
     */
    photosGetWallUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getWallUploadServer", params);
        });
    }
    /**
     * Returns the server address for photo upload in a private message for a user.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetMessagesUploadServerResponse>}
     */
    photosGetMessagesUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getMessagesUploadServer", params);
        });
    }
    /**
     * Saves a photo after being successfully uploaded. URL obtained with [vk.com/dev/photos.getMessagesUploadServer|photos.getMessagesUploadServer] method.
     *
     * @param {{
     *   photo: (string),
     *   server: (number|undefined),
     *   hash: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosSaveMessagesPhotoResponse>}
     */
    photosSaveMessagesPhoto(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.saveMessagesPhoto", params);
        });
    }
    /**
     * Reports (submits a complaint about) a photo.
     *
     * @param {{
     *   owner_id: (number),
     *   photo_id: (number),
     *   reason: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosReport(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.report", params);
        });
    }
    /**
     * Reports (submits a complaint about) a comment on a photo.
     *
     * @param {{
     *   owner_id: (number),
     *   comment_id: (number),
     *   reason: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosReportComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.reportComment", params);
        });
    }
    /**
     * Returns a list of photos.
     *
     * @param {{
     *   q: (string|undefined),
     *   lat: (number|undefined),
     *   long: (number|undefined),
     *   start_time: (number|undefined),
     *   end_time: (number|undefined),
     *   sort: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   radius: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosSearchResponse>}
     */
    photosSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.search", params);
        });
    }
    /**
     * Returns a list of user IDs or detailed information about a user's friends.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   order: (string|undefined),
     *   list_id: (number|undefined),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetResponse>}
     */
    friendsGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.get", params);
        });
    }
    /**
     * Returns a list of user IDs of a user's friends who are online.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   list_id: (number|undefined),
     *   online_mobile: (boolean|undefined),
     *   order: (string|undefined),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetOnlineResponse>}
     */
    friendsGetOnline(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getOnline", params);
        });
    }
    /**
     * Returns a list of user IDs of the mutual friends of two users.
     *
     * @param {{
     *   source_uid: (number|undefined),
     *   target_uid: (number|undefined),
     *   target_uids: (number[]|undefined),
     *   order: (string|undefined),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetMutualResponse>}
     */
    friendsGetMutual(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getMutual", params);
        });
    }
    /**
     * Returns a list of user IDs of the current user's recently added friends.
     *
     * @param {{
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetRecentResponse>}
     */
    friendsGetRecent(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getRecent", params);
        });
    }
    /**
     * Returns information about the current user's incoming and outgoing friend requests.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   need_mutual: (boolean|undefined),
     *   out: (boolean|undefined),
     *   sort: (number|undefined),
     *   suggested: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetRequestsResponse>}
     */
    friendsGetRequests(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getRequests", params);
        });
    }
    /**
     * Approves or creates a friend request.
     *
     * @param {{
     *   user_id: (number),
     *   text: (string|undefined),
     *   follow: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsAddResponse>}
     */
    friendsAdd(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.add", params);
        });
    }
    /**
     * Edits the friend lists of the selected user.
     *
     * @param {{
     *   user_id: (number),
     *   list_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    friendsEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.edit", params);
        });
    }
    /**
     * Declines a friend request or deletes a user from the current user's friend list.
     *
     * @param {{
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsDeleteResponse>}
     */
    friendsDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.delete", params);
        });
    }
    /**
     * Returns a list of the user's friend lists.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   return_system: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetListsResponse>}
     */
    friendsGetLists(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getLists", params);
        });
    }
    /**
     * Creates a new friend list for the current user.
     *
     * @param {{
     *   name: (string),
     *   user_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsAddListResponse>}
     */
    friendsAddList(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.addList", params);
        });
    }
    /**
     * Edits a friend list of the current user.
     *
     * @param {{
     *   name: (string|undefined),
     *   list_id: (number),
     *   user_ids: (number[]|undefined),
     *   add_user_ids: (number[]|undefined),
     *   delete_user_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    friendsEditList(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.editList", params);
        });
    }
    /**
     * Deletes a friend list of the current user.
     *
     * @param {{
     *   list_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    friendsDeleteList(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.deleteList", params);
        });
    }
    /**
     * Returns a list of IDs of the current user's friends who installed the application.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetAppUsersResponse>}
     */
    friendsGetAppUsers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getAppUsers", params);
        });
    }
    /**
     * Returns a list of the current user's friends whose phone numbers, validated or specified in a profile, are in a given list.
     *
     * @param {{
     *   phones: (string[]|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetByPhonesResponse>}
     */
    friendsGetByPhones(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getByPhones", params);
        });
    }
    /**
     * Marks all incoming friend requests as viewed.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    friendsDeleteAllRequests(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.deleteAllRequests", params);
        });
    }
    /**
     * Returns a list of profiles of users whom the current user may know.
     *
     * @param {{
     *   filter: (string[]|undefined),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetSuggestionsResponse>}
     */
    friendsGetSuggestions(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getSuggestions", params);
        });
    }
    /**
     * Checks the current user's friendship status with other specified users.
     *
     * @param {{
     *   user_ids: (number[]),
     *   need_sign: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsAreFriendsResponse>}
     */
    friendsAreFriends(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.areFriends", params);
        });
    }
    /**
     * Returns a list of friends who can be called by the current user.
     *
     * @param {{
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsGetAvailableForCallResponse>}
     */
    friendsGetAvailableForCall(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.getAvailableForCall", params);
        });
    }
    /**
     * Returns a list of friends matching the search criteria.
     *
     * @param {{
     *   user_id: (number),
     *   q: (string|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FriendsSearchResponse>}
     */
    friendsSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("friends.search", params);
        });
    }
    /**
     * Gets a list of comments for the page added through the [vk.com/dev/Comments|Comments widget].
     *
     * @param {{
     *   widget_api_id: (number|undefined),
     *   url: (string|undefined),
     *   page_id: (string|undefined),
     *   order: (string|undefined),
     *   fields: (string[]|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WidgetsGetCommentsResponse>}
     */
    widgetsGetComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("widgets.getComments", params);
        });
    }
    /**
     * Gets a list of application/site pages where the [vk.com/dev/Comments|Comments widget] or [vk.com/dev/Like|Like widget] is installed.
     *
     * @param {{
     *   widget_api_id: (number|undefined),
     *   order: (string|undefined),
     *   period: (string|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WidgetsGetPagesResponse>}
     */
    widgetsGetPages(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("widgets.getPages", params);
        });
    }
    /**
     * Allows to hide stories from chosen sources from current user's feed.
     *
     * @param {{
     *   owners_ids: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    storiesBanOwner(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.banOwner", params);
        });
    }
    /**
     * Allows to delete story.
     *
     * @param {{
     *   owner_id: (number),
     *   story_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    storiesDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.delete", params);
        });
    }
    /**
     * Returns stories available for current user.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StoriesGetResponse>}
     */
    storiesGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.get", params);
        });
    }
    /**
     * Returns list of sources hidden from current user's feed.
     *
     * @param {{
     *   fields: (string[]|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StoriesGetBannedResponse>}
     */
    storiesGetBanned(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.getBanned", params);
        });
    }
    /**
     * Returns story by its ID.
     *
     * @param {{
     *   stories: (string[]|undefined),
     *   extended: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StoriesGetByIdResponse>}
     */
    storiesGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.getById", params);
        });
    }
    /**
     * Returns URL for uploading a story with photo.
     *
     * @param {{
     *   add_to_news: (boolean|undefined),
     *   user_ids: (number[]|undefined),
     *   reply_to_story: (string|undefined),
     *   link_text: (string|undefined),
     *   link_url: (string|undefined),
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StoriesGetPhotoUploadServerResponse>}
     */
    storiesGetPhotoUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.getPhotoUploadServer", params);
        });
    }
    /**
     * Returns replies to the story.
     *
     * @param {{
     *   owner_id: (number),
     *   story_id: (number),
     *   access_key: (string|undefined),
     *   extended: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StoriesGetRepliesResponse>}
     */
    storiesGetReplies(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.getReplies", params);
        });
    }
    /**
     * Returns stories available for current user.
     *
     * @param {{
     *   owner_id: (number),
     *   story_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StoriesGetStatsResponse>}
     */
    storiesGetStats(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.getStats", params);
        });
    }
    /**
     * Allows to receive URL for uploading story with video.
     *
     * @param {{
     *   add_to_news: (boolean|undefined),
     *   user_ids: (number[]|undefined),
     *   reply_to_story: (string|undefined),
     *   link_text: (string|undefined),
     *   link_url: (string|undefined),
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StoriesGetVideoUploadServerResponse>}
     */
    storiesGetVideoUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.getVideoUploadServer", params);
        });
    }
    /**
     * Returns a list of story viewers.
     *
     * @param {{
     *   owner_id: (number),
     *   story_id: (number),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StoriesGetViewersResponse>}
     */
    storiesGetViewers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.getViewers", params);
        });
    }
    /**
     * Hides all replies in the last 24 hours from the user to current user's stories.
     *
     * @param {{
     *   owner_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    storiesHideAllReplies(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.hideAllReplies", params);
        });
    }
    /**
     * Hides the reply to the current user's story.
     *
     * @param {{
     *   owner_id: (number),
     *   story_id: (number),
     *   access_key: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    storiesHideReply(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.hideReply", params);
        });
    }
    /**
     * Allows to show stories from hidden sources in current user's feed.
     *
     * @param {{
     *   owners_ids: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    storiesUnbanOwner(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stories.unbanOwner", params);
        });
    }
    /**
     * Returns payment balance of the application in hundredth of a vote.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.SecureGetAppBalanceResponse>}
     */
    secureGetAppBalance(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.getAppBalance", params);
        });
    }
    /**
     * Shows history of votes transaction between users and the application.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.SecureGetTransactionsHistoryResponse>}
     */
    secureGetTransactionsHistory(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.getTransactionsHistory", params);
        });
    }
    /**
     * Shows a list of SMS notifications sent by the application using [vk.com/dev/secure.sendSMSNotification|secure.sendSMSNotification] method.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   date_from: (number|undefined),
     *   date_to: (number|undefined),
     *   limit: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.SecureGetSMSHistoryResponse>}
     */
    secureGetSMSHistory(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.getSMSHistory", params);
        });
    }
    /**
     * Sends 'SMS' notification to a user's mobile device.
     *
     * @param {{
     *   user_id: (number),
     *   message: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    secureSendSMSNotification(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.sendSMSNotification", params);
        });
    }
    /**
     * Sends notification to the user.
     *
     * @param {{
     *   user_ids: (number[]|undefined),
     *   user_id: (number|undefined),
     *   message: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.SecureSendNotificationResponse>}
     */
    secureSendNotification(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.sendNotification", params);
        });
    }
    /**
     * Sets a counter which is shown to the user in bold in the left menu.
     *
     * @param {{
     *   counters: (string[]|undefined),
     *   user_id: (number|undefined),
     *   counter: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    secureSetCounter(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.setCounter", params);
        });
    }
    /**
     * Sets user game level in the application which can be seen by his/her friends.
     *
     * @param {{
     *   levels: (string[]|undefined),
     *   user_id: (number|undefined),
     *   level: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    secureSetUserLevel(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.setUserLevel", params);
        });
    }
    /**
     * Returns one of the previously set game levels of one or more users in the application.
     *
     * @param {{
     *   user_ids: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.SecureGetUserLevelResponse>}
     */
    secureGetUserLevel(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.getUserLevel", params);
        });
    }
    /**
     * Allows to receive data for the connection to Streaming API.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StreamingGetServerUrlResponse>}
     */
    streamingGetServerUrl(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("streaming.getServerUrl", params);
        });
    }
    /**
     * Returns a value of variable with the name set by key parameter.
     *
     * @param {{
     *   key: (string|undefined),
     *   keys: (string[]|undefined),
     *   user_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StorageGetResponse>}
     */
    storageGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("storage.get", params);
        });
    }
    /**
     * Saves a value of variable with the name set by 'key' parameter.
     *
     * @param {{
     *   key: (string),
     *   value: (string|undefined),
     *   user_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    storageSet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("storage.set", params);
        });
    }
    /**
     * Returns the names of all variables.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StorageGetKeysResponse>}
     */
    storageGetKeys(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("storage.getKeys", params);
        });
    }
    /**
     * Returns a list of orders.
     *
     * @param {{
     *   count: (number|undefined),
     *   test_mode: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OrdersGetResponse>}
     */
    ordersGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("orders.get", params);
        });
    }
    /**
     * Returns information about orders by their IDs.
     *
     * @param {{
     *   order_id: (number|undefined),
     *   order_ids: (number[]|undefined),
     *   test_mode: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OrdersGetByIdResponse>}
     */
    ordersGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("orders.getById", params);
        });
    }
    /**
     * Changes order status.
     *
     * @param {{
     *   order_id: (number),
     *   action: (string),
     *   app_order_id: (number|undefined),
     *   test_mode: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OrdersChangeStateResponse>}
     */
    ordersChangeState(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("orders.changeState", params);
        });
    }
    /**
     * undefined
     *
     * @param {{
     *   user_id: (number),
     *   votes: (string[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OrdersGetAmountResponse>}
     */
    ordersGetAmount(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("orders.getAmount", params);
        });
    }
    /**
     * Saves photos after successful uploading.
     *
     * @param {{
     *   album_id: (number|undefined),
     *   group_id: (number|undefined),
     *   server: (number|undefined),
     *   photos_list: (string|undefined),
     *   hash: (string|undefined),
     *   latitude: (number|undefined),
     *   longitude: (number|undefined),
     *   caption: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosSaveResponse>}
     */
    photosSave(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.save", params);
        });
    }
    /**
     * Allows to copy a photo to the "Saved photos" album
     *
     * @param {{
     *   owner_id: (number),
     *   photo_id: (number),
     *   access_key: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosCopyResponse>}
     */
    photosCopy(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.copy", params);
        });
    }
    /**
     * Edits the caption of a photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   caption: (string|undefined),
     *   latitude: (number|undefined),
     *   longitude: (number|undefined),
     *   place_str: (string|undefined),
     *   foursquare_id: (string|undefined),
     *   delete_place: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.edit", params);
        });
    }
    /**
     * Moves a photo from one album to another.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   target_album_id: (number),
     *   photo_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosMove(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.move", params);
        });
    }
    /**
     * Makes a photo into an album cover.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   album_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosMakeCover(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.makeCover", params);
        });
    }
    /**
     * Reorders the album in the list of user albums.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   album_id: (number),
     *   before: (number|undefined),
     *   after: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosReorderAlbums(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.reorderAlbums", params);
        });
    }
    /**
     * Reorders the photo in the list of photos of the user album.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   before: (number|undefined),
     *   after: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosReorderPhotos(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.reorderPhotos", params);
        });
    }
    /**
     * Returns a list of photos belonging to a user or community, in reverse chronological order.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   extended: (boolean|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   photo_sizes: (boolean|undefined),
     *   no_service_albums: (boolean|undefined),
     *   need_hidden: (boolean|undefined),
     *   skip_hidden: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetAllResponse>}
     */
    photosGetAll(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getAll", params);
        });
    }
    /**
     * Returns a list of photos in which a user is tagged.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   sort: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetUserPhotosResponse>}
     */
    photosGetUserPhotos(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getUserPhotos", params);
        });
    }
    /**
     * Deletes a photo album belonging to the current user.
     *
     * @param {{
     *   album_id: (number),
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosDeleteAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.deleteAlbum", params);
        });
    }
    /**
     * Deletes a photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.delete", params);
        });
    }
    /**
     * Restores a deleted photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosRestore(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.restore", params);
        });
    }
    /**
     * Confirms a tag on a photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (string),
     *   tag_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosConfirmTag(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.confirmTag", params);
        });
    }
    /**
     * Returns a list of comments on a photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   need_likes: (boolean|undefined),
     *   start_comment_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   sort: (string|undefined),
     *   access_key: (string|undefined),
     *   extended: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetCommentsResponse>}
     */
    photosGetComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getComments", params);
        });
    }
    /**
     * Returns a list of comments on a specific photo album or all albums of the user sorted in reverse chronological order.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   album_id: (number|undefined),
     *   need_likes: (boolean|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetAllCommentsResponse>}
     */
    photosGetAllComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getAllComments", params);
        });
    }
    /**
     * Adds a new comment on the photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   from_group: (boolean|undefined),
     *   reply_to_comment: (number|undefined),
     *   sticker_id: (number|undefined),
     *   access_key: (string|undefined),
     *   guid: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosCreateCommentResponse>}
     */
    photosCreateComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.createComment", params);
        });
    }
    /**
     * Deletes a comment on the photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosDeleteCommentResponse>}
     */
    photosDeleteComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.deleteComment", params);
        });
    }
    /**
     * Restores a deleted comment on a photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosRestoreCommentResponse>}
     */
    photosRestoreComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.restoreComment", params);
        });
    }
    /**
     * Edits a comment on a photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosEditComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.editComment", params);
        });
    }
    /**
     * Returns a list of tags on a photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   access_key: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetTagsResponse>}
     */
    photosGetTags(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getTags", params);
        });
    }
    /**
     * Adds a tag on the photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   user_id: (number),
     *   x: (number|undefined),
     *   y: (number|undefined),
     *   x2: (number|undefined),
     *   y2: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosPutTagResponse>}
     */
    photosPutTag(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.putTag", params);
        });
    }
    /**
     * Removes a tag from a photo.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   photo_id: (number),
     *   tag_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    photosRemoveTag(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.removeTag", params);
        });
    }
    /**
     * Returns a list of photos with tags that have not been viewed.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PhotosGetNewTagsResponse>}
     */
    photosGetNewTags(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("photos.getNewTags", params);
        });
    }
    /**
     * Returns a list of posts from user or community walls by their IDs.
     *
     * @param {{
     *   posts: (string[]),
     *   extended: (boolean|undefined),
     *   copy_history_depth: (number|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WallGetByIdResponse>}
     */
    wallGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.getById", params);
        });
    }
    /**
     * Adds a new post on a user wall or community wall. Can also be used to publish suggested or scheduled posts.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   friends_only: (boolean|undefined),
     *   from_group: (boolean|undefined),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   services: (string|undefined),
     *   signed: (boolean|undefined),
     *   publish_date: (number|undefined),
     *   lat: (number|undefined),
     *   long: (number|undefined),
     *   place_id: (number|undefined),
     *   post_id: (number|undefined),
     *   guid: (string|undefined),
     *   mark_as_ads: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WallPostResponse>}
     */
    wallPost(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.post", params);
        });
    }
    /**
     * Reposts (copies) an object to a user wall or community wall.
     *
     * @param {{
     *   object: (string),
     *   message: (string|undefined),
     *   group_id: (number|undefined),
     *   mark_as_ads: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WallRepostResponse>}
     */
    wallRepost(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.repost", params);
        });
    }
    /**
     * Returns information about reposts of a post on user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   post_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WallGetRepostsResponse>}
     */
    wallGetReposts(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.getReposts", params);
        });
    }
    /**
     * Edits a post on a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   post_id: (number),
     *   friends_only: (boolean|undefined),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   services: (string|undefined),
     *   signed: (boolean|undefined),
     *   publish_date: (number|undefined),
     *   lat: (number|undefined),
     *   long: (number|undefined),
     *   place_id: (number|undefined),
     *   mark_as_ads: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.edit", params);
        });
    }
    /**
     * Deletes a post from a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   post_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.delete", params);
        });
    }
    /**
     * Restores a post deleted from a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   post_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallRestore(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.restore", params);
        });
    }
    /**
     * Pins the post on wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   post_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallPin(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.pin", params);
        });
    }
    /**
     * Unpins the post on wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   post_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallUnpin(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.unpin", params);
        });
    }
    /**
     * Returns a list of comments on a post on a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   post_id: (number),
     *   need_likes: (boolean|undefined),
     *   start_comment_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   sort: (string|undefined),
     *   preview_length: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WallGetCommentsResponse>}
     */
    wallGetComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.getComments", params);
        });
    }
    /**
     * Adds a comment to a post on a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   post_id: (number),
     *   from_group: (number|undefined),
     *   message: (string|undefined),
     *   reply_to_comment: (number|undefined),
     *   attachments: (string[]|undefined),
     *   sticker_id: (number|undefined),
     *   guid: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.WallCreateCommentResponse>}
     */
    wallCreateComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.createComment", params);
        });
    }
    /**
     * Edits a comment on a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallEditComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.editComment", params);
        });
    }
    /**
     * Deletes a comment on a post on a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallDeleteComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.deleteComment", params);
        });
    }
    /**
     * Restores a comment deleted from a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallRestoreComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.restoreComment", params);
        });
    }
    /**
     * Reports (submits a complaint about) a post on a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number),
     *   post_id: (number),
     *   reason: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallReportPost(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.reportPost", params);
        });
    }
    /**
     * Reports (submits a complaint about) a comment on a post on a user wall or community wall.
     *
     * @param {{
     *   owner_id: (number),
     *   comment_id: (number),
     *   reason: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    wallReportComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("wall.reportComment", params);
        });
    }
    /**
     * Returns data required to show the status of a user or community.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StatusGetResponse>}
     */
    statusGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("status.get", params);
        });
    }
    /**
     * Sets a new status for the current user.
     *
     * @param {{
     *   text: (string|undefined),
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    statusSet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("status.set", params);
        });
    }
    /**
     * Completes the lead started by user.
     *
     * @param {{
     *   vk_sid: (string),
     *   secret: (string),
     *   comment: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LeadsCompleteResponse>}
     */
    leadsComplete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("leads.complete", params);
        });
    }
    /**
     * Creates new session for the user passing the offer.
     *
     * @param {{
     *   lead_id: (number),
     *   secret: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LeadsStartResponse>}
     */
    leadsStart(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("leads.start", params);
        });
    }
    /**
     * Returns lead stats data.
     *
     * @param {{
     *   lead_id: (number),
     *   secret: (string|undefined),
     *   date_start: (string|undefined),
     *   date_end: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LeadsGetStatsResponse>}
     */
    leadsGetStats(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("leads.getStats", params);
        });
    }
    /**
     * Returns a list of last user actions for the offer.
     *
     * @param {{
     *   offer_id: (number),
     *   secret: (string),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   status: (number|undefined),
     *   reverse: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LeadsGetUsersResponse>}
     */
    leadsGetUsers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("leads.getUsers", params);
        });
    }
    /**
     * Checks if the user can start the lead.
     *
     * @param {{
     *   lead_id: (number),
     *   test_result: (number|undefined),
     *   age: (number|undefined),
     *   country: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LeadsCheckUserResponse>}
     */
    leadsCheckUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("leads.checkUser", params);
        });
    }
    /**
     * Counts the metric event.
     *
     * @param {{
     *   data: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LeadsMetricHitResponse>}
     */
    leadsMetricHit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("leads.metricHit", params);
        });
    }
    /**
     * Returns information about a wiki page.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   page_id: (number|undefined),
     *   global: (boolean|undefined),
     *   site_preview: (boolean|undefined),
     *   title: (string|undefined),
     *   need_source: (boolean|undefined),
     *   need_html: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PagesGetResponse>}
     */
    pagesGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("pages.get", params);
        });
    }
    /**
     * Saves the text of a wiki page.
     *
     * @param {{
     *   text: (string|undefined),
     *   page_id: (number|undefined),
     *   group_id: (number|undefined),
     *   user_id: (number|undefined),
     *   title: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PagesSaveResponse>}
     */
    pagesSave(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("pages.save", params);
        });
    }
    /**
     * Saves modified read and edit access settings for a wiki page.
     *
     * @param {{
     *   page_id: (number),
     *   group_id: (number|undefined),
     *   user_id: (number|undefined),
     *   view: (number|undefined),
     *   edit: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PagesSaveAccessResponse>}
     */
    pagesSaveAccess(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("pages.saveAccess", params);
        });
    }
    /**
     * Returns a list of all previous versions of a wiki page.
     *
     * @param {{
     *   page_id: (number),
     *   group_id: (number|undefined),
     *   user_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PagesGetHistoryResponse>}
     */
    pagesGetHistory(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("pages.getHistory", params);
        });
    }
    /**
     * Returns a list of wiki pages in a group.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PagesGetTitlesResponse>}
     */
    pagesGetTitles(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("pages.getTitles", params);
        });
    }
    /**
     * Returns the text of one of the previous versions of a wiki page.
     *
     * @param {{
     *   version_id: (number),
     *   group_id: (number|undefined),
     *   user_id: (number|undefined),
     *   need_html: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PagesGetVersionResponse>}
     */
    pagesGetVersion(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("pages.getVersion", params);
        });
    }
    /**
     * Returns HTML representation of the wiki markup.
     *
     * @param {{
     *   text: (string),
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PagesParseWikiResponse>}
     */
    pagesParseWiki(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("pages.parseWiki", params);
        });
    }
    /**
     * Allows to clear the cache of particular 'external' pages which may be attached to VK posts.
     *
     * @param {{
     *   url: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    pagesClearCache(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("pages.clearCache", params);
        });
    }
    /**
     * Returns information specifying whether a user is a member of a community.
     *
     * @param {{
     *   group_id: (string),
     *   user_id: (number|undefined),
     *   user_ids: (number[]|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsIsMemberResponse>}
     */
    groupsIsMember(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.isMember", params);
        });
    }
    /**
     * Returns information about communities by their IDs.
     *
     * @param {{
     *   group_ids: (string[]|undefined),
     *   group_id: (string|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetByIdResponse>}
     */
    groupsGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getById", params);
        });
    }
    /**
     * Returns a list of the communities to which a user belongs.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   extended: (boolean|undefined),
     *   filter: (string[]|undefined),
     *   fields: (string[]|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetResponse>}
     */
    groupsGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.get", params);
        });
    }
    /**
     * Returns a list of community members.
     *
     * @param {{
     *   group_id: (string|undefined),
     *   sort: (string|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   filter: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetMembersResponse>}
     */
    groupsGetMembers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getMembers", params);
        });
    }
    /**
     * With this method you can join the group or public page, and also confirm your participation in an event.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   not_sure: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsJoin(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.join", params);
        });
    }
    /**
     * With this method you can leave a group, public page, or event.
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsLeave(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.leave", params);
        });
    }
    /**
     * Returns a list of communities matching the search criteria.
     *
     * @param {{
     *   q: (string),
     *   type: (string|undefined),
     *   country_id: (number|undefined),
     *   city_id: (number|undefined),
     *   future: (boolean|undefined),
     *   market: (boolean|undefined),
     *   sort: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsSearchResponse>}
     */
    groupsSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.search", params);
        });
    }
    /**
     * Returns communities list for a catalog category.
     *
     * @param {{
     *   category_id: (number|undefined),
     *   subcategory_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetCatalogResponse>}
     */
    groupsGetCatalog(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getCatalog", params);
        });
    }
    /**
     * Returns categories list for communities catalog
     *
     * @param {{
     *   extended: (boolean|undefined),
     *   subcategories: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetCatalogInfoResponse>}
     */
    groupsGetCatalogInfo(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getCatalogInfo", params);
        });
    }
    /**
     * Returns a list of invitations to join communities and events.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetInvitesResponse>}
     */
    groupsGetInvites(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getInvites", params);
        });
    }
    /**
     * Returns invited users list of a community
     *
     * @param {{
     *   group_id: (number),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetInvitedUsersResponse>}
     */
    groupsGetInvitedUsers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getInvitedUsers", params);
        });
    }
    /**
     * Adds a user to a community blacklist.
     *
     * @param {{
     *   group_id: (number),
     *   user_id: (number),
     *   end_date: (number|undefined),
     *   reason: (number|undefined),
     *   comment: (string|undefined),
     *   comment_visible: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsBanUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.banUser", params);
        });
    }
    /**
     * Removes a user from a community blacklist.
     *
     * @param {{
     *   group_id: (number),
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsUnbanUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.unbanUser", params);
        });
    }
    /**
     * Returns a list of users on a community blacklist.
     *
     * @param {{
     *   group_id: (number),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   user_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetBannedResponse>}
     */
    groupsGetBanned(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getBanned", params);
        });
    }
    /**
     * Creates a new community.
     *
     * @param {{
     *   title: (string),
     *   description: (string|undefined),
     *   type: (string|undefined),
     *   public_category: (number|undefined),
     *   subtype: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsCreateResponse>}
     */
    groupsCreate(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.create", params);
        });
    }
    /**
     * Edits a community.
     *
     * @param {{
     *   group_id: (number),
     *   title: (string|undefined),
     *   description: (string|undefined),
     *   screen_name: (string|undefined),
     *   access: (number|undefined),
     *   website: (string|undefined),
     *   subject: (string|undefined),
     *   email: (string|undefined),
     *   phone: (string|undefined),
     *   rss: (string|undefined),
     *   event_start_date: (number|undefined),
     *   event_finish_date: (number|undefined),
     *   event_group_id: (number|undefined),
     *   public_category: (number|undefined),
     *   public_subcategory: (number|undefined),
     *   public_date: (string|undefined),
     *   wall: (number|undefined),
     *   topics: (number|undefined),
     *   photos: (number|undefined),
     *   video: (number|undefined),
     *   audio: (number|undefined),
     *   links: (boolean|undefined),
     *   events: (boolean|undefined),
     *   places: (boolean|undefined),
     *   contacts: (boolean|undefined),
     *   docs: (number|undefined),
     *   wiki: (number|undefined),
     *   messages: (boolean|undefined),
     *   age_limits: (number|undefined),
     *   market: (boolean|undefined),
     *   market_comments: (boolean|undefined),
     *   market_country: (number[]|undefined),
     *   market_city: (number[]|undefined),
     *   market_currency: (number|undefined),
     *   market_contact: (number|undefined),
     *   market_wiki: (number|undefined),
     *   obscene_filter: (boolean|undefined),
     *   obscene_stopwords: (boolean|undefined),
     *   obscene_words: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.edit", params);
        });
    }
    /**
     * Edits the place in community.
     *
     * @param {{
     *   group_id: (number),
     *   title: (string|undefined),
     *   address: (string|undefined),
     *   country_id: (number|undefined),
     *   city_id: (number|undefined),
     *   latitude: (number|undefined),
     *   longitude: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsEditPlaceResponse>}
     */
    groupsEditPlace(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.editPlace", params);
        });
    }
    /**
     * Returns community settings.
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetSettingsResponse>}
     */
    groupsGetSettings(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getSettings", params);
        });
    }
    /**
     * Returns a list of requests to the community.
     *
     * @param {{
     *   group_id: (number),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetRequestsResponse>}
     */
    groupsGetRequests(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getRequests", params);
        });
    }
    /**
     * Allows to add, remove or edit the community manager.
     *
     * @param {{
     *   group_id: (number),
     *   user_id: (number),
     *   role: (string|undefined),
     *   is_contact: (boolean|undefined),
     *   contact_position: (string|undefined),
     *   contact_phone: (string|undefined),
     *   contact_email: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsEditManager(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.editManager", params);
        });
    }
    /**
     * Allows to invite friends to the community.
     *
     * @param {{
     *   group_id: (number),
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsInvite(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.invite", params);
        });
    }
    /**
     * Allows to add a link to the community.
     *
     * @param {{
     *   group_id: (number),
     *   link: (string),
     *   text: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsAddLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.addLink", params);
        });
    }
    /**
     * Allows to delete a link from the community.
     *
     * @param {{
     *   group_id: (number),
     *   link_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsDeleteLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.deleteLink", params);
        });
    }
    /**
     * Allows to edit a link in the community.
     *
     * @param {{
     *   group_id: (number),
     *   link_id: (number),
     *   text: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsEditLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.editLink", params);
        });
    }
    /**
     * Allows to reorder links in the community.
     *
     * @param {{
     *   group_id: (number),
     *   link_id: (number),
     *   after: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsReorderLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.reorderLink", params);
        });
    }
    /**
     * Removes a user from the community.
     *
     * @param {{
     *   group_id: (number),
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsRemoveUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.removeUser", params);
        });
    }
    /**
     * Allows to approve join request to the community.
     *
     * @param {{
     *   group_id: (number),
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsApproveRequest(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.approveRequest", params);
        });
    }
    /**
     * Returns Callback API confirmation code for the community.
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetCallbackConfirmationCodeResponse>}
     */
    groupsGetCallbackConfirmationCode(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getCallbackConfirmationCode", params);
        });
    }
    /**
     * Returns [vk.com/dev/callback_api|Callback API] notifications settings.
     *
     * @param {{
     *   group_id: (number),
     *   server_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetCallbackSettingsResponse>}
     */
    groupsGetCallbackSettings(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getCallbackSettings", params);
        });
    }
    /**
     * Allow to set notifications settings for group.
     *
     * @param {{
     *   group_id: (number),
     *   server_id: (number|undefined),
     *   message_new: (boolean|undefined),
     *   message_reply: (boolean|undefined),
     *   message_allow: (boolean|undefined),
     *   message_deny: (boolean|undefined),
     *   photo_new: (boolean|undefined),
     *   audio_new: (boolean|undefined),
     *   video_new: (boolean|undefined),
     *   wall_reply_new: (boolean|undefined),
     *   wall_reply_edit: (boolean|undefined),
     *   wall_reply_delete: (boolean|undefined),
     *   wall_reply_restore: (boolean|undefined),
     *   wall_post_new: (boolean|undefined),
     *   wall_repost: (boolean|undefined),
     *   board_post_new: (boolean|undefined),
     *   board_post_edit: (boolean|undefined),
     *   board_post_restore: (boolean|undefined),
     *   board_post_delete: (boolean|undefined),
     *   photo_comment_new: (boolean|undefined),
     *   photo_comment_edit: (boolean|undefined),
     *   photo_comment_delete: (boolean|undefined),
     *   photo_comment_restore: (boolean|undefined),
     *   video_comment_new: (boolean|undefined),
     *   video_comment_edit: (boolean|undefined),
     *   video_comment_delete: (boolean|undefined),
     *   video_comment_restore: (boolean|undefined),
     *   market_comment_new: (boolean|undefined),
     *   market_comment_edit: (boolean|undefined),
     *   market_comment_delete: (boolean|undefined),
     *   market_comment_restore: (boolean|undefined),
     *   poll_vote_new: (boolean|undefined),
     *   group_join: (boolean|undefined),
     *   group_leave: (boolean|undefined),
     *   user_block: (boolean|undefined),
     *   user_unblock: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsSetCallbackSettings(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.setCallbackSettings", params);
        });
    }
    /**
     * Returns the data needed to query a Long Poll server for events
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetLongPollServerResponse>}
     */
    groupsGetLongPollServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getLongPollServer", params);
        });
    }
    /**
     * Returns Long Poll notification settings
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GroupsGetLongPollSettingsResponse>}
     */
    groupsGetLongPollSettings(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.getLongPollSettings", params);
        });
    }
    /**
     * Sets Long Poll notification settings
     *
     * @param {{
     *   group_id: (number),
     *   enabled: (boolean|undefined),
     *   message_new: (boolean|undefined),
     *   message_reply: (boolean|undefined),
     *   message_edit: (boolean|undefined),
     *   message_allow: (boolean|undefined),
     *   message_deny: (boolean|undefined),
     *   photo_new: (boolean|undefined),
     *   audio_new: (boolean|undefined),
     *   video_new: (boolean|undefined),
     *   wall_reply_new: (boolean|undefined),
     *   wall_reply_edit: (boolean|undefined),
     *   wall_reply_delete: (boolean|undefined),
     *   wall_reply_restore: (boolean|undefined),
     *   wall_post_new: (boolean|undefined),
     *   wall_repost: (boolean|undefined),
     *   board_post_new: (boolean|undefined),
     *   board_post_edit: (boolean|undefined),
     *   board_post_restore: (boolean|undefined),
     *   board_post_delete: (boolean|undefined),
     *   photo_comment_new: (boolean|undefined),
     *   photo_comment_edit: (boolean|undefined),
     *   photo_comment_delete: (boolean|undefined),
     *   photo_comment_restore: (boolean|undefined),
     *   video_comment_new: (boolean|undefined),
     *   video_comment_edit: (boolean|undefined),
     *   video_comment_delete: (boolean|undefined),
     *   video_comment_restore: (boolean|undefined),
     *   market_comment_new: (boolean|undefined),
     *   market_comment_edit: (boolean|undefined),
     *   market_comment_delete: (boolean|undefined),
     *   market_comment_restore: (boolean|undefined),
     *   poll_vote_new: (boolean|undefined),
     *   group_join: (boolean|undefined),
     *   group_leave: (boolean|undefined),
     *   user_block: (boolean|undefined),
     *   user_unblock: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    groupsSetLongPollSettings(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("groups.setLongPollSettings", params);
        });
    }
    /**
     * Returns a list of topics on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_ids: (number[]|undefined),
     *   order: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   preview: (number|undefined),
     *   preview_length: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.BoardGetTopicsResponse>}
     */
    boardGetTopics(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.getTopics", params);
        });
    }
    /**
     * Returns a list of comments on a topic on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   need_likes: (boolean|undefined),
     *   start_comment_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   sort: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.BoardGetCommentsResponse>}
     */
    boardGetComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.getComments", params);
        });
    }
    /**
     * Creates a new topic on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   title: (string),
     *   text: (string|undefined),
     *   from_group: (boolean|undefined),
     *   attachments: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.BoardAddTopicResponse>}
     */
    boardAddTopic(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.addTopic", params);
        });
    }
    /**
     * Adds a comment on a topic on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   from_group: (boolean|undefined),
     *   sticker_id: (number|undefined),
     *   guid: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.BoardCreateCommentResponse>}
     */
    boardCreateComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.createComment", params);
        });
    }
    /**
     * Deletes a topic from a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardDeleteTopic(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.deleteTopic", params);
        });
    }
    /**
     * Edits the title of a topic on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   title: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardEditTopic(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.editTopic", params);
        });
    }
    /**
     * Edits a comment on a topic on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   comment_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardEditComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.editComment", params);
        });
    }
    /**
     * Restores a comment deleted from a topic on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardRestoreComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.restoreComment", params);
        });
    }
    /**
     * Deletes a comment on a topic on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardDeleteComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.deleteComment", params);
        });
    }
    /**
     * Re-opens a previously closed topic on a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardOpenTopic(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.openTopic", params);
        });
    }
    /**
     * Closes a topic on a community's discussion board so that comments cannot be posted.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardCloseTopic(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.closeTopic", params);
        });
    }
    /**
     * Pins a topic (fixes its place) to the top of a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardFixTopic(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.fixTopic", params);
        });
    }
    /**
     * Unpins a pinned topic from the top of a community's discussion board.
     *
     * @param {{
     *   group_id: (number),
     *   topic_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    boardUnfixTopic(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("board.unfixTopic", params);
        });
    }
    /**
     * Returns detailed information about videos.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   videos: (string[]|undefined),
     *   album_id: (number|undefined),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetResponse>}
     */
    videoGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.get", params);
        });
    }
    /**
     * Edits information about a video on a user or community page.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   video_id: (number),
     *   name: (string|undefined),
     *   desc: (string|undefined),
     *   privacy_view: (string[]|undefined),
     *   privacy_comment: (string[]|undefined),
     *   no_comments: (boolean|undefined),
     *   repeat: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.edit", params);
        });
    }
    /**
     * Adds a video to a user or community page.
     *
     * @param {{
     *   target_id: (number|undefined),
     *   video_id: (number),
     *   owner_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoAdd(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.add", params);
        });
    }
    /**
     * Returns a server address (required for upload) and video data.
     *
     * @param {{
     *   name: (string|undefined),
     *   description: (string|undefined),
     *   is_private: (boolean|undefined),
     *   wallpost: (boolean|undefined),
     *   link: (string|undefined),
     *   group_id: (number|undefined),
     *   album_id: (number|undefined),
     *   privacy_view: (string[]|undefined),
     *   privacy_comment: (string[]|undefined),
     *   no_comments: (boolean|undefined),
     *   repeat: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoSaveResponse>}
     */
    videoSave(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.save", params);
        });
    }
    /**
     * Deletes a video from a user or community page.
     *
     * @param {{
     *   video_id: (number),
     *   owner_id: (number|undefined),
     *   target_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.delete", params);
        });
    }
    /**
     * Restores a previously deleted video.
     *
     * @param {{
     *   video_id: (number),
     *   owner_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoRestore(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.restore", params);
        });
    }
    /**
     * Returns a list of videos under the set search criterion.
     *
     * @param {{
     *   q: (string),
     *   sort: (number|undefined),
     *   hd: (number|undefined),
     *   adult: (boolean|undefined),
     *   filters: (string[]|undefined),
     *   search_own: (boolean|undefined),
     *   offset: (number|undefined),
     *   longer: (number|undefined),
     *   shorter: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoSearchResponse>}
     */
    videoSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.search", params);
        });
    }
    /**
     * Returns list of videos in which the user is tagged.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetUserVideosResponse>}
     */
    videoGetUserVideos(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getUserVideos", params);
        });
    }
    /**
     * Returns a list of video albums owned by a user or community.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetAlbumsResponse>}
     */
    videoGetAlbums(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getAlbums", params);
        });
    }
    /**
     * Returns video album info
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   album_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetAlbumByIdResponse>}
     */
    videoGetAlbumById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getAlbumById", params);
        });
    }
    /**
     * Creates an empty album for videos.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   title: (string|undefined),
     *   privacy: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoAddAlbumResponse>}
     */
    videoAddAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.addAlbum", params);
        });
    }
    /**
     * Edits the title of a video album.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   album_id: (number),
     *   title: (string),
     *   privacy: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoEditAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.editAlbum", params);
        });
    }
    /**
     * Deletes a video album.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   album_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoDeleteAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.deleteAlbum", params);
        });
    }
    /**
     * Reorders the album in the list of user video albums.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   album_id: (number),
     *   before: (number|undefined),
     *   after: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoReorderAlbums(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.reorderAlbums", params);
        });
    }
    /**
     * Reorders the video in the video album.
     *
     * @param {{
     *   target_id: (number|undefined),
     *   album_id: (number|undefined),
     *   owner_id: (number),
     *   video_id: (number),
     *   before_owner_id: (number|undefined),
     *   before_video_id: (number|undefined),
     *   after_owner_id: (number|undefined),
     *   after_video_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoReorderVideos(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.reorderVideos", params);
        });
    }
    /**
     * undefined
     *
     * @param {{
     *   target_id: (number|undefined),
     *   album_id: (number|undefined),
     *   album_ids: (number[]|undefined),
     *   owner_id: (number),
     *   video_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoAddToAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.addToAlbum", params);
        });
    }
    /**
     * undefined
     *
     * @param {{
     *   target_id: (number|undefined),
     *   album_id: (number|undefined),
     *   album_ids: (number[]|undefined),
     *   owner_id: (number),
     *   video_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoRemoveFromAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.removeFromAlbum", params);
        });
    }
    /**
     * undefined
     *
     * @param {{
     *   target_id: (number|undefined),
     *   owner_id: (number),
     *   video_id: (number),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetAlbumsByVideoResponse>}
     */
    videoGetAlbumsByVideo(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getAlbumsByVideo", params);
        });
    }
    /**
     * Returns a list of comments on a video.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   video_id: (number),
     *   need_likes: (boolean|undefined),
     *   start_comment_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   sort: (string|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetCommentsResponse>}
     */
    videoGetComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getComments", params);
        });
    }
    /**
     * Adds a new comment on a video.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   video_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   from_group: (boolean|undefined),
     *   reply_to_comment: (number|undefined),
     *   sticker_id: (number|undefined),
     *   guid: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoCreateCommentResponse>}
     */
    videoCreateComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.createComment", params);
        });
    }
    /**
     * Deletes a comment on a video.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoDeleteComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.deleteComment", params);
        });
    }
    /**
     * Restores a previously deleted comment on a video.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoRestoreCommentResponse>}
     */
    videoRestoreComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.restoreComment", params);
        });
    }
    /**
     * Edits the text of a comment on a video.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   comment_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoEditComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.editComment", params);
        });
    }
    /**
     * Returns a list of tags on a video.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   video_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetTagsResponse>}
     */
    videoGetTags(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getTags", params);
        });
    }
    /**
     * Adds a tag on a video.
     *
     * @param {{
     *   user_id: (number),
     *   owner_id: (number|undefined),
     *   video_id: (number),
     *   tagged_name: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoPutTagResponse>}
     */
    videoPutTag(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.putTag", params);
        });
    }
    /**
     * Removes a tag from a video.
     *
     * @param {{
     *   tag_id: (number),
     *   owner_id: (number|undefined),
     *   video_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoRemoveTag(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.removeTag", params);
        });
    }
    /**
     * Returns a list of videos with tags that have not been viewed.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetNewTagsResponse>}
     */
    videoGetNewTags(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getNewTags", params);
        });
    }
    /**
     * Reports (submits a complaint about) a video.
     *
     * @param {{
     *   owner_id: (number),
     *   video_id: (number),
     *   reason: (number|undefined),
     *   comment: (string|undefined),
     *   search_query: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoReport(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.report", params);
        });
    }
    /**
     * Reports (submits a complaint about) a comment on a video.
     *
     * @param {{
     *   owner_id: (number),
     *   comment_id: (number),
     *   reason: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoReportComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.reportComment", params);
        });
    }
    /**
     * Returns video catalog
     *
     * @param {{
     *   count: (number|undefined),
     *   items_count: (number|undefined),
     *   from: (string|undefined),
     *   filters: (string[]|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetCatalogResponse>}
     */
    videoGetCatalog(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getCatalog", params);
        });
    }
    /**
     * Returns a separate catalog section
     *
     * @param {{
     *   section_id: (string),
     *   from: (string),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.VideoGetCatalogSectionResponse>}
     */
    videoGetCatalogSection(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.getCatalogSection", params);
        });
    }
    /**
     * Hides a video catalog section from a user.
     *
     * @param {{
     *   section_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    videoHideCatalogSection(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("video.hideCatalogSection", params);
        });
    }
    /**
     * Returns a list of notes created by a user.
     *
     * @param {{
     *   note_ids: (number[]|undefined),
     *   user_id: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NotesGetResponse>}
     */
    notesGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.get", params);
        });
    }
    /**
     * Returns a note by its ID.
     *
     * @param {{
     *   note_id: (number),
     *   owner_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NotesGetByIdResponse>}
     */
    notesGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.getById", params);
        });
    }
    /**
     * Creates a new note for the current user.
     *
     * @param {{
     *   title: (string),
     *   text: (string),
     *   privacy_view: (string[]|undefined),
     *   privacy_comment: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NotesAddResponse>}
     */
    notesAdd(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.add", params);
        });
    }
    /**
     * Edits a note of the current user.
     *
     * @param {{
     *   note_id: (number),
     *   title: (string),
     *   text: (string),
     *   privacy_view: (string[]|undefined),
     *   privacy_comment: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    notesEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.edit", params);
        });
    }
    /**
     * Deletes a note of the current user.
     *
     * @param {{
     *   note_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    notesDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.delete", params);
        });
    }
    /**
     * Returns a list of comments on a note.
     *
     * @param {{
     *   note_id: (number),
     *   owner_id: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NotesGetCommentsResponse>}
     */
    notesGetComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.getComments", params);
        });
    }
    /**
     * Adds a new comment on a note.
     *
     * @param {{
     *   note_id: (number),
     *   owner_id: (number|undefined),
     *   reply_to: (number|undefined),
     *   message: (string),
     *   guid: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NotesCreateCommentResponse>}
     */
    notesCreateComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.createComment", params);
        });
    }
    /**
     * Edits a comment on a note.
     *
     * @param {{
     *   comment_id: (number),
     *   owner_id: (number|undefined),
     *   message: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    notesEditComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.editComment", params);
        });
    }
    /**
     * Deletes a comment on a note.
     *
     * @param {{
     *   comment_id: (number),
     *   owner_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    notesDeleteComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.deleteComment", params);
        });
    }
    /**
     * Restores a deleted comment on a note.
     *
     * @param {{
     *   comment_id: (number),
     *   owner_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    notesRestoreComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notes.restoreComment", params);
        });
    }
    /**
     * Adds a new location to the location database.
     *
     * @param {{
     *   type: (number|undefined),
     *   title: (string),
     *   latitude: (number),
     *   longitude: (number),
     *   country: (number|undefined),
     *   city: (number|undefined),
     *   address: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PlacesAddResponse>}
     */
    placesAdd(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("places.add", params);
        });
    }
    /**
     * Returns information about locations by their IDs.
     *
     * @param {{
     *   places: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PlacesGetByIdResponse>}
     */
    placesGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("places.getById", params);
        });
    }
    /**
     * Returns a list of locations that match the search criteria.
     *
     * @param {{
     *   q: (string|undefined),
     *   city: (number|undefined),
     *   latitude: (number),
     *   longitude: (number),
     *   radius: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PlacesSearchResponse>}
     */
    placesSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("places.search", params);
        });
    }
    /**
     * Checks a user in at the specified location.
     *
     * @param {{
     *   place_id: (number|undefined),
     *   text: (string|undefined),
     *   latitude: (number|undefined),
     *   longitude: (number|undefined),
     *   friends_only: (boolean|undefined),
     *   services: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PlacesCheckinResponse>}
     */
    placesCheckin(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("places.checkin", params);
        });
    }
    /**
     * Returns a list of user check-ins at locations according to the set parameters.
     *
     * @param {{
     *   latitude: (number|undefined),
     *   longitude: (number|undefined),
     *   place: (number|undefined),
     *   user_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   timestamp: (number|undefined),
     *   friends_only: (boolean|undefined),
     *   need_places: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PlacesGetCheckinsResponse>}
     */
    placesGetCheckins(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("places.getCheckins", params);
        });
    }
    /**
     * Returns a list of all types of locations.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PlacesGetTypesResponse>}
     */
    placesGetTypes(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("places.getTypes", params);
        });
    }
    /**
     * Returns non-null values of user counters.
     *
     * @param {{
     *   filter: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountGetCountersResponse>}
     */
    accountGetCounters(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.getCounters", params);
        });
    }
    /**
     * Sets an application screen name (up to 17 characters), that is shown to the user in the left menu.
     *
     * @param {{
     *   user_id: (number),
     *   name: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountSetNameInMenu(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.setNameInMenu", params);
        });
    }
    /**
     * Marks the current user as online for 15 minutes.
     *
     * @param {{
     *   voip: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountSetOnline(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.setOnline", params);
        });
    }
    /**
     * Marks a current user as offline.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountSetOffline(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.setOffline", params);
        });
    }
    /**
     * Allows to search the VK users using phone numbers, e-mail addresses and user IDs on other services.
     *
     * @param {{
     *   contacts: (string[]|undefined),
     *   service: (string),
     *   mycontact: (string|undefined),
     *   return_all: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountLookupContactsResponse>}
     */
    accountLookupContacts(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.lookupContacts", params);
        });
    }
    /**
     * Subscribes an iOS/Android/Windows Phone-based device to receive push notifications
     *
     * @param {{
     *   token: (string),
     *   device_model: (string|undefined),
     *   device_year: (number|undefined),
     *   device_id: (string),
     *   system_version: (string|undefined),
     *   settings: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountRegisterDevice(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.registerDevice", params);
        });
    }
    /**
     * Unsubscribes a device from push notifications.
     *
     * @param {{
     *   device_id: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountUnregisterDevice(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.unregisterDevice", params);
        });
    }
    /**
     * Mutes push notifications for the set period of time.
     *
     * @param {{
     *   device_id: (string|undefined),
     *   time: (number|undefined),
     *   peer_id: (number|undefined),
     *   sound: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountSetSilenceMode(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.setSilenceMode", params);
        });
    }
    /**
     * Gets settings of push notifications.
     *
     * @param {{
     *   device_id: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountGetPushSettingsResponse>}
     */
    accountGetPushSettings(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.getPushSettings", params);
        });
    }
    /**
     * Change push settings.
     *
     * @param {{
     *   device_id: (string),
     *   settings: (string|undefined),
     *   key: (string|undefined),
     *   value: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountSetPushSettings(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.setPushSettings", params);
        });
    }
    /**
     * Gets settings of the user in this application.
     *
     * @param {{
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountGetAppPermissionsResponse>}
     */
    accountGetAppPermissions(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.getAppPermissions", params);
        });
    }
    /**
     * Returns a list of active ads (offers) which executed by the user will bring him/her respective number of votes to his balance in the application.
     *
     * @param {{
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountGetActiveOffersResponse>}
     */
    accountGetActiveOffers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.getActiveOffers", params);
        });
    }
    /**
     * Adds user to the banlist.
     *
     * @param {{
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountBanUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.banUser", params);
        });
    }
    /**
     * Deletes user from the blacklist.
     *
     * @param {{
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountUnbanUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.unbanUser", params);
        });
    }
    /**
     * Returns a user's blacklist.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountGetBannedResponse>}
     */
    accountGetBanned(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.getBanned", params);
        });
    }
    /**
     * Returns current account info.
     *
     * @param {{
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountGetInfoResponse>}
     */
    accountGetInfo(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.getInfo", params);
        });
    }
    /**
     * Allows to edit the current account info.
     *
     * @param {{
     *   name: (string|undefined),
     *   value: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    accountSetInfo(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.setInfo", params);
        });
    }
    /**
     * Changes a user password after access is successfully restored with the [vk.com/dev/auth.restore|auth.restore] method.
     *
     * @param {{
     *   restore_sid: (string|undefined),
     *   change_password_hash: (string|undefined),
     *   old_password: (string|undefined),
     *   new_password: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountChangePasswordResponse>}
     */
    accountChangePassword(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.changePassword", params);
        });
    }
    /**
     * Returns the current account info.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountGetProfileInfoResponse>}
     */
    accountGetProfileInfo(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.getProfileInfo", params);
        });
    }
    /**
     * Edits current profile info.
     *
     * @param {{
     *   first_name: (string|undefined),
     *   last_name: (string|undefined),
     *   maiden_name: (string|undefined),
     *   screen_name: (string|undefined),
     *   cancel_request_id: (number|undefined),
     *   sex: (number|undefined),
     *   relation: (number|undefined),
     *   relation_partner_id: (number|undefined),
     *   bdate: (string|undefined),
     *   bdate_visibility: (number|undefined),
     *   home_town: (string|undefined),
     *   country_id: (number|undefined),
     *   city_id: (number|undefined),
     *   status: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AccountSaveProfileInfoResponse>}
     */
    accountSaveProfileInfo(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("account.saveProfileInfo", params);
        });
    }
    /**
     * Returns a list of the current user's incoming or outgoing private messages.
     *
     * @param {{
     *   out: (boolean|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   filter: (number|undefined),
     *   time_offset: (number|undefined),
     *   preview_length: (number|undefined),
     *   last_message_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetResponse>}
     */
    messagesGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.get", params);
        });
    }
    /**
     * Returns a list of the current user's conversations.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   start_message_id: (number|undefined),
     *   preview_length: (number|undefined),
     *   unread: (boolean|undefined),
     *   important: (boolean|undefined),
     *   unanswered: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetDialogsResponse>}
     */
    messagesGetDialogs(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getDialogs", params);
        });
    }
    /**
     * Returns messages by their IDs.
     *
     * @param {{
     *   message_ids: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetByIdResponse>}
     */
    messagesGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getById", params);
        });
    }
    /**
     * Returns a list of the current user's private messages that match search criteria.
     *
     * @param {{
     *   q: (string|undefined),
     *   peer_id: (number|undefined),
     *   date: (number|undefined),
     *   preview_length: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesSearchResponse>}
     */
    messagesSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.search", params);
        });
    }
    /**
     * Returns message history for the specified user or group chat.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   user_id: (number|undefined),
     *   peer_id: (number|undefined),
     *   start_message_id: (number|undefined),
     *   rev: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetHistoryResponse>}
     */
    messagesGetHistory(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getHistory", params);
        });
    }
    /**
     * Returns media files from the dialog or group chat.
     *
     * @param {{
     *   peer_id: (number),
     *   media_type: (string|undefined),
     *   start_from: (string|undefined),
     *   count: (number|undefined),
     *   photo_sizes: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetHistoryAttachmentsResponse>}
     */
    messagesGetHistoryAttachments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getHistoryAttachments", params);
        });
    }
    /**
     * Sends a message.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   random_id: (number|undefined),
     *   peer_id: (number|undefined),
     *   domain: (string|undefined),
     *   chat_id: (number|undefined),
     *   user_ids: (number[]|undefined),
     *   message: (string|undefined),
     *   lat: (number|undefined),
     *   long: (number|undefined),
     *   attachment: (string|undefined),
     *   forward_messages: (string|undefined),
     *   sticker_id: (number|undefined),
     *   notification: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesSendResponse>}
     */
    messagesSend(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.send", params);
        });
    }
    /**
     * Deletes one or more messages.
     *
     * @param {{
     *   message_ids: (number[]|undefined),
     *   spam: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesDeleteResponse>}
     */
    messagesDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.delete", params);
        });
    }
    /**
     * Deletes all private messages in a conversation.
     *
     * @param {{
     *   user_id: (string|undefined),
     *   peer_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesDeleteDialog(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.deleteDialog", params);
        });
    }
    /**
     * Restores a deleted message.
     *
     * @param {{
     *   message_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesRestore(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.restore", params);
        });
    }
    /**
     * Marks messages as read.
     *
     * @param {{
     *   message_ids: (number[]|undefined),
     *   peer_id: (string|undefined),
     *   start_message_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesMarkAsRead(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.markAsRead", params);
        });
    }
    /**
     * Marks and unmarks messages as important (starred).
     *
     * @param {{
     *   message_ids: (number[]|undefined),
     *   important: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesMarkAsImportantResponse>}
     */
    messagesMarkAsImportant(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.markAsImportant", params);
        });
    }
    /**
     * Marks and unmarks dialogs as important.
     *
     * @param {{
     *   peer_id: (number[]|undefined),
     *   important: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesMarkAsImportantDialog(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.markAsImportantDialog", params);
        });
    }
    /**
     * Marks and unmarks dialogs as unanswered.
     *
     * @param {{
     *   peer_id: (number[]|undefined),
     *   important: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesMarkAsUnansweredDialog(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.markAsUnansweredDialog", params);
        });
    }
    /**
     * Returns data required for connection to a Long Poll server.
     *
     * @param {{
     *   lp_version: (number|undefined),
     *   need_pts: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetLongPollServerResponse>}
     */
    messagesGetLongPollServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getLongPollServer", params);
        });
    }
    /**
     * Returns updates in user's private messages.
     *
     * @param {{
     *   ts: (number|undefined),
     *   pts: (number|undefined),
     *   preview_length: (number|undefined),
     *   onlines: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   events_limit: (number|undefined),
     *   msgs_limit: (number|undefined),
     *   max_msg_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetLongPollHistoryResponse>}
     */
    messagesGetLongPollHistory(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getLongPollHistory", params);
        });
    }
    /**
     * Returns information about a chat.
     *
     * @param {{
     *   chat_id: (number|undefined),
     *   chat_ids: (number[]|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetChatResponse>}
     */
    messagesGetChat(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getChat", params);
        });
    }
    /**
     * Creates a chat with several participants.
     *
     * @param {{
     *   user_ids: (number[]),
     *   title: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesCreateChatResponse>}
     */
    messagesCreateChat(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.createChat", params);
        });
    }
    /**
     * Edits the title of a chat.
     *
     * @param {{
     *   chat_id: (number),
     *   title: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesEditChat(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.editChat", params);
        });
    }
    /**
     * Returns a list of IDs of users participating in a chat.
     *
     * @param {{
     *   chat_id: (number|undefined),
     *   chat_ids: (number[]|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetChatUsersResponse>}
     */
    messagesGetChatUsers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getChatUsers", params);
        });
    }
    /**
     * Changes the status of a user as typing in a conversation.
     *
     * @param {{
     *   user_id: (string|undefined),
     *   type: (string|undefined),
     *   peer_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesSetActivity(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.setActivity", params);
        });
    }
    /**
     * Returns a list of the current user's conversations that match search criteria.
     *
     * @param {{
     *   q: (string|undefined),
     *   limit: (number|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesSearchDialogsResponse>}
     */
    messagesSearchDialogs(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.searchDialogs", params);
        });
    }
    /**
     * Adds a new user to a chat.
     *
     * @param {{
     *   chat_id: (number),
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesAddChatUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.addChatUser", params);
        });
    }
    /**
     * Allows the current user to leave a chat or, if the current user started the chat, allows the user to remove another user from the chat.
     *
     * @param {{
     *   chat_id: (number),
     *   user_id: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesRemoveChatUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.removeChatUser", params);
        });
    }
    /**
     * Returns a user's current status and date of last activity.
     *
     * @param {{
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesGetLastActivityResponse>}
     */
    messagesGetLastActivity(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.getLastActivity", params);
        });
    }
    /**
     * Sets a previously-uploaded picture as the cover picture of a chat.
     *
     * @param {{
     *   file: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesSetChatPhotoResponse>}
     */
    messagesSetChatPhoto(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.setChatPhoto", params);
        });
    }
    /**
     * Deletes a chat's cover picture.
     *
     * @param {{
     *   chat_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesDeleteChatPhotoResponse>}
     */
    messagesDeleteChatPhoto(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.deleteChatPhoto", params);
        });
    }
    /**
     * Denies sending message from community to the current user.
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesDenyMessagesFromGroup(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.denyMessagesFromGroup", params);
        });
    }
    /**
     * Allows sending messages from community to the current user.
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    messagesAllowMessagesFromGroup(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.allowMessagesFromGroup", params);
        });
    }
    /**
     * Returns information whether sending messages from the community to current user is allowed.
     *
     * @param {{
     *   group_id: (number),
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MessagesIsMessagesFromGroupAllowedResponse>}
     */
    messagesIsMessagesFromGroupAllowed(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("messages.isMessagesFromGroupAllowed", params);
        });
    }
    /**
     * Returns data required to show newsfeed for the current user.
     *
     * @param {{
     *   filters: (string[]|undefined),
     *   return_banned: (boolean|undefined),
     *   start_time: (number|undefined),
     *   end_time: (number|undefined),
     *   max_photos: (number|undefined),
     *   source_ids: (string[]|undefined),
     *   start_from: (string|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedGetResponse>}
     */
    newsfeedGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.get", params);
        });
    }
    /**
     * , Returns a list of newsfeeds recommended to the current user.
     *
     * @param {{
     *   start_time: (number|undefined),
     *   end_time: (number|undefined),
     *   max_photos: (number|undefined),
     *   start_from: (string|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedGetRecommendedResponse>}
     */
    newsfeedGetRecommended(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.getRecommended", params);
        });
    }
    /**
     * Returns a list of comments in the current user's newsfeed.
     *
     * @param {{
     *   count: (number|undefined),
     *   filters: (string[]|undefined),
     *   reposts: (string|undefined),
     *   start_time: (number|undefined),
     *   end_time: (number|undefined),
     *   start_from: (string|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedGetCommentsResponse>}
     */
    newsfeedGetComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.getComments", params);
        });
    }
    /**
     * Returns a list of posts on user walls in which the current user is mentioned.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   start_time: (number|undefined),
     *   end_time: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedGetMentionsResponse>}
     */
    newsfeedGetMentions(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.getMentions", params);
        });
    }
    /**
     * Returns a list of users and communities banned from the current user's newsfeed.
     *
     * @param {{
     *   extended: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedGetBannedResponse>}
     */
    newsfeedGetBanned(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.getBanned", params);
        });
    }
    /**
     * Prevents news from specified users and communities from appearing in the current user's newsfeed.
     *
     * @param {{
     *   user_ids: (number[]|undefined),
     *   group_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    newsfeedAddBan(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.addBan", params);
        });
    }
    /**
     * Allows news from previously banned users and communities to be shown in the current user's newsfeed.
     *
     * @param {{
     *   user_ids: (number[]|undefined),
     *   group_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    newsfeedDeleteBan(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.deleteBan", params);
        });
    }
    /**
     * Hides an item from the newsfeed.
     *
     * @param {{
     *   type: (string),
     *   owner_id: (number),
     *   item_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    newsfeedIgnoreItem(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.ignoreItem", params);
        });
    }
    /**
     * Returns a hidden item to the newsfeed.
     *
     * @param {{
     *   type: (string),
     *   owner_id: (number),
     *   item_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    newsfeedUnignoreItem(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.unignoreItem", params);
        });
    }
    /**
     * Returns search results by statuses.
     *
     * @param {{
     *   q: (string|undefined),
     *   extended: (boolean|undefined),
     *   count: (number|undefined),
     *   latitude: (number|undefined),
     *   longitude: (number|undefined),
     *   start_time: (number|undefined),
     *   end_time: (number|undefined),
     *   start_from: (string|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedSearchResponse>}
     */
    newsfeedSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.search", params);
        });
    }
    /**
     * Returns a list of newsfeeds followed by the current user.
     *
     * @param {{
     *   list_ids: (number[]|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedGetListsResponse>}
     */
    newsfeedGetLists(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.getLists", params);
        });
    }
    /**
     * Creates and edits user newsfeed lists
     *
     * @param {{
     *   list_id: (number|undefined),
     *   title: (string),
     *   source_ids: (number[]|undefined),
     *   no_reposts: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedSaveListResponse>}
     */
    newsfeedSaveList(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.saveList", params);
        });
    }
    /**
     * undefined
     *
     * @param {{
     *   list_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    newsfeedDeleteList(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.deleteList", params);
        });
    }
    /**
     * Unsubscribes the current user from specified newsfeeds.
     *
     * @param {{
     *   type: (string),
     *   owner_id: (number|undefined),
     *   item_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    newsfeedUnsubscribe(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.unsubscribe", params);
        });
    }
    /**
     * Returns communities and users that current user is suggested to follow.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   shuffle: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NewsfeedGetSuggestedSourcesResponse>}
     */
    newsfeedGetSuggestedSources(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("newsfeed.getSuggestedSources", params);
        });
    }
    /**
     * Returns a list of IDs of users who added the specified object to their 'Likes' list.
     *
     * @param {{
     *   type: (string),
     *   owner_id: (number|undefined),
     *   item_id: (number|undefined),
     *   page_url: (string|undefined),
     *   filter: (string|undefined),
     *   friends_only: (boolean|undefined),
     *   extended: (boolean|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   skip_own: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LikesGetListResponse>}
     */
    likesGetList(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("likes.getList", params);
        });
    }
    /**
     * Adds the specified object to the 'Likes' list of the current user.
     *
     * @param {{
     *   type: (string),
     *   owner_id: (number|undefined),
     *   item_id: (number),
     *   access_key: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LikesAddResponse>}
     */
    likesAdd(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("likes.add", params);
        });
    }
    /**
     * Deletes the specified object from the 'Likes' list of the current user.
     *
     * @param {{
     *   type: (string),
     *   owner_id: (number|undefined),
     *   item_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LikesDeleteResponse>}
     */
    likesDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("likes.delete", params);
        });
    }
    /**
     * Checks for the object in the 'Likes' list of the specified user.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   type: (string),
     *   owner_id: (number|undefined),
     *   item_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.LikesIsLikedResponse>}
     */
    likesIsLiked(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("likes.isLiked", params);
        });
    }
    /**
     * Returns detailed information about a poll by its ID.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   is_board: (boolean|undefined),
     *   poll_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PollsGetByIdResponse>}
     */
    pollsGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("polls.getById", params);
        });
    }
    /**
     * Adds the current user's vote to the selected answer in the poll.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   poll_id: (number),
     *   answer_id: (number),
     *   is_board: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PollsAddVoteResponse>}
     */
    pollsAddVote(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("polls.addVote", params);
        });
    }
    /**
     * Deletes the current user's vote from the selected answer in the poll.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   poll_id: (number),
     *   answer_id: (number),
     *   is_board: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PollsDeleteVoteResponse>}
     */
    pollsDeleteVote(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("polls.deleteVote", params);
        });
    }
    /**
     * Returns a list of IDs of users who selected specific answers in the poll.
     *
     * @param {{
     *   owner_id: (number|undefined),
     *   poll_id: (number),
     *   answer_ids: (number[]),
     *   is_board: (boolean|undefined),
     *   friends_only: (boolean|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PollsGetVotersResponse>}
     */
    pollsGetVoters(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("polls.getVoters", params);
        });
    }
    /**
     * Creates polls that can be attached to the users' or communities' posts.
     *
     * @param {{
     *   question: (string|undefined),
     *   is_anonymous: (boolean|undefined),
     *   owner_id: (number|undefined),
     *   add_answers: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.PollsCreateResponse>}
     */
    pollsCreate(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("polls.create", params);
        });
    }
    /**
     * Edits created polls
     *
     * @param {{
     *   owner_id: (number),
     *   poll_id: (number),
     *   question: (string|undefined),
     *   add_answers: (string|undefined),
     *   edit_answers: (string|undefined),
     *   delete_answers: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    pollsEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("polls.edit", params);
        });
    }
    /**
     * Returns detailed information about user or community documents.
     *
     * @param {{
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   owner_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DocsGetResponse>}
     */
    docsGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.get", params);
        });
    }
    /**
     * Returns information about documents by their IDs.
     *
     * @param {{
     *   docs: (string[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DocsGetByIdResponse>}
     */
    docsGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.getById", params);
        });
    }
    /**
     * Returns the server address for document upload.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DocsGetUploadServerResponse>}
     */
    docsGetUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.getUploadServer", params);
        });
    }
    /**
     * Returns the server address for document upload onto a user's or community's wall.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DocsGetWallUploadServerResponse>}
     */
    docsGetWallUploadServer(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.getWallUploadServer", params);
        });
    }
    /**
     * Saves a document after [vk.com/dev/upload_files_2|uploading it to a server].
     *
     * @param {{
     *   file: (string),
     *   title: (string|undefined),
     *   tags: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DocsSaveResponse>}
     */
    docsSave(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.save", params);
        });
    }
    /**
     * Deletes a user or community document.
     *
     * @param {{
     *   owner_id: (number),
     *   doc_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    docsDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.delete", params);
        });
    }
    /**
     * Copies a document to a user's or community's document list.
     *
     * @param {{
     *   owner_id: (number),
     *   doc_id: (number),
     *   access_key: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DocsAddResponse>}
     */
    docsAdd(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.add", params);
        });
    }
    /**
     * Returns documents types available for current user.
     *
     * @param {{
     *   owner_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DocsGetTypesResponse>}
     */
    docsGetTypes(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.getTypes", params);
        });
    }
    /**
     * Returns a list of documents matching the search criteria.
     *
     * @param {{
     *   q: (string),
     *   search_own: (boolean|undefined),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DocsSearchResponse>}
     */
    docsSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.search", params);
        });
    }
    /**
     * Edits a document.
     *
     * @param {{
     *   owner_id: (number),
     *   doc_id: (number),
     *   title: (string|undefined),
     *   tags: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    docsEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("docs.edit", params);
        });
    }
    /**
     * Returns a list of users whom the current user has bookmarked.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FaveGetUsersResponse>}
     */
    faveGetUsers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.getUsers", params);
        });
    }
    /**
     * Returns a list of photos that the current user has liked.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   photo_sizes: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FaveGetPhotosResponse>}
     */
    faveGetPhotos(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.getPhotos", params);
        });
    }
    /**
     * Returns a list of wall posts that the current user has liked.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FaveGetPostsResponse>}
     */
    faveGetPosts(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.getPosts", params);
        });
    }
    /**
     * Returns a list of videos that the current user has liked.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FaveGetVideosResponse>}
     */
    faveGetVideos(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.getVideos", params);
        });
    }
    /**
     * Returns a list of links that the current user has bookmarked.
     *
     * @param {{
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FaveGetLinksResponse>}
     */
    faveGetLinks(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.getLinks", params);
        });
    }
    /**
     * Returns market items bookmarked by current user.
     *
     * @param {{
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.FaveGetMarketItemsResponse>}
     */
    faveGetMarketItems(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.getMarketItems", params);
        });
    }
    /**
     * Adds a profile to user faves.
     *
     * @param {{
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    faveAddUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.addUser", params);
        });
    }
    /**
     * Removes a profile from user faves.
     *
     * @param {{
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    faveRemoveUser(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.removeUser", params);
        });
    }
    /**
     * Adds a community to user faves.
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    faveAddGroup(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.addGroup", params);
        });
    }
    /**
     * Removes a community from user faves.
     *
     * @param {{
     *   group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    faveRemoveGroup(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.removeGroup", params);
        });
    }
    /**
     * Adds a link to user faves.
     *
     * @param {{
     *   link: (string),
     *   text: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    faveAddLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.addLink", params);
        });
    }
    /**
     * Removes link from the user's faves.
     *
     * @param {{
     *   link_id: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    faveRemoveLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("fave.removeLink", params);
        });
    }
    /**
     * Returns a list of notifications about other users' feedback to the current user's wall posts.
     *
     * @param {{
     *   count: (number|undefined),
     *   start_from: (string|undefined),
     *   filters: (string[]|undefined),
     *   start_time: (number|undefined),
     *   end_time: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NotificationsGetResponse>}
     */
    notificationsGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notifications.get", params);
        });
    }
    /**
     * Resets the counter of new notifications about other users' feedback to the current user's wall posts.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.NotificationsMarkAsViewedResponse>}
     */
    notificationsMarkAsViewed(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("notifications.markAsViewed", params);
        });
    }
    /**
     * Returns statistics of a community or an application.
     *
     * @param {{
     *   group_id: (number|undefined),
     *   app_id: (number|undefined),
     *   date_from: (string|undefined),
     *   date_to: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StatsGetResponse>}
     */
    statsGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stats.get", params);
        });
    }
    /**
     * undefined
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    statsTrackVisitor(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stats.trackVisitor", params);
        });
    }
    /**
     * Returns stats for a wall post.
     *
     * @param {{
     *   owner_id: (number),
     *   post_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.StatsGetPostReachResponse>}
     */
    statsGetPostReach(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("stats.getPostReach", params);
        });
    }
    /**
     * Allows the programmer to do a quick search for any substring.
     *
     * @param {{
     *   q: (string|undefined),
     *   offset: (number|undefined),
     *   limit: (number|undefined),
     *   filters: (string[]|undefined),
     *   search_global: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.SearchGetHintsResponse>}
     */
    searchGetHints(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("search.getHints", params);
        });
    }
    /**
     * Returns a list of applications (apps) available to users in the App Catalog.
     *
     * @param {{
     *   sort: (string|undefined),
     *   offset: (number|undefined),
     *   count: (number),
     *   platform: (string|undefined),
     *   extended: (boolean|undefined),
     *   return_friends: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   q: (string|undefined),
     *   genre_id: (number|undefined),
     *   filter: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AppsGetCatalogResponse>}
     */
    appsGetCatalog(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("apps.getCatalog", params);
        });
    }
    /**
     * Returns applications data.
     *
     * @param {{
     *   app_id: (number|undefined),
     *   app_ids: (string[]|undefined),
     *   platform: (string|undefined),
     *   fields: (string[]|undefined),
     *   name_case: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AppsGetResponse>}
     */
    appsGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("apps.get", params);
        });
    }
    /**
     * Sends a request to another user in an app that uses VK authorization.
     *
     * @param {{
     *   user_id: (number),
     *   text: (string|undefined),
     *   type: (string|undefined),
     *   name: (string|undefined),
     *   key: (string|undefined),
     *   separate: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AppsSendRequestResponse>}
     */
    appsSendRequest(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("apps.sendRequest", params);
        });
    }
    /**
     * Deletes all request notifications from the current app.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    appsDeleteAppRequests(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("apps.deleteAppRequests", params);
        });
    }
    /**
     * Creates friends list for requests and invites in current app.
     *
     * @param {{
     *   count: (number|undefined),
     *   type: (string|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AppsGetFriendsListResponse>}
     */
    appsGetFriendsList(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("apps.getFriendsList", params);
        });
    }
    /**
     * Returns players rating in the game.
     *
     * @param {{
     *   type: (string),
     *   global: (boolean|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AppsGetLeaderboardResponse>}
     */
    appsGetLeaderboard(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("apps.getLeaderboard", params);
        });
    }
    /**
     * Adds user activity information to an application
     *
     * @param {{
     *   user_id: (number),
     *   activity_id: (number),
     *   value: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    secureAddAppEvent(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.addAppEvent", params);
        });
    }
    /**
     * Returns user score in app
     *
     * @param {{
     *   user_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AppsGetScoreResponse>}
     */
    appsGetScore(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("apps.getScore", params);
        });
    }
    /**
     * Checks whether a link is blocked in VK.
     *
     * @param {{
     *   url: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UtilsCheckLinkResponse>}
     */
    utilsCheckLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("utils.checkLink", params);
        });
    }
    /**
     * Deletes shortened link from user's list.
     *
     * @param {{
     *   key: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    utilsDeleteFromLastShortened(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("utils.deleteFromLastShortened", params);
        });
    }
    /**
     * Returns a list of user's shortened links.
     *
     * @param {{
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UtilsGetLastShortenedLinksResponse>}
     */
    utilsGetLastShortenedLinks(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("utils.getLastShortenedLinks", params);
        });
    }
    /**
     * Returns stats data for shortened link.
     *
     * @param {{
     *   key: (string),
     *   access_key: (string|undefined),
     *   interval: (string|undefined),
     *   intervals_count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UtilsGetLinkStatsResponse>}
     */
    utilsGetLinkStats(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("utils.getLinkStats", params);
        });
    }
    /**
     * Allows to receive a link shortened via vk.cc.
     *
     * @param {{
     *   url: (string),
     *   private: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UtilsGetShortLinkResponse>}
     */
    utilsGetShortLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("utils.getShortLink", params);
        });
    }
    /**
     * Detects a type of object (e.g., user, community, application) and its ID by screen name.
     *
     * @param {{
     *   screen_name: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UtilsResolveScreenNameResponse>}
     */
    utilsResolveScreenName(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("utils.resolveScreenName", params);
        });
    }
    /**
     * Returns the current time of the VK server.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.UtilsGetServerTimeResponse>}
     */
    utilsGetServerTime(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("utils.getServerTime", params);
        });
    }
    /**
     * Returns a list of countries.
     *
     * @param {{
     *   need_all: (boolean|undefined),
     *   code: (string|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetCountriesResponse>}
     */
    databaseGetCountries(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getCountries", params);
        });
    }
    /**
     * Returns a list of regions.
     *
     * @param {{
     *   country_id: (number),
     *   q: (string|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetRegionsResponse>}
     */
    databaseGetRegions(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getRegions", params);
        });
    }
    /**
     * Returns information about streets by their IDs.
     *
     * @param {{
     *   street_ids: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetStreetsByIdResponse>}
     */
    databaseGetStreetsById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getStreetsById", params);
        });
    }
    /**
     * Returns information about countries by their IDs.
     *
     * @param {{
     *   country_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetCountriesByIdResponse>}
     */
    databaseGetCountriesById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getCountriesById", params);
        });
    }
    /**
     * Returns a list of cities.
     *
     * @param {{
     *   country_id: (number),
     *   region_id: (number|undefined),
     *   q: (string|undefined),
     *   need_all: (boolean|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetCitiesResponse>}
     */
    databaseGetCities(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getCities", params);
        });
    }
    /**
     * Returns information about cities by their IDs.
     *
     * @param {{
     *   city_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetCitiesByIdResponse>}
     */
    databaseGetCitiesById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getCitiesById", params);
        });
    }
    /**
     * Returns a list of higher education institutions.
     *
     * @param {{
     *   q: (string|undefined),
     *   country_id: (number|undefined),
     *   city_id: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetUniversitiesResponse>}
     */
    databaseGetUniversities(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getUniversities", params);
        });
    }
    /**
     * Returns a list of schools.
     *
     * @param {{
     *   q: (string|undefined),
     *   city_id: (number),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetSchoolsResponse>}
     */
    databaseGetSchools(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getSchools", params);
        });
    }
    /**
     * Returns a list of school classes specified for the country.
     *
     * @param {{
     *   country_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetSchoolClassesResponse>}
     */
    databaseGetSchoolClasses(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getSchoolClasses", params);
        });
    }
    /**
     * Returns a list of faculties (i.e., university departments).
     *
     * @param {{
     *   university_id: (number),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetFacultiesResponse>}
     */
    databaseGetFaculties(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getFaculties", params);
        });
    }
    /**
     * Returns list of chairs on a specified faculty.
     *
     * @param {{
     *   faculty_id: (number),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.DatabaseGetChairsResponse>}
     */
    databaseGetChairs(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("database.getChairs", params);
        });
    }
    /**
     * Returns a list of user gifts.
     *
     * @param {{
     *   user_id: (number|undefined),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.GiftsGetResponse>}
     */
    giftsGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("gifts.get", params);
        });
    }
    /**
     * Returns a list of advertising accounts.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetAccountsResponse>}
     */
    adsGetAccounts(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getAccounts", params);
        });
    }
    /**
     * Returns a list of advertising agency's clients.
     *
     * @param {{
     *   account_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetClientsResponse>}
     */
    adsGetClients(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getClients", params);
        });
    }
    /**
     * Creates clients of an advertising agency.
     *
     * @param {{
     *   account_id: (number),
     *   data: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsCreateClientsResponse>}
     */
    adsCreateClients(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.createClients", params);
        });
    }
    /**
     * Edits clients of an advertising agency.
     *
     * @param {{
     *   account_id: (number),
     *   data: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsUpdateClientsResponse>}
     */
    adsUpdateClients(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.updateClients", params);
        });
    }
    /**
     * Archives clients of an advertising agency.
     *
     * @param {{
     *   account_id: (number),
     *   ids: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsDeleteClientsResponse>}
     */
    adsDeleteClients(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.deleteClients", params);
        });
    }
    /**
     * Returns a list of campaigns in an advertising account.
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   include_deleted: (boolean|undefined),
     *   campaign_ids: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetCampaignsResponse>}
     */
    adsGetCampaigns(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getCampaigns", params);
        });
    }
    /**
     * Creates advertising campaigns.
     *
     * @param {{
     *   account_id: (number),
     *   data: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsCreateCampaignsResponse>}
     */
    adsCreateCampaigns(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.createCampaigns", params);
        });
    }
    /**
     * Edits advertising campaigns.
     *
     * @param {{
     *   account_id: (number),
     *   data: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsUpdateCampaignsResponse>}
     */
    adsUpdateCampaigns(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.updateCampaigns", params);
        });
    }
    /**
     * Archives advertising campaigns.
     *
     * @param {{
     *   account_id: (number),
     *   ids: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsDeleteCampaignsResponse>}
     */
    adsDeleteCampaigns(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.deleteCampaigns", params);
        });
    }
    /**
     * Returns number of ads.
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   include_deleted: (boolean|undefined),
     *   campaign_ids: (string|undefined),
     *   ad_ids: (string|undefined),
     *   limit: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetAdsResponse>}
     */
    adsGetAds(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getAds", params);
        });
    }
    /**
     * Returns descriptions of ad layouts.
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   include_deleted: (boolean|undefined),
     *   campaign_ids: (string|undefined),
     *   ad_ids: (string|undefined),
     *   limit: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetAdsLayoutResponse>}
     */
    adsGetAdsLayout(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getAdsLayout", params);
        });
    }
    /**
     * Returns ad targeting parameters.
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   include_deleted: (boolean|undefined),
     *   campaign_ids: (string|undefined),
     *   ad_ids: (string|undefined),
     *   limit: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetAdsTargetingResponse>}
     */
    adsGetAdsTargeting(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getAdsTargeting", params);
        });
    }
    /**
     * Creates ads.
     *
     * @param {{
     *   account_id: (number),
     *   data: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsCreateAdsResponse>}
     */
    adsCreateAds(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.createAds", params);
        });
    }
    /**
     * Edits ads.
     *
     * @param {{
     *   account_id: (number),
     *   data: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsUpdateAdsResponse>}
     */
    adsUpdateAds(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.updateAds", params);
        });
    }
    /**
     * Archives ads.
     *
     * @param {{
     *   account_id: (number),
     *   ids: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsDeleteAdsResponse>}
     */
    adsDeleteAds(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.deleteAds", params);
        });
    }
    /**
     * Allows to check the ad link.
     *
     * @param {{
     *   account_id: (number),
     *   link_type: (string),
     *   link_url: (string),
     *   campaign_id: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsCheckLinkResponse>}
     */
    adsCheckLink(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.checkLink", params);
        });
    }
    /**
     * Returns statistics of performance indicators for ads, campaigns, clients or the whole account.
     *
     * @param {{
     *   account_id: (number),
     *   ids_type: (string),
     *   ids: (string),
     *   period: (string),
     *   date_from: (string),
     *   date_to: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetStatisticsResponse>}
     */
    adsGetStatistics(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getStatistics", params);
        });
    }
    /**
     * Returns demographics for ads or campaigns.
     *
     * @param {{
     *   account_id: (number),
     *   ids_type: (string),
     *   ids: (string),
     *   period: (string),
     *   date_from: (string),
     *   date_to: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetDemographicsResponse>}
     */
    adsGetDemographics(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getDemographics", params);
        });
    }
    /**
     * Allows to get detailed information about the ad post reach.
     *
     * @param {{
     *   account_id: (number),
     *   ads_ids: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetAdsPostsReachResponse>}
     */
    adsGetAdsPostsReach(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getAdsPostsReach", params);
        });
    }
    /**
     * Returns current budget of the advertising account.
     *
     * @param {{
     *   account_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetBudgetResponse>}
     */
    adsGetBudget(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getBudget", params);
        });
    }
    /**
     * Returns a list of managers and supervisors of advertising account.
     *
     * @param {{
     *   account_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetOfficeUsersResponse>}
     */
    adsGetOfficeUsers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getOfficeUsers", params);
        });
    }
    /**
     * Adds managers and/or supervisors to advertising account.
     *
     * @param {{
     *   account_id: (number),
     *   data: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsAddOfficeUsersResponse>}
     */
    adsAddOfficeUsers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.addOfficeUsers", params);
        });
    }
    /**
     * Removes managers and/or supervisors from advertising account.
     *
     * @param {{
     *   account_id: (number),
     *   ids: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsRemoveOfficeUsersResponse>}
     */
    adsRemoveOfficeUsers(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.removeOfficeUsers", params);
        });
    }
    /**
     * Returns the size of targeting audience, and also recommended values for CPC and CPM.
     *
     * @param {{
     *   account_id: (number),
     *   criteria: (string|undefined),
     *   ad_id: (number|undefined),
     *   ad_format: (number|undefined),
     *   ad_platform: (string|undefined),
     *   link_url: (string),
     *   link_domain: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetTargetingStatsResponse>}
     */
    adsGetTargetingStats(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getTargetingStats", params);
        });
    }
    /**
     * Returns a set of auto-suggestions for various targeting parameters.
     *
     * @param {{
     *   section: (string),
     *   ids: (string|undefined),
     *   q: (string|undefined),
     *   country: (number|undefined),
     *   cities: (string|undefined),
     *   lang: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetSuggestionsResponse>}
     */
    adsGetSuggestions(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getSuggestions", params);
        });
    }
    /**
     * Returns a list of possible ad categories.
     *
     * @param {{
     *   lang: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetCategoriesResponse>}
     */
    adsGetCategories(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getCategories", params);
        });
    }
    /**
     * Returns URL to upload an ad photo to.
     *
     * @param {{
     *   ad_format: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetUploadURLResponse>}
     */
    adsGetUploadURL(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getUploadURL", params);
        });
    }
    /**
     * Returns URL to upload an ad video to.
     *
     * @param {{
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetVideoUploadURLResponse>}
     */
    adsGetVideoUploadURL(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getVideoUploadURL", params);
        });
    }
    /**
     * Returns information about current state of a counter — number of remaining runs of methods and time to the next counter nulling in seconds.
     *
     * @param {{
     *   account_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetFloodStatsResponse>}
     */
    adsGetFloodStats(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getFloodStats", params);
        });
    }
    /**
     * Returns a reason of ad rejection for pre-moderation.
     *
     * @param {{
     *   account_id: (number),
     *   ad_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetRejectionReasonResponse>}
     */
    adsGetRejectionReason(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getRejectionReason", params);
        });
    }
    /**
     * Creates a group to re-target ads for users who visited advertiser's site (viewed information about the product, registered, etc.).
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   name: (string),
     *   domain: (string|undefined),
     *   lifetime: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsCreateTargetGroupResponse>}
     */
    adsCreateTargetGroup(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.createTargetGroup", params);
        });
    }
    /**
     * Edits a retarget group.
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   target_group_id: (number),
     *   name: (string),
     *   domain: (string|undefined),
     *   lifetime: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    adsUpdateTargetGroup(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.updateTargetGroup", params);
        });
    }
    /**
     * Deletes a retarget group.
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   target_group_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    adsDeleteTargetGroup(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.deleteTargetGroup", params);
        });
    }
    /**
     * Returns a list of target groups.
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsGetTargetGroupsResponse>}
     */
    adsGetTargetGroups(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.getTargetGroups", params);
        });
    }
    /**
     * Imports a list of advertiser's contacts to count VK registered users against the target group.
     *
     * @param {{
     *   account_id: (number),
     *   client_id: (number|undefined),
     *   target_group_id: (number),
     *   contacts: (string),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.AdsImportTargetContactsResponse>}
     */
    adsImportTargetContacts(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("ads.importTargetContacts", params);
        });
    }
    /**
     * Checks the user authentication in 'IFrame' and 'Flash' apps using the 'access_token' parameter.
     *
     * @param {{
     *   token: (string|undefined),
     *   ip: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.SecureCheckTokenResponse>}
     */
    secureCheckToken(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("secure.checkToken", params);
        });
    }
    /**
     * Returns items list for a community.
     *
     * @param {{
     *   owner_id: (number),
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketGetResponse>}
     */
    marketGet(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.get", params);
        });
    }
    /**
     * Returns information about market items by their ids.
     *
     * @param {{
     *   item_ids: (string[]),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketGetByIdResponse>}
     */
    marketGetById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.getById", params);
        });
    }
    /**
     * Searches market items in a community's catalog
     *
     * @param {{
     *   owner_id: (number),
     *   q: (string|undefined),
     *   price_from: (number|undefined),
     *   price_to: (number|undefined),
     *   tags: (number[]|undefined),
     *   rev: (number|undefined),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   extended: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketSearchResponse>}
     */
    marketSearch(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.search", params);
        });
    }
    /**
     * Returns community's collections list.
     *
     * @param {{
     *   owner_id: (number),
     *   offset: (number|undefined),
     *   count: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketGetAlbumsResponse>}
     */
    marketGetAlbums(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.getAlbums", params);
        });
    }
    /**
     * Returns items album's data
     *
     * @param {{
     *   owner_id: (number),
     *   album_ids: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketGetAlbumByIdResponse>}
     */
    marketGetAlbumById(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.getAlbumById", params);
        });
    }
    /**
     * Creates a new comment for an item.
     *
     * @param {{
     *   owner_id: (number),
     *   item_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   from_group: (boolean|undefined),
     *   reply_to_comment: (number|undefined),
     *   sticker_id: (number|undefined),
     *   guid: (string|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketCreateCommentResponse>}
     */
    marketCreateComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.createComment", params);
        });
    }
    /**
     * Returns comments list for an item.
     *
     * @param {{
     *   owner_id: (number),
     *   item_id: (number),
     *   need_likes: (boolean|undefined),
     *   start_comment_id: (number|undefined),
     *   count: (number|undefined),
     *   sort: (string|undefined),
     *   extended: (boolean|undefined),
     *   fields: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketGetCommentsResponse>}
     */
    marketGetComments(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.getComments", params);
        });
    }
    /**
     * Deletes an item's comment
     *
     * @param {{
     *   owner_id: (number),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketDeleteCommentResponse>}
     */
    marketDeleteComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.deleteComment", params);
        });
    }
    /**
     * Restores a recently deleted comment
     *
     * @param {{
     *   owner_id: (number),
     *   comment_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketRestoreCommentResponse>}
     */
    marketRestoreComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.restoreComment", params);
        });
    }
    /**
     * Chages item comment's text
     *
     * @param {{
     *   owner_id: (number),
     *   comment_id: (number),
     *   message: (string|undefined),
     *   attachments: (string[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketEditComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.editComment", params);
        });
    }
    /**
     * Sends a complaint to the item's comment.
     *
     * @param {{
     *   owner_id: (number),
     *   comment_id: (number),
     *   reason: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketReportComment(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.reportComment", params);
        });
    }
    /**
     * Returns a list of market categories.
     *
     * @param {{
     *   count: (number|undefined),
     *   offset: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketGetCategoriesResponse>}
     */
    marketGetCategories(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.getCategories", params);
        });
    }
    /**
     * Sends a complaint to the item.
     *
     * @param {{
     *   owner_id: (number),
     *   item_id: (number),
     *   reason: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketReport(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.report", params);
        });
    }
    /**
     * Ads a new item to the market.
     *
     * @param {{
     *   owner_id: (number),
     *   name: (string),
     *   description: (string),
     *   category_id: (number),
     *   price: (number),
     *   deleted: (boolean|undefined),
     *   main_photo_id: (number),
     *   photo_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketAddResponse>}
     */
    marketAdd(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.add", params);
        });
    }
    /**
     * Edits an item.
     *
     * @param {{
     *   owner_id: (number),
     *   item_id: (number),
     *   name: (string),
     *   description: (string),
     *   category_id: (number),
     *   price: (number),
     *   deleted: (boolean|undefined),
     *   main_photo_id: (number),
     *   photo_ids: (number[]|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketEdit(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.edit", params);
        });
    }
    /**
     * Deletes an item.
     *
     * @param {{
     *   owner_id: (number),
     *   item_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketDelete(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.delete", params);
        });
    }
    /**
     * Restores recently deleted item
     *
     * @param {{
     *   owner_id: (number),
     *   item_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketRestore(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.restore", params);
        });
    }
    /**
     * Changes item place in a collection.
     *
     * @param {{
     *   owner_id: (number),
     *   album_id: (number|undefined),
     *   item_id: (number),
     *   before: (number|undefined),
     *   after: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketReorderItems(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.reorderItems", params);
        });
    }
    /**
     * Reorders the collections list.
     *
     * @param {{
     *   owner_id: (number),
     *   album_id: (number),
     *   before: (number|undefined),
     *   after: (number|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketReorderAlbums(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.reorderAlbums", params);
        });
    }
    /**
     * Creates new collection of items
     *
     * @param {{
     *   owner_id: (number),
     *   title: (string),
     *   photo_id: (number|undefined),
     *   main_album: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.MarketAddAlbumResponse>}
     */
    marketAddAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.addAlbum", params);
        });
    }
    /**
     * Edits a collection of items
     *
     * @param {{
     *   owner_id: (number),
     *   album_id: (number),
     *   title: (string),
     *   photo_id: (number|undefined),
     *   main_album: (boolean|undefined),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketEditAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.editAlbum", params);
        });
    }
    /**
     * Deletes a collection of items.
     *
     * @param {{
     *   owner_id: (number),
     *   album_id: (number),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketDeleteAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.deleteAlbum", params);
        });
    }
    /**
     * Removes an item from one or multiple collections.
     *
     * @param {{
     *   owner_id: (number),
     *   item_id: (number),
     *   album_ids: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketRemoveFromAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.removeFromAlbum", params);
        });
    }
    /**
     * Adds an item to one or multiple collections.
     *
     * @param {{
     *   owner_id: (number),
     *   item_id: (number),
     *   album_ids: (number[]),
     *   access_token: (string|undefined)
     * }} params
     *
     * @returns {Promise<Responses.OkResponse>}
     */
    marketAddToAlbum(params) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.call("market.addToAlbum", params);
        });
    }
}
exports.VKApi = VKApi;
//# sourceMappingURL=VKApi.js.map