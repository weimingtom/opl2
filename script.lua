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

function setAnims(name)
    camera:setPath(name..'_'..tostring(math.random(1,4)))
    imo:setAnim(name..'_F')
    ani:setAnim(name..'_M')
end

local actions = {
    'zen_a01',
    'zen_a02',
    'zen_a03',
    'zen_a04',
    'zen_a05',
    'zen_a06',
    'zen_a07',
    'zen_a08',
    'sex_a01',
    'sex_a02',
    'sex_a03',
    'sex_a04',
    'sex_a05',
    'sex_a06',
    --'tekoki00',
    --'paizuri_01',
    }

--------------------------------------------------------------------------------

local chars = {
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
--[============================================================================[
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
}

local rooms = {
    bedroom = {
        [1]="room_01",
    },
    gym_storage = {
        [1]="room_02",
    },
    train = {
        [1]="room_03A",
        [2]="room_03",
        anim="room_03A",
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
    for i,v in pairs(what) do
        if tonumber(i) then who:setModel(i,v) end
    end
    if what.anim then
        who:setAnim(what.anim)
    end
end

--------------------------------------------------------------------------------

setModels(ani, chars.ani)
setModels(imo, chars.fate_rin_casual)
setModels(room, rooms.train)

math.randomseed(os.time())
local i = math.random(1, #actions)
--setAnims(actions[i])

local paths = {
    "B1cam1", "B1cam2", "B1cam3", "B2cam1",
    --"C1cam1", "C1cam2",
    "cam01", "cam02", "cam03", "cam04", "cam05",
    --"camA", "camB", "camC"
    }
local anims = { "event_01", "event_02", "event_03", "event_04", "event_05", "event_06" }

camera:setPath(paths[math.random(1,#paths)])
imo:setAnim(anims[math.random(1,#anims)])
ani:setAnim('zen_a01_M')

--ani:setVisible(true)
imo:setVisible(true)
--room:setVisible(true)

back:fade(1, 2)
