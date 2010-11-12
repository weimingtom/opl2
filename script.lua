pl2.dofile('init.lua')
print'continue...'

--------------------------------------------------------------------------------

light1:setPosition(-1, -1, -1)
light1:setDiffuse(0.6, 0.6, 0.6)
light1:setAmbient(0.8, 0.8, 0.8)
light1:setSpecular(1.0, 1.0, 1.0)
light1:setEnabled(true)

light2:setPosition(1, 1, 1)
light2:setDiffuse(0.2, 0.2, 0.2)
light2:setAmbient(0.2, 0.2, 0.2)
light2:setSpecular(0.8, 0.8, 0.8)
light2:setEnabled(true)

camera:setEye(0, 10, -20)
camera:setFocus(0, 5, 0)
camera:setUp(0, 1, 0)
camera:setFov(35)

imo1:clear()
imo2:clear()
ani:clear()
room:clear()

--------------------------------------------------------------------------------

IMO_BODY    =  1
IMO_EYE     =  2
IMO_UNDER_A =  3
IMO_UNDER_B =  4
IMO_SOCKS   =  5
IMO_COS_A   =  6
IMO_COS_B   =  7
IMO_HEAD    =  8
IMO_FACE    =  9
IMO_NECK    = 10
IMO_ARM     = 11
IMO_SHOES   = 12
IMO_HAIR    = 13

--------------------------------------------------------------------------------

local actions = {
    --'zen_a01', 'zen_a02',
    'zen_a03', 'zen_a04', 'zen_a05', 'zen_a06', 'zen_a07', 'zen_a08',
    'sex_a01', 'sex_a02', 'sex_a03', 'sex_a04', 'sex_a05', 'sex_a06',
    --'tekoki00', 'asikoki00', 'paizuri_01', 'bak_02',
    --'vibrator00', 'vibrator01', 'vibrator02',

    zen_a01={'zen_a01_1','zen_a01_2','zen_a01_3','zen_a01_4',point='loc_pos01'},
    zen_a02={'zen_a02_1','zen_a02_2','zen_a02_3','zen_a02_4',point='loc_pos01'},
    zen_a03={'zen_a03_1','zen_a03_2','zen_a03_3','zen_a03_4',point='loc_pos01'},
    zen_a04={'zen_a04_1','zen_a04_2','zen_a04_3','zen_a04_4',point='loc_pos01'},
    zen_a05={'zen_a05_1','zen_a05_2','zen_a05_3','zen_a05_4',point='loc_pos01'},
    zen_a06={'zen_a06_1','zen_a06_2','zen_a06_3','zen_a06_4',point='loc_pos01'},
    zen_a07={'zen_a07_1','zen_a07_2','zen_a07_3','zen_a07_4',point='loc_pos01'},
    zen_a08={'zen_a08_1','zen_a08_2','zen_a08_3','zen_a08_4',point='loc_pos01'},
    sex_a01={'sex_a01_1','sex_a01_2','sex_a01_3','sex_a01_4',point='loc_pos02'},
    sex_a02={'sex_a02_1','sex_a02_2','sex_a02_3','sex_a02_4',point='loc_pos02'},
    sex_a03={'sex_a03_1','sex_a03_2','sex_a03_3','sex_a03_4',point='loc_pos02'},
    sex_a04={'sex_a04_1','sex_a04_2','sex_a04_3','sex_a04_4',point='loc_pos02'},
    sex_a05={'sex_a05_1','sex_a05_2','sex_a05_3','sex_a05_4',point='loc_pos01'},
    sex_a06={'sex_a06_1','sex_a06_2','sex_a06_3','sex_a06_4',point='loc_pos02'},
    tekoki00={'tekoki_1','tekoki_2','tekoki_3','tekoki_4',point='loc_pos01'},
    asikoki00={'ashi03_1','ashi03_2','ashi03_3','ashi03_4',point='loc_pos01'},
    paizuri_01={'sex_a02_1','sex_a02_2','sex_a02_3','sex_a02_4',point='loc_pos02'},
    bak_02={'sex_a02_1','sex_a02_2','sex_a02_3','sex_a02_4',point='loc_pos02'},
    vibrator00={'vib01_1','vib01_2','vib01_3','vib01_4',point='loc_pos01'},
    vibrator01={'vib01_1','vib01_2','vib01_3','vib01_4',point='loc_pos01'},
    vibrator02={'vib02_1','vib02_2','vib02_3','vib02_4',point='loc_pos01'},
    }

