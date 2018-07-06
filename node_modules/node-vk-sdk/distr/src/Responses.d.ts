import * as Models from './Models';
export declare type OkResponse = Models.BaseOkResponse;
export interface AccountChangePasswordResponse {
    /**
     * New token
     */
    token: string;
    /**
     * New secret
     */
    secret: string;
}
export interface AccountGetActiveOffersResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.AccountOffer[];
}
export declare type AccountGetAppPermissionsResponse = number;
export interface AccountGetBannedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserMin[];
}
export declare type AccountGetCountersResponse = Models.AccountAccountCounters;
export declare type AccountGetInfoResponse = Models.AccountInfo;
export declare type AccountGetProfileInfoResponse = Models.AccountUserSettings;
export declare type AccountGetPushSettingsResponse = Models.AccountPushSettings;
export declare type AccountLookupContactsResponse = Models.AccountLookupResult;
export interface AccountSaveProfileInfoResponse {
    /**
     * 1 if changes has been processed
     */
    changed: Models.BaseBoolInt;
    /**
     *
     */
    name_request: Models.AccountNameRequest;
}
export declare type AdsAddOfficeUsersResponse = boolean;
export declare type AdsCheckLinkResponse = Models.AdsLinkStatus;
export declare type AdsCreateAdsResponse = number[];
export declare type AdsCreateCampaignsResponse = number[];
export declare type AdsCreateClientsResponse = number[];
export interface AdsCreateTargetGroupResponse {
    /**
     * Group ID
     */
    id: number;
    /**
     * Pixel code
     */
    pixel: string;
}
export declare type AdsDeleteAdsResponse = number[];
export declare type AdsDeleteCampaignsResponse = number;
export declare type AdsDeleteClientsResponse = number;
export declare type AdsGetAccountsResponse = Models.AdsAccount[];
export declare type AdsGetAdsResponse = Models.AdsAd[];
export declare type AdsGetAdsLayoutResponse = Models.AdsAdLayout[];
export declare type AdsGetAdsPostsReachResponse = Models.AdsPostStats[];
export declare type AdsGetAdsTargetingResponse = Models.AdsTargSettings[];
export declare type AdsGetBudgetResponse = number;
export declare type AdsGetCampaignsResponse = Models.AdsCampaign[];
export interface AdsGetCategoriesResponse {
    /**
     * Old categories
     */
    v1: Models.AdsCategory[];
    /**
     * Actual categories
     */
    v2: Models.AdsCategory[];
}
export declare type AdsGetClientsResponse = Models.AdsClient[];
export declare type AdsGetDemographicsResponse = Models.AdsDemoStats[];
export declare type AdsGetFloodStatsResponse = Models.AdsFloodStats;
export declare type AdsGetOfficeUsersResponse = Models.AdsUsers[];
export declare type AdsGetRejectionReasonResponse = Models.AdsRejectReason;
export declare type AdsGetStatisticsResponse = Models.AdsStats[];
export declare type AdsGetSuggestionsResponse = Models.AdsTargSuggestions[];
export declare type AdsGetSuggestionsRegionsResponse = Models.AdsTargSuggestionsRegions[];
export declare type AdsGetSuggestionsCitiesResponse = Models.AdsTargSuggestionsCities[];
export declare type AdsGetSuggestionsSchoolsResponse = Models.AdsTargSuggestionsSchools[];
export declare type AdsGetTargetGroupsResponse = Models.AdsTargetGroup[];
export declare type AdsGetTargetingStatsResponse = Models.AdsTargStats;
export declare type AdsGetUploadURLResponse = string;
export declare type AdsGetVideoUploadURLResponse = string;
export declare type AdsImportTargetContactsResponse = number;
export declare type AdsRemoveOfficeUsersResponse = boolean;
export declare type AdsUpdateAdsResponse = number[];
export declare type AdsUpdateCampaignsResponse = number;
export declare type AdsUpdateClientsResponse = number;
export interface AppsGetCatalogResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.AppsApp[];
}
export interface AppsGetLeaderboardResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.AppsLeaderboard[];
}
export interface AppsGetLeaderboardExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.AppsLeaderboard[];
    /**
     *
     */
    profiles: Models.UsersUserMin[];
}
export interface AppsGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.AppsApp[];
}
export declare type AppsGetScoreResponse = number;
export declare type AppsSendRequestResponse = number;
export interface AppsGetFriendsListResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export interface AuthSignupResponse {
    /**
     * Parameter to retry
     */
    sid: string;
}
export interface AuthConfirmResponse {
    /**
     * 1 if success
     */
    success: number;
    /**
     * User ID
     */
    user_id: number;
}
export interface AuthRestoreResponse {
    /**
     * 1 if success
     */
    success: number;
    /**
     * Parameter needed to grant access by code
     */
    sid: string;
}
export interface BoardGetTopicsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.BoardTopic[];
    /**
     *
     */
    default_order: Models.BoardDefaultOrder;
    /**
     * Information whether current user can add topic
     */
    can_add_topics: Models.BaseBoolInt;
}
export interface BoardGetTopicsExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.BoardTopic[];
    /**
     *
     */
    default_order: Models.BoardDefaultOrder;
    /**
     * Information whether current user can add topic
     */
    can_add_topics: Models.BaseBoolInt;
    /**
     *
     */
    profiles: Models.UsersUserMin[];
}
export interface BoardGetCommentsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.BoardTopicComment[];
    /**
     *
     */
    poll: Models.BoardTopicPoll;
}
export interface BoardGetCommentsExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.BoardTopicComment[];
    /**
     *
     */
    poll: Models.BoardTopicPoll;
    /**
     *
     */
    profiles: Models.UsersUser[];
    /**
     *
     */
    groups: Models.GroupsGroup[];
}
export declare type BoardAddTopicResponse = number;
export declare type BoardCreateCommentResponse = number;
export interface DatabaseGetCountriesResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.BaseCountry[];
}
export interface DatabaseGetRegionsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.DatabaseRegion[];
}
export declare type DatabaseGetStreetsByIdResponse = Models.DatabaseStreet[];
export declare type DatabaseGetCountriesByIdResponse = Models.BaseCountry[];
export interface DatabaseGetChairsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.BaseObject[];
}
export interface DatabaseGetCitiesResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.DatabaseCity[];
}
export declare type DatabaseGetCitiesByIdResponse = Models.BaseObject[];
export interface DatabaseGetUniversitiesResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.DatabaseUniversity[];
}
export interface DatabaseGetSchoolsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.DatabaseSchool[];
}
export declare type DatabaseGetSchoolClassesResponse = any[][];
export interface DatabaseGetFacultiesResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.DatabaseFaculty[];
}
export interface DocsGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.DocsDoc[];
}
export declare type DocsGetByIdResponse = Models.DocsDoc[];
export interface DocsGetUploadServerResponse {
    /**
     * Upload URL
     */
    upload_url: string;
}
export interface DocsGetWallUploadServerResponse {
    /**
     * Upload URL
     */
    upload_url: string;
}
export declare type DocsSaveResponse = Models.DocsDoc[];
export interface DocsAddResponse {
    /**
     * Doc ID
     */
    id: number;
}
export interface DocsGetTypesResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.DocsDocTypes[];
}
export interface DocsSearchResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.DocsDoc[];
}
export interface FaveGetUsersResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserMin[];
}
export interface FaveGetPhotosResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhoto[];
}
export interface FaveGetPostsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallpostFull[];
}
export interface FaveGetVideosResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideo[];
}
export interface FaveGetLinksResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.FaveFavesLink[];
}
export interface FaveGetMarketItemsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketItem[];
}
export interface FriendsGetResponse {
    /**
     * Total friends number
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface FriendsGetFieldsResponse {
    /**
     * Total friends number
     */
    count: number;
    /**
     *
     */
    items: Models.FriendsUserXtrLists[];
}
export declare type FriendsGetOnlineResponse = number[];
export interface FriendsGetOnlineOnlineMobileResponse {
    /**
     *
     */
    online: number[];
    /**
     *
     */
    online_mobile: number[];
}
export declare type FriendsGetMutualResponse = number[];
export declare type FriendsGetMutualTargetUidsResponse = Models.FriendsMutualFriend[];
export declare type FriendsGetRecentResponse = number[];
export interface FriendsGetRequestsResponse {
    /**
     * Total requests number
     */
    count: number;
    /**
     *
     */
    items: number[];
    /**
     * Total unread requests number
     */
    count_unread: number;
}
export interface FriendsGetRequestsNeedMutualResponse {
    /**
     * Total requests number
     */
    count: number;
    /**
     *
     */
    items: Models.FriendsRequests[];
}
export interface FriendsGetRequestsExtendedResponse {
    /**
     * Total requests number
     */
    count: number;
    /**
     *
     */
    items: Models.FriendsRequestsXtrMessage[];
}
export declare type FriendsAddResponse = number;
export interface FriendsDeleteResponse {
    /**
     *
     */
    success: Models.BaseOkResponse;
    /**
     * Returns 1 if friend has been deleted
     */
    friend_deleted: number;
    /**
     * Returns 1 if out request has been canceled
     */
    out_request_deleted: number;
    /**
     * Returns 1 if incoming request has been declined
     */
    in_request_deleted: number;
    /**
     * Returns 1 if suggestion has been declined
     */
    suggestion_deleted: number;
}
export interface FriendsGetListsResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: Models.FriendsFriendsList[];
}
export interface FriendsAddListResponse {
    /**
     * List ID
     */
    list_id: number;
}
export declare type FriendsGetAppUsersResponse = number[];
export declare type FriendsGetByPhonesResponse = Models.FriendsUserXtrPhone[];
export interface FriendsGetSuggestionsResponse {
    /**
     * Total results number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export declare type FriendsAreFriendsResponse = Models.FriendsFriendStatus[];
export interface FriendsGetAvailableForCallResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface FriendsGetAvailableForCallFieldsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export interface FriendsSearchResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export interface GiftsGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.GiftsGift[];
}
export declare type GroupsIsMemberResponse = Models.BaseBoolInt;
export declare type GroupsIsMemberUserIdsResponse = Models.GroupsMemberStatus[];
export interface GroupsIsMemberExtendedResponse {
    /**
     * Information whether user is a member of the group
     */
    member: Models.BaseBoolInt;
    /**
     * Information whether user has been invited to the group
     */
    invitation: Models.BaseBoolInt;
    /**
     * Information whether user has sent request to the group
     */
    request: Models.BaseBoolInt;
}
export declare type GroupsIsMemberUserIdsExtendedResponse = Models.GroupsMemberStatusFull[];
export declare type GroupsGetByIdResponse = Models.GroupsGroupFull[];
export interface GroupsGetCallbackConfirmationCodeResponse {
    /**
     * Confirmation code
     */
    code: string;
}
export interface GroupsGetCallbackSettingsResponse {
    /**
     * Whether notifications about new message enabled
     */
    message_new: Models.BaseBoolInt;
    /**
     * Whether notifications about edited message enabled
     */
    message_edit: Models.BaseBoolInt;
    /**
     * Whether notifications about new message enabled
     */
    message_reply: Models.BaseBoolInt;
    /**
     * Whether notifications about allowed messages enabled
     */
    message_allow: Models.BaseBoolInt;
    /**
     * Whether notifications about denied messages enabled
     */
    message_deny: Models.BaseBoolInt;
    /**
     * Whether notifications about new photos enabled
     */
    photo_new: Models.BaseBoolInt;
    /**
     * Whether notifications about new audios enabled
     */
    audio_new: Models.BaseBoolInt;
    /**
     * Whether notifications about new videos enabled
     */
    video_new: Models.BaseBoolInt;
    /**
     * Whether notifications about new wall replies enabled
     */
    wall_reply_new: Models.BaseBoolInt;
    /**
     * Comment on wall has been edited
     */
    wall_reply_edit: Models.BaseBoolInt;
    /**
     * Comment on wall has been deleted
     */
    wall_reply_delete: Models.BaseBoolInt;
    /**
     * Comment on wall has been restored
     */
    wall_post_restore: Models.BaseBoolInt;
    /**
     * Whether notifications about new board posts enabled
     */
    board_post_new: Models.BaseBoolInt;
    /**
     * Whether notifications about board posts edits enabled
     */
    board_post_edit: Models.BaseBoolInt;
    /**
     * Whether notifications about board posts restores enabled
     */
    board_post_restore: Models.BaseBoolInt;
    /**
     * Whether notifications about board posts deleted enabled
     */
    board_post_delete: Models.BaseBoolInt;
    /**
     * Whether notifications about new photo comments enabled
     */
    photo_comment_new: Models.BaseBoolInt;
    /**
     * Comment on photo has been edited
     */
    photo_comment_edit: Models.BaseBoolInt;
    /**
     * Comment on photo has been deleted
     */
    photo_comment_delete: Models.BaseBoolInt;
    /**
     * Comment on photo has been restored
     */
    photo_comment_restore: Models.BaseBoolInt;
    /**
     * Whether notifications about new video comments enabled
     */
    video_comment_new: Models.BaseBoolInt;
    /**
     * Comment on video has been edited
     */
    video_comment_edit: Models.BaseBoolInt;
    /**
     * Comment on video has been deleted
     */
    video_comment_delete: Models.BaseBoolInt;
    /**
     * Comment on video has been restored
     */
    video_comment_restore: Models.BaseBoolInt;
    /**
     * Whether notifications about new market comments enabled
     */
    market_comment_new: Models.BaseBoolInt;
    /**
     * Comment on market item has been edited
     */
    market_comment_edit: Models.BaseBoolInt;
    /**
     * Comment on market item has been deleted
     */
    market_comment_delete: Models.BaseBoolInt;
    /**
     * Comment on market item has been restored
     */
    market_comment_restore: Models.BaseBoolInt;
    /**
     * Whether notifications about anyone joined the community enabled
     */
    group_join: Models.BaseBoolInt;
    /**
     * Whether notifications about anyone left the community enabled
     */
    group_leave: Models.BaseBoolInt;
}
export declare type GroupsGetLongPollServerResponse = Models.GroupsLongPollServer;
export declare type GroupsGetLongPollSettingsResponse = Models.GroupsLongPollSettings;
export interface GroupsGetResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface GroupsGetExtendedResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: Models.GroupsGroupFull[];
}
export interface GroupsGetMembersResponse {
    /**
     * Total members number
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface GroupsGetMembersFieldsResponse {
    /**
     * Total members number
     */
    count: number;
    /**
     *
     */
    items: Models.GroupsUserXtrRole[];
}
export interface GroupsGetMembersFilterResponse {
    /**
     * Total members number
     */
    count: number;
    /**
     *
     */
    items: Models.GroupsMemberRole[];
}
export interface GroupsSearchResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: Models.GroupsGroup[];
}
export interface GroupsGetCatalogResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: Models.GroupsGroup[];
}
export interface GroupsGetCatalogInfoResponse {
    /**
     * Information whether catalog is enabled for current user
     */
    enabled: number;
    /**
     *
     */
    categories: Models.GroupsGroupCategory[];
}
export interface GroupsGetCatalogInfoExtendedResponse {
    /**
     * Information whether catalog is enabled for current user
     */
    enabled: number;
    /**
     *
     */
    categories: Models.GroupsGroupCategoryFull[];
}
export interface GroupsGetInvitesResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: Models.GroupsGroupXtrInvitedBy[];
}
export interface GroupsGetInvitesExtendedResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: Models.GroupsGroupXtrInvitedBy[];
    /**
     *
     */
    profiles: Models.UsersUserMin[];
}
export interface GroupsGetInvitedUsersResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export interface GroupsGetBannedResponse {
    /**
     * Total users number
     */
    count: number;
    /**
     *
     */
    items: Models.GroupsOwnerXtrBanInfo[];
}
export declare type GroupsCreateResponse = Models.GroupsGroup;
export interface GroupsEditPlaceResponse {
    /**
     *
     */
    success: Models.BaseOkResponse;
    /**
     * Place address
     */
    address: string;
}
export declare type GroupsGetSettingsResponse = Models.GroupsGroupSettings;
export interface GroupsGetRequestsResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface GroupsGetRequestsFieldsResponse {
    /**
     * Total communities number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export declare type GroupsAddLinkResponse = Models.GroupsGroupLink;
export declare type LeadsCompleteResponse = Models.LeadsComplete;
export declare type LeadsStartResponse = Models.LeadsStart;
export declare type LeadsGetStatsResponse = Models.LeadsLead;
export declare type LeadsGetUsersResponse = Models.LeadsEntry[];
export declare type LeadsCheckUserResponse = Models.LeadsChecked;
export interface LeadsMetricHitResponse {
    /**
     * Information whether request has been processed successfully
     */
    result: boolean;
    /**
     * Redirect link
     */
    redirect_link: string;
}
export interface LikesGetListResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface LikesGetListExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserMin[];
}
export interface LikesAddResponse {
    /**
     * Total likes number
     */
    likes: number;
}
export interface LikesDeleteResponse {
    /**
     * Total likes number
     */
    likes: number;
}
export interface LikesIsLikedResponse {
    /**
     * Information whether user liked the object
     */
    liked: Models.BaseBoolInt;
    /**
     * Information whether user reposted the object
     */
    copied: Models.BaseBoolInt;
}
export interface MarketGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketItem[];
}
export interface MarketGetExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketItemFull[];
}
export interface MarketGetByIdResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketItem[];
}
export interface MarketGetByIdExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketItemFull[];
}
export interface MarketSearchResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketItem[];
}
export interface MarketSearchExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketItemFull[];
}
export interface MarketGetAlbumsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketAlbum[];
}
export interface MarketGetAlbumByIdResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketAlbum[];
}
export declare type MarketCreateCommentResponse = number;
export interface MarketGetCommentsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallComment[];
}
export declare type MarketDeleteCommentResponse = Models.BaseBoolInt;
export declare type MarketRestoreCommentResponse = Models.BaseBoolInt;
export interface MarketGetCategoriesResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MarketMarketCategory[];
}
export interface MarketAddResponse {
    /**
     * Item ID
     */
    market_item_id: number;
}
export interface MarketAddAlbumResponse {
    /**
     * Album ID
     */
    market_album_id: number;
}
export interface MessagesGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MessagesMessage[];
}
export interface MessagesDeleteResponse {
}
export interface MessagesGetDialogsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     * Unread dialogs number
     */
    unread_dialogs: number;
    /**
     *
     */
    items: Models.MessagesDialog[];
}
export interface MessagesGetByIdResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MessagesMessage[];
}
export interface MessagesSearchResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.MessagesMessage[];
}
export interface MessagesGetHistoryResponse {
    /**
     * Total number
     */
    count: number;
    /**
     * Unread dialogs number
     */
    unread: number;
    /**
     *
     */
    items: Models.MessagesMessage[];
    /**
     * Id of last read inbound message
     */
    in_read: number;
    /**
     * Id of last read outbound message
     */
    out_read: number;
}
export interface MessagesGetHistoryAttachmentsResponse {
    /**
     *
     */
    items: Models.MessagesHistoryAttachment[];
    /**
     * Value for pagination
     */
    next_from: string;
}
export declare type MessagesSendResponse = number;
export declare type MessagesMarkAsImportantResponse = number[];
export declare type MessagesGetLongPollServerResponse = Models.MessagesLongpollParams;
export interface MessagesGetLongPollHistoryResponse {
    /**
     *
     */
    history: number[][];
    /**
     *
     */
    groups: Models.GroupsGroup[];
    /**
     *
     */
    messages: Models.MessagesLongpollMessages;
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    chats: Models.MessagesChat[];
    /**
     * Persistence timestamp
     */
    new_pts: number;
    /**
     * Has more
     */
    more: Models.BaseBoolInt;
}
export declare type MessagesGetChatResponse = Models.MessagesChat;
export declare type MessagesGetChatFieldsResponse = Models.MessagesChatFull;
export declare type MessagesGetChatChatIdsResponse = Models.MessagesChat[];
export declare type MessagesGetChatChatIdsFieldsResponse = Models.MessagesChatFull[];
export declare type MessagesCreateChatResponse = number;
export declare type MessagesEditChatResponse = Models.BaseOkResponse;
export declare type MessagesGetChatUsersResponse = number[];
export declare type MessagesGetChatUsersFieldsResponse = Models.MessagesUserXtrInvitedBy[];
export interface MessagesGetChatUsersChatIdsResponse {
}
export interface MessagesGetChatUsersChatIdsFieldsResponse {
}
export declare type MessagesSearchDialogsResponse = any[];
export declare type MessagesGetLastActivityResponse = Models.MessagesLastActivity;
export interface MessagesSetChatPhotoResponse {
    /**
     * Service message ID
     */
    message_id: number;
    /**
     *
     */
    chat: Models.MessagesChat;
}
export interface MessagesDeleteChatPhotoResponse {
    /**
     * Service message ID
     */
    message_id: number;
    /**
     *
     */
    chat: Models.MessagesChat;
}
export interface MessagesIsMessagesFromGroupAllowedResponse {
    /**
     *
     */
    is_allowed: Models.BaseBoolInt;
}
export interface NewsfeedGetResponse {
    /**
     *
     */
    items: Models.NewsfeedNewsfeedItem[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface NewsfeedGetRecommendedResponse {
    /**
     *
     */
    items: Models.NewsfeedNewsfeedItem[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
    /**
     * New offset value
     */
    new_offset: string;
    /**
     * New from value
     */
    new_from: string;
}
export interface NewsfeedGetCommentsResponse {
    /**
     *
     */
    items: Models.NewsfeedNewsfeedItem[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
    /**
     * New from value
     */
    next_from: string;
}
export interface NewsfeedGetMentionsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallpostToId[];
}
export interface NewsfeedGetBannedResponse {
    /**
     *
     */
    groups: number[];
    /**
     *
     */
    members: number[];
}
export interface NewsfeedGetBannedExtendedResponse {
    /**
     *
     */
    groups: Models.UsersUserFull[];
    /**
     *
     */
    members: Models.GroupsGroupFull[];
}
export interface NewsfeedSearchResponse {
    /**
     *
     */
    items: Models.WallWallpostFull[];
    /**
     *
     */
    suggested_queries: string[];
}
export interface NewsfeedSearchExtendedResponse {
    /**
     *
     */
    items: Models.WallWallpostFull[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface NewsfeedGetListsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.NewsfeedList[];
}
export interface NewsfeedGetListsExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.NewsfeedListFull[];
}
export declare type NewsfeedSaveListResponse = number;
export interface NewsfeedGetSuggestedSourcesResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: any[];
}
export interface NotesGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.NotesNote[];
}
export declare type NotesGetByIdResponse = Models.NotesNote;
export declare type NotesAddResponse = number;
export interface NotesGetCommentsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.NotesNoteComment[];
}
export declare type NotesCreateCommentResponse = number;
export interface NotificationsGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.NotificationsNotification[];
    /**
     *
     */
    profiles: Models.UsersUser[];
    /**
     *
     */
    groups: Models.GroupsGroup[];
    /**
     * Time when user has been checked notifications last time
     */
    last_viewed: number;
}
export declare type NotificationsMarkAsViewedResponse = Models.BaseBoolInt;
export declare type OrdersGetResponse = Models.OrdersOrder[];
export declare type OrdersGetByIdResponse = Models.OrdersOrder[];
export declare type OrdersChangeStateResponse = string;
export declare type OrdersGetAmountResponse = Models.OrdersAmount;
export declare type PagesGetResponse = Models.PagesWikipageFull;
export declare type PagesSaveResponse = number;
export declare type PagesSaveAccessResponse = number;
export declare type PagesGetHistoryResponse = Models.PagesWikipageVersion[];
export declare type PagesGetTitlesResponse = Models.PagesWikipage[];
export declare type PagesGetVersionResponse = Models.PagesWikipageFull;
export declare type PagesParseWikiResponse = string;
export declare type PhotosCreateAlbumResponse = Models.PhotosPhotoAlbumFull;
export interface PhotosGetAlbumsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhotoAlbumFull[];
}
export interface PhotosGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhoto[];
}
export interface PhotosGetExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhotoFull[];
}
export declare type PhotosGetAlbumsCountResponse = number;
export declare type PhotosGetByIdResponse = Models.PhotosPhoto[];
export declare type PhotosGetByIdExtendedResponse = Models.PhotosPhotoFull[];
export declare type PhotosGetUploadServerResponse = Models.PhotosPhotoUpload;
export interface PhotosGetOwnerCoverPhotoUploadServerResponse {
    /**
     * URL to upload the photo
     */
    upload_url: string;
}
export declare type PhotosSaveOwnerCoverPhotoResponse = Models.BaseImage[];
export interface PhotosGetOwnerPhotoUploadServerResponse {
    /**
     * URL to upload the photo
     */
    upload_url: string;
}
export interface PhotosGetChatUploadServerResponse {
    /**
     * URL to upload the photo
     */
    upload_url: string;
}
export interface PhotosGetMarketUploadServerResponse {
    /**
     * URL to upload the photo
     */
    upload_url: string;
}
export interface PhotosGetMarketAlbumUploadServerResponse {
    /**
     * URL to upload the photo
     */
    upload_url: string;
}
export declare type PhotosSaveMarketPhotoResponse = Models.PhotosPhoto[];
export declare type PhotosSaveMarketAlbumPhotoResponse = Models.PhotosPhoto[];
export interface PhotosSaveOwnerPhotoResponse {
    /**
     * Parameter for saveProfilePhoto method
     */
    photo_hash: string;
    /**
     * Uploaded image url
     */
    photo_src: string;
}
export declare type PhotosSaveWallPhotoResponse = Models.PhotosPhoto[];
export declare type PhotosGetWallUploadServerResponse = Models.PhotosPhotoUpload;
export declare type PhotosGetMessagesUploadServerResponse = Models.PhotosPhotoUpload;
export declare type PhotosSaveMessagesPhotoResponse = Models.PhotosPhoto[];
export interface PhotosSearchResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhoto[];
}
export declare type PhotosSaveResponse = Models.PhotosPhoto[];
export declare type PhotosCopyResponse = number;
export interface PhotosGetAllResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhotoXtrRealOffset[];
    /**
     * Information whether next page is presented
     */
    more: Models.BaseBoolInt;
}
export interface PhotosGetAllExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhotoFullXtrRealOffset[];
    /**
     * Information whether next page is presented
     */
    more: Models.BaseBoolInt;
}
export interface PhotosGetUserPhotosResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhoto[];
}
export interface PhotosGetUserPhotoExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhotoFull[];
}
export interface PhotosGetCommentsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     * Real offset of the comments
     */
    real_offset: number;
    /**
     *
     */
    items: Models.WallWallComment[];
}
export interface PhotosGetCommentsExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     * Real offset of the comments
     */
    real_offset: number;
    /**
     *
     */
    items: Models.WallWallComment[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface PhotosGetAllCommentsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosCommentXtrPid[];
}
export declare type PhotosCreateCommentResponse = number;
export declare type PhotosDeleteCommentResponse = Models.BaseBoolInt;
export declare type PhotosRestoreCommentResponse = Models.BaseBoolInt;
export declare type PhotosGetTagsResponse = Models.PhotosPhotoTag[];
export declare type PhotosPutTagResponse = number;
export interface PhotosGetNewTagsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PhotosPhotoXtrTagInfo[];
}
export interface PlacesAddResponse {
    /**
     * Place ID
     */
    id: number;
}
export declare type PlacesGetByIdResponse = Models.PlacesPlaceMin[];
export interface PlacesSearchResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PlacesPlaceFull[];
}
export interface PlacesCheckinResponse {
    /**
     * Checkin ID
     */
    id: number;
}
export interface PlacesGetCheckinsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.PlacesCheckin[];
}
export declare type PlacesGetTypesResponse = Models.PlacesTypes[];
export declare type PollsGetByIdResponse = Models.PollsPoll;
export declare type PollsAddVoteResponse = Models.BaseBoolInt;
export declare type PollsDeleteVoteResponse = Models.BaseBoolInt;
export declare type PollsGetVotersResponse = Models.PollsVoters[];
export declare type PollsCreateResponse = Models.PollsPoll;
export interface SearchGetHintsResponse {
    /**
     *
     */
    items: Models.SearchHint[];
    /**
     *
     */
    suggested_queries: string[];
}
export declare type SecureGetAppBalanceResponse = number;
export declare type SecureGetSMSHistoryResponse = Models.SecureSmsNotification[];
export declare type SecureGetTransactionsHistoryResponse = Models.SecureTransaction[];
export declare type SecureGetUserLevelResponse = Models.SecureLevel[];
export declare type SecureSendNotificationResponse = number[];
export declare type SecureCheckTokenResponse = Models.SecureTokenChecked;
export interface StreamingGetServerUrlResponse {
    /**
     * Server host
     */
    endpoint: string;
    /**
     * Access key
     */
    key: string;
}
export declare type StatsGetResponse = Models.StatsPeriod[];
export declare type StatsGetPostReachResponse = Models.StatsWallpostStat[];
export declare type StatusGetResponse = Models.StatusStatus;
export declare type StorageGetResponse = string;
export declare type StorageGetKeysResponse = string[];
export declare type UsersGetResponse = Models.UsersUserXtrCounters[];
export interface UsersSearchResponse {
    /**
     * Total number of available results
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export declare type UsersIsAppUserResponse = Models.BaseBoolInt;
export interface UsersGetSubscriptionsResponse {
    /**
     *
     */
    users: Models.UsersUsersArray;
    /**
     *
     */
    groups: Models.GroupsGroupsArray;
}
export interface UsersGetSubscriptionsExtendedResponse {
    /**
     * Total number of available results
     */
    count: number;
    /**
     *
     */
    items: any[];
}
export interface UsersGetFollowersResponse {
    /**
     * Total friends number
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface UsersGetFollowersFieldsResponse {
    /**
     * Total number of available results
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export interface UsersGetNearbyResponse {
    /**
     * Users number
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
export declare type UtilsGetShortLinkResponse = Models.UtilsShortLink;
export interface UtilsGetLastShortenedLinksResponse {
    /**
     * Total number of available results
     */
    count: number;
    /**
     *
     */
    items: Models.UtilsLastShortenedLink[];
}
export declare type UtilsGetLinkStatsResponse = Models.UtilsLinkStats;
export declare type UtilsGetLinkStatsExtendedResponse = Models.UtilsLinkStatsExtended;
export declare type UtilsCheckLinkResponse = Models.UtilsLinkChecked;
export declare type UtilsResolveScreenNameResponse = Models.UtilsDomainResolved;
export declare type UtilsGetServerTimeResponse = number;
export interface VideoGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideo[];
}
export interface VideoGetExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideoFull[];
    /**
     *
     */
    profiles: Models.UsersUserMin[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export declare type VideoSaveResponse = Models.VideoSaveResult;
export interface VideoSearchResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideo[];
}
export interface VideoSearchExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideo[];
    /**
     *
     */
    profiles: Models.UsersUserMin[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface VideoGetUserVideosResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideo[];
}
export interface VideoGetUserVideosExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideo[];
    /**
     *
     */
    profiles: Models.UsersUserMin[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface VideoGetAlbumsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideoAlbumFull[];
}
export interface VideoGetAlbumsExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideoAlbumFull[];
}
export declare type VideoGetAlbumByIdResponse = Models.VideoVideoAlbumFull;
export interface VideoAddAlbumResponse {
    /**
     * Created album ID
     */
    album_id: number;
}
export declare type VideoGetAlbumsByVideoResponse = number[];
export interface VideoGetAlbumsByVideoExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideoAlbumFull[];
}
export interface VideoGetCommentsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallComment[];
    /**
     *
     */
    profiles: Models.UsersUserMin[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface VideoGetCommentsExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallComment[];
}
export declare type VideoCreateCommentResponse = number;
export declare type VideoRestoreCommentResponse = Models.BaseBoolInt;
export declare type VideoGetTagsResponse = Models.VideoVideoTag[];
export declare type VideoPutTagResponse = number;
export interface VideoGetNewTagsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.VideoVideoTagInfo[];
}
export interface VideoGetCatalogResponse {
    /**
     *
     */
    items: Models.VideoCatBlock[];
    /**
     * New value for _from_ parameter
     */
    next: string;
}
export interface VideoGetCatalogExtendedResponse {
    /**
     *
     */
    items: Models.VideoCatBlock[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
    /**
     * New value for _from_ parameter
     */
    next: string;
}
export interface VideoGetCatalogSectionResponse {
    /**
     *
     */
    items: Models.VideoCatElement[];
    /**
     * New value for _from_ parameter
     */
    next: string;
}
export interface VideoGetCatalogSectionExtendedResponse {
    /**
     *
     */
    items: Models.VideoCatElement[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
    /**
     * New value for _from_ parameter
     */
    next: string;
}
export interface WallGetResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallpostFull[];
}
export interface WallGetExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallpostFull[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface WallSearchResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallpostFull[];
}
export interface WallSearchExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallpostFull[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export declare type WallGetByIdResponse = Models.WallWallpostFull[];
export interface WallGetByIdExtendedResponse {
    /**
     *
     */
    items: Models.WallWallpostFull[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface WallPostResponse {
    /**
     * Created post ID
     */
    post_id: number;
}
export interface WallRepostResponse {
    /**
     *
     */
    success: Models.BaseOkResponse;
    /**
     * Created post ID
     */
    post_id: number;
    /**
     * Reposts number
     */
    reposts_count: number;
    /**
     * Reposts number
     */
    likes_count: number;
}
export interface WallGetRepostsResponse {
    /**
     *
     */
    items: Models.WallWallpostFull[];
    /**
     *
     */
    profiles: Models.UsersUser[];
    /**
     *
     */
    groups: Models.GroupsGroup[];
}
export interface WallGetCommentsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallComment[];
}
export interface WallGetCommentsExtendedResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    items: Models.WallWallComment[];
    /**
     *
     */
    profiles: Models.UsersUser[];
    /**
     *
     */
    groups: Models.GroupsGroup[];
}
export interface WallCreateCommentResponse {
    /**
     * Created comment ID
     */
    comment_id: number;
}
export interface WidgetsGetCommentsResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    posts: Models.WidgetsWidgetComment[];
}
export interface WidgetsGetPagesResponse {
    /**
     * Total number
     */
    count: number;
    /**
     *
     */
    pages: Models.WidgetsWidgetPage[];
}
export interface StoriesGetResponse {
    /**
     * Stories count
     */
    count: number;
    /**
     *
     */
    items: Models.StoriesStory[][];
}
export interface StoriesGetExtendedResponse {
    /**
     * Stories count
     */
    count: number;
    /**
     *
     */
    items: Models.StoriesStory[][];
    /**
     *
     */
    profiles: Models.UsersUser[];
    /**
     *
     */
    groups: Models.GroupsGroup[];
}
export interface StoriesGetBannedResponse {
    /**
     * Stories count
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface StoriesGetBannedExtendedResponse {
    /**
     * Stories count
     */
    count: number;
    /**
     *
     */
    items: number[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface StoriesGetByIdResponse {
    /**
     * Stories count
     */
    count: number;
    /**
     *
     */
    items: Models.StoriesStory[];
}
export interface StoriesGetByIdExtendedResponse {
    /**
     * Stories count
     */
    count: number;
    /**
     *
     */
    items: Models.StoriesStory[];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export interface StoriesGetPhotoUploadServerResponse {
    /**
     * Upload URL
     */
    upload_url: string;
    /**
     * Users ID who can to see story.
     */
    user_ids: number[];
}
export interface StoriesGetRepliesResponse {
    /**
     * Stories count
     */
    count: number;
    /**
     *
     */
    items: Models.StoriesStory[][];
}
export interface StoriesGetRepliesExtendedResponse {
    /**
     * Stories count
     */
    count: number;
    /**
     *
     */
    items: Models.StoriesStory[][];
    /**
     *
     */
    profiles: Models.UsersUserFull[];
    /**
     *
     */
    groups: Models.GroupsGroupFull[];
}
export declare type StoriesGetStatsResponse = Models.StoriesStoryStats;
export interface StoriesGetVideoUploadServerResponse {
    /**
     * Upload URL
     */
    upload_url: string;
    /**
     * Users ID who can to see story.
     */
    user_ids: number[];
}
export interface StoriesGetViewersResponse {
    /**
     * Viewers count
     */
    count: number;
    /**
     *
     */
    items: number[];
}
export interface StoriesGetViewersExtendedResponse {
    /**
     * Viewers count
     */
    count: number;
    /**
     *
     */
    items: Models.UsersUserFull[];
}
