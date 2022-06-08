import { vk } from '../config/vk';

export const fetchUserPhoto = (userId: number) => {
    return vk.api.users.get({
        user_ids: [userId],
        fields: ['photo_200'],
        name_case: 'nom',
    }).catch((err) => {
        return err;
    });
};