function setAnims(name,cam,_ani,_imo)
    _ani, _imo = _ani or ani, _imo or imo

    math.random()
    local a = math.random(1,#actions)
    name = name or actions[a]
    local b = math.random(1,#actions[name])
    camera:setPath(actions[name][cam] or actions[name][b])
    --print(#actions,a,#actions[name],b)
    local _ = _imo:setAnim(name..'_F') --or error('error loading animation "'..name..'_F"')
    --imo2:setAnim(name..'_F')
    local _ = _ani:setAnim(name..'_M') --or error('error loading animation "'..name..'_M"')

    local point = actions[name].point
    _ani:setPoint(point)
    _imo:setPoint(point)
    camera:setPoint(point)

--[[
    if name:sub(0,3) == 'vib' then
        imo2:clear()
        imo2:setModel(1,'vib01')
        imo2:setAnim(name..'_I')
        imo2:setPoint(point)
        imo2:setVisible(true)
    end
]]
end

--------------------------------------------------------------------------------

local chars = {
--    'fate_saber',
--    'fate_saber_lily',
--    'fate_rin_casual',
--    'fate_sakura',
--    'vocaloid_miku',
--    'eva_rei_plugsuit',
--    'eva_asuka_plugsuit',
    'moon_venus',
    'moon_pluto',
--    'brs_00',

    ani = {
        [1]="ani_bodyB_00",
    },
    ani_shorts = {
        [1]="ani_bodyC_00",
    },
    ani_nude = {
        [1]="ani_bodyA_00",
    },
    fate_saber_armor = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_saver_00",
        [3]="imo_underA_01A",
        [4]="imo_underA_01B",
        [5]="imo_socksC_00",
        [6]="imo_cos_saver_00A",
        [7]="imo_cos_saver_00B",
        [11]="imo_arm_saver_00",
        [12]="imo_shoes_saver_00",
        [13]="imo_hair_saver_00",
    },
    fate_saber = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_saver_00",
        [3]="imo_under_item_10A",
        [4]="imo_under_item_10B",
        [5]="imo_socksC_00",
        [6]="imo_cos_saver_10A",
        [7]="imo_cos_saver_10B",
        [12]="imo_shoes_saver_10",
        [13]="imo_hair_saver_00",
    },
    fate_saber_casual = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_saver_00",
        [3]="imo_underA_01A",
        [4]="imo_underA_01B",
        [5]="imo_socksC_00",
        [6]="imo_cos_saver_20A",
        [7]="imo_cos_saver_20B",
        [12]="imo_shoes_saver_20",
        [13]="imo_hair_saver_00",
    },
    fate_saber_nude = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_saver_00",
        --[3]="imo_under_item_10A",
        --[4]="imo_under_item_10B",
        --[5]="imo_socksC_00",
        --[6]="imo_cos_saver_10A",
        --[7]="imo_cos_saver_10B",
        --[12]="imo_shoes_saver_10",
        [13]="imo_hair_saver_00",
    },
    fate_rin_casual = {
        [1]="imo_bodyB_00",
        [2]="imo_eye_saver_30",
        [3]="imo_underA_01A",
        [4]="imo_underA_01B",
        [5]="imo_socksC_00",
        [6]="imo_cos_saver_30A",
        [7]="imo_cos_saver_30B",
        [12]="imo_shoes_saver_30",
        [13]="imo_hair_saver_30",
    },
    fate_rin_fuku = {
        [1]="imo_bodyB_00",
        [2]="imo_eye_saver_30",
        [3]="imo_underA_01A",
        [4]="imo_underA_01B",
        [5]="imo_socksC_00",
        [6]="imo_cos_saver_40A",
        [7]="imo_cos_saver_40B",
        [12]="imo_shoes_saver_30",
        [13]="imo_hair_saver_30",
    },
    fate_rin_nude = {
        [1]="imo_bodyB_00",
        [2]="imo_eye_saver_30",
        --[3]="imo_underA_01A",
        --[4]="imo_underA_01B",
        --[5]="imo_socksC_00",
        --[6]="imo_cos_saver_30A",
        --[7]="imo_cos_saver_30B",
        --[12]="imo_shoes_saver_30",
        [13]="imo_hair_saver_30",
    },
    fate_sakura = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_saver_50",
        [3]="imo_underA_01A",
        [4]="imo_underA_01B",
        [5]="imo_socks_item_00",
        [6]="imo_cos_saver_40A",
        [7]="imo_cos_saver_40B",
        [12]="imo_shoes_saver_30",
        [13]="imo_hair_saver_50",
    },
    fate_sakura_nude = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_saver_50",
        --[3]="imo_underA_01A",
        --[4]="imo_underA_01B",
        --[5]="imo_socks_item_00",
        --[6]="imo_cos_saver_40A",
        --[7]="imo_cos_saver_40B",
        --[12]="imo_shoes_saver_30",
        [13]="imo_hair_saver_50",
    },
    fate_saber_lily = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_saver_00",
        [3]="imo_under_item_10A",
        [4]="imo_under_item_10B",
        [5]="imo_socks_saver_60",
        [6]="imo_cos_saver_60A",
        [7]="imo_cos_saver_60B",
        [10]="imo_neck_saver_60",
        [11]="imo_arm_saver_60",
        [12]="imo_shoes_saver_60",
        [13]="imo_hair_saver_60",
    },
    fate_saber_lily_nude = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_saver_00",
        --[3]="imo_under_item_10A",
        --[4]="imo_under_item_10B",
        --[5]="imo_socks_saver_60",
        --[6]="imo_cos_saver_60A",
        --[7]="imo_cos_saver_60B",
        --[10]="imo_neck_saver_60",
        --[11]="imo_arm_saver_60",
        --[12]="imo_shoes_saver_60",
        [13]="imo_hair_saver_60",
    },
    vocaloid_miku = {
        [1]="imo_bodyA_01",
        [2]="imo_eye_vocaloid_00",
        [3]="imo_underA_06A",
        [4]="imo_underA_06B",
        [6]="imo_cos_vocaloid_00A",
        [7]="imo_cos_vocaloid_00B",
        [8]="imo_head_vocaloid_00",
        [11]="imo_arm_vocaloid_00",
        [12]="imo_shoes_vocaloid_00",
        [13]="imo_hair_vocaloid_00",
    },
    vocaloid_miku_nude = {
        [1]="imo_bodyA_01",
        [2]="imo_eye_vocaloid_00",
        --[3]="imo_underA_06A",
        --[4]="imo_underA_06B",
        --[6]="imo_cos_vocaloid_00A",
        --[7]="imo_cos_vocaloid_00B",
        [8]="imo_head_vocaloid_00",
        --[11]="imo_arm_vocaloid_00",
        --[12]="imo_shoes_vocaloid_00",
        [13]="imo_hair_vocaloid_00",
    },
    eva_rei_plugsuit = {
        [1]="imo_bodyB_01",
        [2]="imo_eye_00",
        [3]="imo_under_eva_00A",
        [5]="imo_socks_eva_00",
        [8]="imo_head_eva_00",
        [11]="imo_arm_eva_00",
        [13]="imo_hair_eva_00",
    },
    eva_rei_nude = {
        [1]="imo_bodyB_01",
        [2]="imo_eye_00",
        --[3]="imo_under_eva_00A",
        --[5]="imo_socks_eva_00",
        [8]="imo_head_eva_00",
        --[11]="imo_arm_eva_00",
        [13]="imo_hair_eva_00",
    },
    eva_asuka_plugsuit = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_eva_01",
        [3]="imo_under_eva_01A",
        [5]="imo_socks_eva_01",
        [8]="imo_head_eva_01",
        [11]="imo_arm_eva_01",
        [13]="imo_hair_eva_01",
    },
    eva_asuka_nude = {
        [1]="imo_bodyA_00",
        [2]="imo_eye_eva_01",
        --[3]="imo_under_eva_01A",
        --[5]="imo_socks_eva_01",
        [8]="imo_head_eva_01",
        --[11]="imo_arm_eva_01",
        [13]="imo_hair_eva_01",
    },
