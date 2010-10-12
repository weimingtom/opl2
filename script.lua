--------------------------------------------------------------------------------

light0 = pl2.light(0)
light1 = pl2.light(1)

camera = pl2.camera(0)

imo1 = pl2.character(0)
imo2 = pl2.character(1)
ani  = pl2.character(2)
room = pl2.character(3)
imo = imo1

back = pl2.layer(0)
fore = pl2.layer(1)

--------------------------------------------------------------------------------

light0:setPosition(-1, -1, -1)
light0:setDiffuse(0.6, 0.6, 0.6)
light0:setAmbient(0.8, 0.8, 0.8)
light0:setSpecular(1.0, 1.0, 1.0)
light0:setEnabled(true)

light1:setPosition(1, 1, 1)
light1:setDiffuse(0.2, 0.2, 0.2)
light1:setAmbient(0.2, 0.2, 0.2)
light1:setSpecular(0.8, 0.8, 0.8)
light1:setEnabled(true)

camera:setEye(0, 10, -20)
camera:setFocus(0, 5, 0)
camera:setUp(0, 1, 0)
camera:setFov(35)

--------------------------------------------------------------------------------

function setAnims(name)
    camera:setPath(name..'_'..tostring(math.random(1,4)))
    imo:setAnim(name..'_F')
    ani:setAnim(name..'_M')
end

local actions = {
    'sex_a01',
    'sex_a02',
    'sex_a03',
    --'sex_a04',
    --'sex_a05',
    --'sex_a06',
    --'tekoki00',
    --'paizuri_01',
    }

--------------------------------------------------------------------------------

ani:setModel(0, "ani_bodyA_00")

imo:setModel(0, "imo_bodyA_02")
imo:setModel(1, "imo_hairA_00")
imo:setModel(2, "imo_eye_00")

--imo:setModel(3, "imo_cos_saver_40A")
--imo:setModel(4, "imo_cos_saver_40B")

imo:setModel(3, "imo_cos_vocaloid_00A");
imo:setModel(4, "imo_cos_vocaloid_00B");

--imo:setAnim("event_01")

--room:setModel(0, "room_01")

--room:setModel(0, "room_02")

--room:setModel(1, "room_03")
--room:setModel(0, "room_03A")
--room:setAnim("room_03A")

--room:setModel(0, "class01")

room:setModel(0, "beach01a")
room:setModel(1, "beach01b")
room:setAnim("beach01b")

math.randomseed(os.time())
local i = math.random(1, #actions)
--setAnims(actions[i])

camera:setPath("B1cam2")
imo:setAnim("event_02")

imo:setVisible(true)
--ani:setVisible(true)
room:setVisible(true)

back:fade(1, 5)