---[============================================================================[
    brs_00 = {
        [1]="imo_bodyA_01",
        [2]="imo_eye_BRS_00",
        [3]="imo_under_BRS_00A",
        [4]="imo_under_BRS_00B",
        [10]="imo_neck_BRS_00",
        [11]="imo_arm_BRS_00",
        [12]="imo_shoes_BRS_00",
        [13]="imo_hair_BRS_00",
    },
    brs_01 = {
        [1]="imo_bodyA_01",
        [2]="imo_eye_BRS_00",
        [3]="imo_under_BRS_00A",
        [4]="imo_under_BRS_00B",
        [10]="imo_neck_BRS_01",
        [11]="imo_arm_BRS_00",
        [12]="imo_shoes_BRS_00",
        [13]="imo_hair_BRS_01",
    },
    brs_02 = {
        [1]="imo_bodyA_01",
        [2]="imo_eye_BRS_00",
        [3]="imo_under_BRS_00A",
        [4]="imo_under_BRS_00B",
        [11]="imo_arm_BRS_00",
        [12]="imo_shoes_BRS_00",
        [13]="imo_hair_BRS_01",
    },
--]============================================================================]
    moon_venus = {
        [1]='imo_bodyS-Venus_00',
        [2]='imo_eye_04',
        [3]='imo_underS-Venus_00A',
        [7]='imo_cosS-Venus_00B',
        [8]='imo_headS-Venus_00',
        [9]='imo_faceS-Venus_00',
        [10]='imo_neckS-Venus_00',
        [11]='imo_armS-Venus_00',
        [12]='imo_shoesS-Venus_00',
        [13]='imo_hairS-Venus_00',
    },
    moon_venus_nude = {
        [1]='imo_bodyS-Venus_00',
        [2]='imo_eye_04',
        --[3]='imo_underS-Venus_00A',
        --[7]='imo_cosS-Venus_00B',
        [8]='imo_headS-Venus_00',
        --[9]='imo_faceS-Venus_00',
        [10]='imo_neckS-Venus_00',
        [11]='imo_armS-Venus_00',
        [12]='imo_shoesS-Venus_00',
        [13]='imo_hairS-Venus_00',
    },
    moon_pluto = {
        [1]='imo_bodyC_00',
        [2]='imo_eye_08',
        [3]='imo_underS-Pluto_00A',
        [6]='imo_cosS-Pluto_00A',
        [7]='imo_cosS-Pluto_00B',
        [8]='imo_headS-Pluto_00',
        [9]='imo_faceS-Pluto_00',
        [10]='imo_neckS-Pluto_00',
        [11]='imo_armS-Pluto_00',
        [12]='imo_shoesS-Pluto_00',
        [13]='imo_hairS-Pluto_00',
    },
    moon_pluto_nude = {
        [1]='imo_bodyC_00',
        [2]='imo_eye_08',
        --[3]='imo_underS-Pluto_00A',
        --[6]='imo_cosS-Pluto_00A',
        --[7]='imo_cosS-Pluto_00B',
        [8]='imo_headS-Pluto_00',
        --[9]='imo_faceS-Pluto_00',
        [10]='imo_neckS-Pluto_00',
        [11]='imo_armS-Pluto_00',
        [12]='imo_shoesS-Pluto_00',
        [13]='imo_hairS-Pluto_00',
    },
    moon_pluto2 = {
        [1]='imo_bodyC_00',
        [2]='imo_eye_08',
        [3]='imo_underS-Pluto_01A',
        [6]='imo_cosS-Pluto_00A',
        [7]='imo_cosS-Pluto_00B',
        [8]='imo_headS-Pluto_00',
        [9]='imo_faceS-Pluto_00',
        [10]='imo_neckS-Pluto_00',
        [11]='imo_armS-Pluto_00',
        [12]='imo_shoesS-Pluto_00',
        [13]='imo_hairS-Pluto_00',
    },
}

local rooms = {
    --'bedroom',
    'gym_storage',
    'train',
    'classroom',
    'beach',
    'furo',

    bedroom = {
        [1]="room_01",
    },
    gym_storage = {
        [1]="room_02",
    },
    train = {
        [1]="room_03",
        [2]="room_03A",
        anim="room_03A",
    },
    furo = {
        [1]="room_huro01",
    },
    classroom = {
        [1]="class01",
    },
    beach = {
        [1]="beach01a",
        [2]="beach01b",
        anim="beach01b",
    },
}

function setModels(who,what)
    who:clear()
    for i = 1, 16 do
        who:setModel(i,what[i])
    end
    if what.anim then
        who:setAnim(what.anim)
    end
end

--------------------------------------------------------------------------------

if false then
    fore:fade(1, 0)
    back:fade(1, 0)

    ani1, ani2 = ani, room
    
    room:setModels(rooms.bedroom)

    local _
    
    _ = camera:setPath('sex_a01_1') or error('failed setting camera path')
    _ = camera:setPoint('loc_pos01') or error('failed setting camera point')

    _ = imo1:setModels(chars.moon_venus_nude) or error('failed setting imo1 model')
    _ = imo1:setAnim('sex_a01_F') or error('failed setting imo1 anim')
    _ = imo1:setPoint('loc_pos01') or error('failed setting imo1 point')
    _ = ani1:setModels(chars.ani_nude) or error('failed setting ani1 model')
    _ = ani1:setAnim('sex_a01_M') or error('failed setting ani1 anim')
    _ = ani1:setPoint('loc_pos01') or error('failed setting ani1 point')
---[[
    _ = imo2:setModels(chars.moon_pluto_nude) or error('failed setting imo2 model')
    _ = imo2:setAnim('sex_a02_F') or error('failed setting imo2 anim')
    _ = imo2:setPoint('loc_pos02') or error('failed setting imo2 point')
    _ =  ani2:setModels(chars.ani_nude) or error('failed setting ani2 model')
    _ = ani2:setAnim('sex_a02_M') or error('failed setting ani2 anim')
    _ = ani2:setPoint('loc_pos02') or error('failed setting ani2 point')
--]]
    imo1:setVisible(true)
    imo2:setVisible(true)
    ani1:setVisible(true)
    ani2:setVisible(true)

    pl2.showText('')
    return
end

fore:fade(1, 0)

math.randomseed(os.time())
math.random()
---[=[
ani:setModels(chars.ani)
--ani:setVisible(true)
--ani:setBlack(true)

imo:setModels(chars[chars[math.random(1,#chars)]])
--imo:setModels(chars.moon_pluto)
--imo:setVisible(true)
--]=]
--room:setModels(rooms[rooms[math.random(1,#rooms)]])
room:setModels(rooms.train)
room:setAnim(rooms.train.anim)
room:setVisible(true)

function doScene(name)
    setAnims(name)
    print'begin scene'
    back:fade(1, 2)
    --pl2.wait(1)
---[=[
    local menu = { 'ストーリーモード', 'とことんＨモード', '終了', }
    local i = pl2.showMenu(menu)
    print('selected',i)

    pl2.showText("You selected: "..menu[i])
--]=]
--    pl2.showText("Scene: "..name)
    back:fade(0, 2)
    pl2.wait(2)
    print'end scene'
end

doScene('zen_a01')
if true then return end
doScene('zen_a03')
imo1:setModel(IMO_COS_A, nil)
imo1:setModel(IMO_NECK, nil)
doScene('zen_a04')
imo1:setModel(IMO_UNDER_A, nil)
doScene('zen_a05')
ani:setModels(chars.ani_shorts)
imo1:setModel(IMO_COS_B, nil)
doScene('zen_a06')
imo1:setModel(IMO_UNDER_B, nil)
doScene('zen_a07')
ani:setModels(chars.ani_nude)
doScene('zen_a08')

while true do
    doScene('sex_a01')
    doScene('sex_a02')
    doScene('sex_a03')
    doScene('sex_a04')
    doScene('sex_a05')
    doScene('sex_a06')
end
